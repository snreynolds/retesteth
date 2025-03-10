#include "DebugVMTrace.h"
#include <EthChecks.h>
#include <TestHelper.h>
#include <libdataobj/ConvertFile.h>
#include <testStructures/Common.h>
#include <boost/filesystem/fstream.hpp>
#include <Options.h>

using namespace std;
using namespace test::debug;
namespace fs = boost::filesystem;

const string c_tooManyRawsMessage = "==TOO MANY LOG ROWS TO PRINT (Use --vmtraceraw <folder>)==";
namespace test::teststruct
{
VMLogRecord::VMLogRecord(DataObject const& _obj)
{
    try
    {
        if (_obj.getSubObjects().size() == 2)
        {
            REQUIRE_JSONFIELDS(_obj, "VMLogRecord " + _obj.getKey(),
                {
                    {"output", {{DataType::String}, jsonField::Required}},
                    {"gasUsed", {{DataType::String}, jsonField::Required}}});
        }
        else if (_obj.getSubObjects().size() == 4)
        {
            REQUIRE_JSONFIELDS(_obj, "VMLogRecord " + _obj.getKey(),
                {
                    {"output", {{DataType::String}, jsonField::Required}},
                    {"gasUsed", {{DataType::String}, jsonField::Required}},
                    {"time", {{DataType::Integer}, jsonField::Required}},
                    {"error", {{DataType::String}, jsonField::Required}}});
        }
        else
        {
        REQUIRE_JSONFIELDS(_obj, "VMLogRecord " + _obj.getKey(),
            {{"pc", {{DataType::Integer}, jsonField::Required}},
             {"op", {{DataType::Integer}, jsonField::Required}},
             {"gas", {{DataType::String}, jsonField::Required}},
             {"gasCost", {{DataType::String}, jsonField::Required}},
             {"memory", {{DataType::String}, jsonField::Optional}},
             {"memSize", {{DataType::Integer}, jsonField::Required}},
             {"stack", {{DataType::Array}, jsonField::Required}},
             {"depth", {{DataType::Integer}, jsonField::Required}},
             {"refund", {{DataType::Integer}, jsonField::Required}},
             {"opName", {{DataType::String}, jsonField::Required}},
             {"returnData", {{DataType::String}, jsonField::Optional}},
             {"error", {{DataType::String}, jsonField::Optional}}});
        }

        if (_obj.getSubObjects().size() == 2)
        {
            isShort = true;
        }
        else if (_obj.getSubObjects().size() == 4)
        {
            error = _obj.atKey("error").asString();
            isShort = true;
        }
        else
        {
            pc = _obj.atKey("pc").asInt();
            op = _obj.atKey("op").asInt();
            gas = spVALUE(new VALUE(_obj.atKey("gas")));
            gasCost = spVALUE(new VALUE(_obj.atKey("gasCost")));
            if (_obj.count("memory"))
                memory = spBYTES(new BYTES(_obj.atKey("memory")));
            else
                memory = spBYTES(new BYTES(DataObject("0x")));
            memSize = _obj.atKey("memSize").asInt();
            for (auto const& el : _obj.atKey("stack").getSubObjects())
                stack.emplace_back(el->asString());
            if (_obj.count("returnData"))
                returnData = spBYTES(new BYTES(_obj.atKey("returnData")));
            else
                returnData = spBYTES(new BYTES(DataObject("0x")));
            depth = _obj.atKey("depth").asInt();
            refund = _obj.atKey("refund").asInt();
            opName = _obj.atKey("opName").asString();
            error = _obj.count("error") ? _obj.atKey("error").asString() : "";
        }

    }
    catch (std::exception const& _ex)
    {
        throw UpwardsException(string("VMLogRecord parse error: ") + _ex.what());
    }
}


DebugVMTrace::DebugVMTrace(
    string const& _info, string const& _trNumber, FH32 const& _trHash, boost::filesystem::path const& _logs)
{
    try
    {
        m_infoString = _info;
        m_trNumber = _trNumber;
        m_trHash = spFH32(_trHash.copy());

        string line;
        size_t k = 0;
        const size_t c_maxRowsToPrint = 300;
        if (!fs::exists(_logs))
            return;

        fs::ifstream fileHandler(_logs);
        auto readLog = [this](string const& _line){
            auto const data = ConvertJsoncppStringToData(_line);
            if (data->getSubObjects().size() == 3)
            {
                m_output = data->atKey("output").asString();
                m_gasUsed = spVALUE(new VALUE(data->atKey("gasUsed")));
                m_time = data->atKey("time").asInt();
            }
            else
                m_log.emplace_back(VMLogRecord(data));
        };

        if (Options::get().fillvmtrace)
        {
            // Load logs optimized
            while (getline(fileHandler, line))
                readLog(line);
        }
        else
        {
            while (getline(fileHandler, line))
            {
                if (++k < c_maxRowsToPrint)
                {
                    m_rawUnparsedLogs += line + "\n";
                    readLog(line);
                }
                else if (k == c_maxRowsToPrint)
                {
                    m_limitReached = true;
                    m_rawUnparsedLogs += c_tooManyRawsMessage;
                    m_output = "";
                    m_gasUsed = spVALUE(new VALUE(DataObject("0x00")));
                    m_time = 0;
                    break;
                }
            }
        }
        fileHandler.close();

        // Take a handle of t8ntool file in our own tmp path
        auto const uniqueFolder = fs::unique_path();
        fs::create_directory(_logs.parent_path().parent_path() / uniqueFolder);
        m_rawVmTraceFile = _logs.parent_path().parent_path() / uniqueFolder / _logs.stem();
        fs::rename(_logs, m_rawVmTraceFile);
    }
    catch (std::exception const& _ex)
    {
        throw UpwardsException(string("DebugVMTrace parse error: ") + _ex.what());
    }
}

void DebugVMTrace::print()
{
    ETH_DC_MESSAGE(DC::DEFAULT, m_infoString);
    ETH_DC_MESSAGE(DC::DEFAULT, m_rawUnparsedLogs);
}

void DebugVMTrace::printNice()
{
    ETH_DC_MESSAGE(DC::DEFAULT, m_infoString);
    if (m_log.size() == 0)
        return;

    string s_comment = "";
    dev::bigint maxGas = m_log.at(0).isShort ? 500000 : m_log.at(0).gas->asBigInt();
    size_t k = 0;
    size_t const step = 9;
    string const stepw = "          ";

    std::cout << cBYellowBlack << "N" << setw(15) << "OPNAME" << setw(10) << "GASCOST" << setw(10) << "TOTALGAS" << setw(10)
              << "REMAINGAS" << setw(10) << "STACK" << cDefault << std::endl;
    for (VMLogRecord const& el : m_log)
    {
        // Last record with error info
        if (el.isShort)
            continue;

        if (!s_comment.empty())
        {
            std::cout << setw(step * el.depth) << cYellow << s_comment << cDefault << std::endl;
            s_comment = string();
        }
        std::cout << setw(step * (el.depth - 1));
        std::cout << test::fto_string(k++) + "-" + test::fto_string(el.depth)
                  << setw(15) << el.opName
                  << setw(10) << el.gasCost->asDecString()
                  << setw(10) << maxGas - el.gas->asBigInt()
                  << setw(10) << el.gas->asDecString();

        const size_t c_stackPreviewSize = 10;
        if (el.stack.size())
            std::cout << setw(5) <<  el.stack.size() << ":[";
        for (auto const& stackEl : el.stack)
        {
            if (stackEl.size() > c_stackPreviewSize)
                std::cout << stackEl.substr(2, c_stackPreviewSize) << "...,";
            else
                std::cout << stackEl.substr(2) << ",";
        }

        if (el.stack.size())
            std::cout << "]";
        std::cout << std::endl;
        if (el.error.size())
            std::cout << "Detected error: " << el.error << std::endl;

        // Opcode highlight
        static vector<string> callopcodes = { "CALLCODE", "CALL", "DELEGATECALL" };
        if ( inArray(callopcodes, el.opName) && el.stack.size() > 1)
            s_comment = "SUBCALL: " + el.stack.at(el.stack.size() - 2);
        if (el.opName == "SSTORE" && el.stack.size() > 1)
            s_comment = stepw + "SSTORE [" + el.stack.at(el.stack.size() - 1) + "] = " + el.stack.at(el.stack.size() - 2);
        if (el.opName == "MSTORE" && el.stack.size() > 1)
            s_comment = stepw + "MSTORE [" + el.stack.at(el.stack.size() - 1) + "] = " + el.stack.at(el.stack.size() - 2);
        if (el.opName == "RETURN")
            s_comment = stepw + "RETURN " + el.memory->asString();
    }
    if (m_limitReached)
        std::cout << c_tooManyRawsMessage << std::endl;
    std::cout << std::endl;
}

DebugVMTrace::~DebugVMTrace()
{
    if (!m_rawVmTraceFile.empty())
    {
        boost::filesystem::remove(m_rawVmTraceFile);
        boost::filesystem::remove(m_rawVmTraceFile.parent_path());
    }
}

void DebugVMTrace::exportLogs(fs::path const& _folder)
{
    try
    {
        if (!fs::exists(_folder.parent_path()))
            fs::create_directories(_folder.parent_path());
        fs::rename(m_rawVmTraceFile, _folder);
    }
    catch (std::exception const& _ex)
    {
        try
        {
            fs::copy(m_rawVmTraceFile, _folder);
            fs::remove(m_rawVmTraceFile);
            fs::remove(m_rawVmTraceFile.parent_path());
        }
        catch (std::exception const& _ex)
        {
            throw UpwardsException(string("DebugVMTrace::exportLogs error: ") + _ex.what());
        }
    }
}

}  // namespace teststruct
