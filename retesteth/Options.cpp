/*
    This file is part of cpp-ethereum.

    cpp-ethereum is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    cpp-ethereum is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with cpp-ethereum.  If not, see <http://www.gnu.org/licenses/>.
*/
/** @file
 * Class for handling testeth custom options
 */

#include "Options.h"
#include <retesteth/TestHelper.h>
#include <boost/algorithm/string.hpp>
#include <boost/test/unit_test.hpp>

using namespace std;
using namespace test;
namespace fs = boost::filesystem;
Options::DynamicOptions Options::m_dynamicOptions;

void displayTestSuites();
void printVersion()
{
    cout << prepareVersionString() << "\n";
}


#define ADD_OPTION(VAR, STR, STRHELP) \
 VAR.setDefHelp(STR, STRHELP); \
 m_options.push_back(&VAR);

#define ADD_OPTIONV(VAR, STR, STRHELP, VALIDATOR) \
 VAR.setValidator(VALIDATOR); \
 VAR.setDefHelp(STR, STRHELP); \
 m_options.push_back(&VAR);

#define ADD_OPTION_BOOST(VAR, STR, STRHELP) \
 VAR.setBeforeSeparator(); \
 VAR.setDefHelp(STR, STRHELP); \
 m_options.push_back(&VAR);

#define ADD_OPTION_OVERRIDE(VAR, STR, STRHELP) \
 VAR.setOverrideOption(); \
 VAR.setDefHelp(STR, STRHELP); \
 m_options.push_back(&VAR);

Options::Options(int argc, const char** argv)
{
    ADD_OPTION_OVERRIDE(help, "-h|--help", [](){
        printVersion();
        cout << std::left;
        cout << "\nUsage:\n";
        cout << "General options\n";
        cout << setw(30) << "-h --help" << setw(0) << "Display list of command arguments\n";
    });
    help.setValidator([this](){
        for(auto& option : m_options)
            option->printHelp();
        exit(0);
    });

    ADD_OPTION_OVERRIDE(version, "--version", [](){
        cout << setw(30) << "--version " << setw(0) << "Display build information\n";
    });
    version.setValidator([](){
            printVersion();
            exit(0);
    });

    ADD_OPTION_OVERRIDE(listsuites, "--list", [](){
        cout << setw(30) << "--list" << setw(0) << "Display available test suites\n";
    });
    listsuites.setValidator([](){
            displayTestSuites();
            exit(0);
    });

    ADD_OPTION_BOOST(rCurrentTestSuite, "-t", [](){
        cout << "\nSetting test suite\n";
        cout << setw(30) << "-t <TestSuite>" << setw(0) << "Execute test operations\n";
        cout << setw(30) << "-t <TestSuite>/<TestCase>" << setw(0) << "See `--testfile` and `--testfolder` to run custom tests\n";
        cout << "\nAll options below must follow after `--`\n";
    });
    ADD_OPTIONV(threadCount, "-j", [](){
        cout << "\nRetesteth options\n";
        cout << setw(40) << "-j <ThreadNumber>" << setw(0) << "Run test execution using threads\n";
        },[this](){
            threadCount = max((size_t)1, (size_t)threadCount);
    });
    ADD_OPTION(clients, "--clients", [](){
        cout << setw(40) << "--clients `client1, client2`" << setw(0)
             << "Use following configurations from datadir path (default: ~/.retesteth)\n";
    });
    ADD_OPTION(datadir, "--datadir", [](){
        cout << setw(40) << "--datadir" << setw(0) << "Path to configs (default: ~/.retesteth)\n";
    });
    ADD_OPTION(nodesoverride, "--nodes", [](){
        cout << setw(40) << "--nodes" << setw(0) << "List of client tcp ports (\"addr:ip, addr:ip\")\n";
        cout << setw(40) << " " << setw(0) << "|-Overrides the config file \"socketAddress\" section \n";
    });
    ADD_OPTION(testpath, "--testpath", [](){
        cout << "\nSetting test suite and test\n";
        cout << setw(40) << "--testpath <PathToTheTestRepo>" << setw(25) << "Set path to the test repo\n";
    });
    ADD_OPTION(singleTestFile, "--testfile", [](){
        cout << setw(40) << "--testfile <TestFile>" << setw(0) << "Run tests from a file. Requires -t <TestSuite>\n";
    });
    ADD_OPTION(singleTestOutFile, "--outfile", [](){
        cout << setw(40) << "--outfile <TestFile>" << setw(0) << "When using `--testfile` with `--filltests` output to this file\n";
    });
    ADD_OPTION(singletest, "--singletest", [](){
        cout << setw(40) << "--singletest <TestName>" << setw(0)
             << "Run on a single test. `Testname` is filename without Filler.json\n";
        cout << setw(40) << "--singletest <TestName>/<Subtest>" << setw(0) << "`Subtest` is a test name inside the file\n";
    });
    ADD_OPTION(singleTestNet, "--singlenet", [](){
        cout << setw(40) << "--singlenet <ForkName>" << setw(0) << "Run only specific fork configuration\n";
    });

    auto stateTestOnly = [this](string const& _name){
        if (rCurrentTestSuite.empty() || rCurrentTestSuite.find("StateTests") == string::npos)
        {
            if (rCurrentTestSuite.find("BCGeneral") != string::npos)
                return;
            BOOST_THROW_EXCEPTION(
                InvalidOption("Error: `" + _name + "` option requires `-t GeneralStateTests`"));
        }
    };
    ADD_OPTIONV(trData, "-d", [](){
        cout << "\nDebugging\n";
        cout << setw(30) << "-d <index>" << setw(25) << "Set the transaction data array index when running GeneralStateTests\n";
        cout << setw(30) << "-d <label>" << setw(25) << "Set the transaction data array label (string) when running GeneralStateTests\n";
        }, [stateTestOnly](){
            stateTestOnly("-d");
    });
    ADD_OPTIONV(trGasIndex, "-g", [](){
        cout << setw(30) << "-g <index>" << setw(25) << "Set the transaction gas array index when running GeneralStateTests\n";
        }, [stateTestOnly](){
            stateTestOnly("-g");
        });
    ADD_OPTIONV(trValueIndex, "-v", [](){
        cout << setw(30) << "-v <index>" << setw(25) << "Set the transaction value array index when running GeneralStateTests\n";
        }, [stateTestOnly](){
            stateTestOnly("-v");
    });

    ADD_OPTION(getvectors, "--getvectors", []() {
        cout << setw(30) << "--getvectors" << setw(25) << "Output all subunits of the given test (disables execution)\n";
    });
    ADD_OPTION(statediff, "--statediff", [](){
        cout << setw(30) << "--statediff" << setw(25) << "Print statediff post vs pre\n";
        cout << setw(30) << "--statediff xtoy" << setw(25) << "Statediff from block 'x' to block 'y'\n";
        cout << setw(30) << "--statediff x:ytox2:y2" << setw(25) << "Statediff from block 'x', transaction 'y' to block 'x2', transaction 'y2' (require --filltests)\n";
    });
    ADD_OPTION(vmtrace, "--vmtrace", [](){
        cout << setw(30) << "--vmtrace" << setw(25) << "Trace transaction execution\n";
        cout << setw(30) << "--vmtrace x:y" << setw(25) << "Show vmtrace of block 'x', transaction 'y'\n";
        cout << setw(30) << "--vmtrace <folder>" << setw(25) << "Trace transactions execution to a given folder\n";
        cout << setw(30) << "--vmtrace x:y <folder>" << setw(25) << "Same as above but combined\n";
    });
    ADD_OPTIONV(vmtraceraw, "--vmtraceraw", [](){
        cout << setw(30) << "--vmtraceraw" << setw(25) << "Trace transaction execution raw format\n";
        cout << setw(30) << "--vmtraceraw x:y" << setw(25) << "Show vmtrace in raw format of block 'x', transaction 'y'\n";
        cout << setw(30) << "--vmtraceraw <folder>" << setw(25) << "Trace transactions execution raw format to a given folder\n";
        cout << setw(30) << "--vmtraceraw x:y <folder>" << setw(25) << "Same as above but combined\n";
        }, [this](){
            vmtrace = true;
            vmtrace.isBlockSelected = vmtraceraw.isBlockSelected;
            vmtrace.blockNumber = vmtraceraw.blockNumber;
            vmtrace.transactionNumber = vmtraceraw.transactionNumber;
    });
    ADD_OPTIONV(vmtrace_nomemory, "--vmtrace.nomemory", [](){
        cout << setw(30) << "--vmtrace.nomemory" << setw(25) << "Disable memory in vmtrace/vmtraceraw\n";
        }, [this](){
            if (!(vmtrace.initialized() || vmtraceraw.initialized()))
                BOOST_THROW_EXCEPTION(InvalidOption("Error: --vmtrace.nomemory requires --vmtrace or --vmtraceraw"));
    });
    ADD_OPTIONV(vmtrace_nostack, "--vmtrace.nostack", [](){
            cout << setw(30) << "--vmtrace.nostack" << setw(25) << "Disable stack in vmtrace/vmtraceraw\n";
        }, [this](){
            if (!(vmtrace.initialized() || vmtraceraw.initialized()))
                BOOST_THROW_EXCEPTION(InvalidOption("Error: --vmtrace.nostack requires --vmtrace or --vmtraceraw"));
    });
    ADD_OPTIONV(vmtrace_noreturndata, "--vmtrace.noreturndata", [](){
            cout << setw(30) << "--vmtrace.noreturndata" << setw(25) << "Disable returndata in vmtrace/vmtraceraw\n";
        }, [this](){
            if (!(vmtrace.initialized() || vmtraceraw.initialized()))
                BOOST_THROW_EXCEPTION(InvalidOption("Error: --vmtrace.noreturndata requires --vmtrace or --vmtraceraw"));
    });
    ADD_OPTION(blockLimit, "--limitblocks", [](){
            cout << setw(30) << "--limitblocks" << setw(25) << "Limit the block exectuion in blockchain tests for debug\n";
    });
    ADD_OPTION(rpcLimit, "--limitrpc", [](){
        cout << setw(30) << "--limitrpc" << setw(25) << "Limit the rpc exectuion in tests for debug\n";
    });
    ADD_OPTIONV(logVerbosity, "--verbosity", [](){
        cout << setw(30) << "--verbosity <level>" << setw(25) << "Set logs verbosity. 0 - silent, 1 - only errors, 2 - informative, >2 - detailed\n";
        cout << setw(30) << "--verbosity <channel>" << setw(25)
             << "Set logs channels. 'STATS|RPC|TESTLOG|LOWLOG|SOCKET|STATE'\n";
        },[this](){
            // disable all output
            static std::ostringstream strCout;
            if (logVerbosity == 0)
            {
                std::cout.rdbuf(strCout.rdbuf());
                std::cerr.rdbuf(strCout.rdbuf());
            }
    });
    ADD_OPTION(nologcolor, "--nologcolor", [](){
        cout << setw(30) << "--nologcolor" << setw(25) << "Disable color codes in log output\n";
    });
    ADD_OPTION(exectimelog, "--exectimelog", [](){
        cout << setw(30) << "--exectimelog" << setw(25) << "Output execution time for each test suite\n";
    });
    ADD_OPTION(enableClientsOutput, "--stderr", [](){
        cout << setw(30) << "--stderr" << setw(25) << "Redirect ipc client stderr to stdout\n";
    });
    ADD_OPTION(travisOutThread, "--travisout", [](){
        cout << setw(30) << "--travisout" << setw(25) << "Output `.` to stdout\n";
    });
    ADD_OPTION(all, "--all", [](){
        cout << "\nAdditional Tests\n";
        cout << setw(30) << "--all" << setw(0) << "Enable all tests\n";
    });
    ADD_OPTION(lowcpu, "--lowcpu", [](){
        cout << setw(30) << "--lowcpu" << setw(25) << "Disable cpu intense tests\n";
    });
    ADD_OPTION(filltests, "--filltests", [](){
        cout << "\nTest Generation\n";
        cout << setw(30) << "--filltests" << setw(0) << "Run test fillers\n";
    });
    ADD_OPTIONV(filloutdated, "--filloutdated", [](){
        cout << setw(30) << "--filloutdated" << setw(0) << "Run only unfilled test fillers\n";
        },[this](){
            if (singletest.initialized() || trData.initialized() || trGasIndex.initialized() || trValueIndex.initialized())
                std::cout << "WARNING: `--filloutdated` option disables all selectors (--singletest, -d, -g, -v)\n";
            filltests = true;
    });
    ADD_OPTIONV(fillvmtrace, "--fillvmtrace", [](){
            cout << setw(30) << "--fillvmtrace" << setw(0) << "Fill tests with vmtrace information (very time consuming)\n";
        },[this](){
            filltests = true;
            vmtrace = true;
        });

    ADD_OPTION(fillchain, "--fillchain", [](){
        cout << setw(30) << "--fillchain" << setw(25) << "When filling the state tests, fill tests as blockchain instead\n";
    });
    fillchain.setValidator([this, stateTestOnly](){
            stateTestOnly("--fillchain");
            if (!filltests.initialized())
            {
                std::cout << "WARNING: `--fillchain` option provided without `--filltests`, activating `--filltests` (did you mean `--filltests`?)\n";
                filltests = true;
        }});
    ADD_OPTION(chainid, "--chainid", [](){
        cout << setw(30) << "--chainid" << setw(25) << "Override config chainid when generating transactions\n";
    });
    ADD_OPTION(showhash, "--showhash", [](){
        cout << setw(30) << "--showhash" << setw(25) << "Show filler hash debug information\n";
    });
    ADD_OPTION(checkhash, "--checkhash", [](){
        cout << setw(30) << "--checkhash" << setw(25) << "Check that tests are updated from fillers\n";
    });
    ADD_OPTIONV(poststate, "--poststate", [](){
        cout << setw(30) << "--poststate" << setw(25) << "Debug(6) show test postState hash or fullstate, when used with --filltests export `postState` in StateTests\n";
        cout << setw(30) << "--poststate x:y" << setw(25) << "Show poststate of block 'x', transaction 'y'\n";
        cout << setw(30) << "--poststate <folder>" << setw(25) << "Same as above plus export test post states into a folder\n";
        cout << setw(30) << "--poststate x:y <folder>" << setw(25) << "Same as above but combined\n";
        }, [this](){
            fullstate = true;
    });
    ADD_OPTION(fullstate, "--fullstate", [](){
        cout << setw(30) << "--fullstate" << setw(25) << "Do not compress large states to hash when debug\n";
    });
    ADD_OPTION(forceupdate, "--forceupdate", [](){
        cout << setw(30) << "--forceupdate" << setw(25) << "Update generated test (_info) even if there are no changes\n";
    });


    // Sanity check
    if ((size_t)argc > m_options.size())
        BOOST_THROW_EXCEPTION(InvalidOption("Error: Too many options provided!"));

    // Init input arguments into the list
    list<const char*> argList;
    for (int i = 1; i < argc; i++)
        argList.push_back(argv[i]);

    // Initialize options with the list, removing element on success
    for(auto& option : m_options)
        option->tryInit(argList);
    for(auto& option : m_options)
        option->validate();

    // Check leftover options in the input
    argList.remove_if([](const char * _el){ return string(_el) == "--"; });
    for(auto const& el : argList)
        BOOST_THROW_EXCEPTION(InvalidOption("Error: Dublicate or unrecognized option: `" + string(el) + "`"));

    if (threadCount == 1)
        dataobject::GCP_SPointer<int>::DISABLETHREADSAFE();
}

Options const& Options::get(int argc, const char** argv)
{
    static Options instance(argc, argv);
    return instance;
}

void displayTestSuites()
{
    cout << "List of available test suites: \n";
    cout << std::left;
    cout << setw(40) << "-t GeneralStateTests" << setw(0) << "Basic state transition tests\n";
    cout << setw(40) << "-t BCGeneralStateTests" << setw(0) << "Basic state transition tests in blockchain form\n";
    cout << setw(40) << "-t BlockchainTests" << setw(0) << "All Blockchain tests\n";
    cout << setw(40) << "-t BlockchainTests/ValidBlocks" << setw(0) << "Subset of correct blocks\n";
    cout << setw(40) << "-t BlockchainTests/InvalidBlocks" << setw(0) << "Subset of malicious blocks\n";
    cout << setw(40) << "-t BlockchainTests/TransitionTests" << setw(0) << "Subset of fork transition tests\n";
    cout << setw(40) << "-t BlockchainTests/ValidBlocks/VMTests" << setw(0)
         << "VMTests converted\n";
    cout << "(Use --filltests to generate the tests, --fillchain to generate BCGeneralStateTests)\n";

    cout << "\nLegacy test suites (Frontier .. ConstantinopleFix):\n";
    cout << setw(55) << "-t LegacyTests" << setw(0) << "All Legacy tests\n";
    cout << setw(55) << "-t LegacyTests/Constantinople" << setw(0) << "Subset of Frontier .. Constantinople tests\n";
    cout << setw(55) << "-t LegacyTests/Constantinople/GeneralStateTests" << setw(0) << "Old state tests\n";
    cout << setw(55) << "-t LegacyTests/Constantinople/BCGeneralStateTests" << setw(0) << "Old state tests in blockchain form\n";
    cout << setw(55) << "-t LegacyTests/Constantinople/BlockchainTests" << setw(0) << "Old blockchain tests\n";

    cout << "\nRetesteth unit tests:\n";
    cout << setw(30) << "-t DataObjectTestSuite" << setw(0) << "Unit tests for json parsing\n";
    cout << setw(30) << "-t EthObjectsSuite" << setw(0) << "Unit tests for test data objects\n";
    cout << setw(30) << "-t LLLCSuite" << setw(0) << "Unit tests for external lllc compiler\n";
    cout << setw(30) << "-t SOLCSuite" << setw(0) << "Unit tests for solidity support\n";
    cout << setw(30) << "-t OptionsSuite" << setw(0) << "Unit tests for this cmd menu\n";
    cout << setw(30) << "-t TestHelperSuite" << setw(0) << "Unit tests for retesteth logic\n";
    cout << "\n";
}

string Options::getGStateTransactionFilter() const
{
    string filter;
    filter += trData.index == -1 ? string() : " dInd: " + to_string(trData.index);
    filter += trData.label.empty() ? string() : " dLbl: " + trData.label;
    filter += trGasIndex == -1 ? string() : " gInd: " + to_string(trGasIndex);
    filter += trValueIndex == -1 ? string() : " vInd: " + to_string(trValueIndex);
    return filter;
}

bool Options::isLegacy()
{
    static bool isLegacy = (boost::unit_test::framework::current_test_case().full_name().find("LegacyTests") != string::npos);

    // Current test case is dynamic if we run all tests. need to see if we hit LegacyTests
    if (Options::get().rCurrentTestSuite.empty())
        isLegacy = (boost::unit_test::framework::current_test_case().full_name().find("LegacyTests") != string::npos);

    return isLegacy;
}

bool Options::Option::match(string const& _arg) const
{
    if (m_argType == ARGS::ONEMERGED)
        return m_sOptionName == _arg.substr(0, m_sOptionName.size());
    else
    {
        for (auto const& el : explode(m_sOptionName, '|'))
            if (el == _arg)
                return true;
    }
    return false;
}
bool Options::Option::isAfterSeparatorOption() const
{
    return !m_allowBeforeSeparator;
}

void Options::Option::validate() const
{
    if (initialized() && m_validatorFunc)
        m_validatorFunc();
}

void Options::Option::setDefHelp(string&& _def, std::function<void()> _help)
{
    m_sOptionName = std::move(_def);
    m_printHelpFunc = _help;
}

void Options::Option::printHelp()
{
    if (m_printHelpFunc)
        m_printHelpFunc();
}

void Options::Option::tryInit(list<const char*>& _argList)
{
    bool seenSeparator = false;
    for (list<const char*>::iterator it = _argList.begin(); it != _argList.end(); it++)
    {
        auto arg = std::string{*it};
        if (arg == "--")
            seenSeparator = true;

        if (match(arg))
        {
            if (m_optionOverrides)
            {
                m_inited = true;
                break;
            }

            if (isAfterSeparatorOption() && !seenSeparator)
                BOOST_THROW_EXCEPTION(
                    InvalidOption("Error: `" + arg + "` option appears before the separator `--`"));
            if (!isAfterSeparatorOption() && seenSeparator)
                BOOST_THROW_EXCEPTION(
                    InvalidOption("Error: `" + arg + "` option appears after the separator `--`"));

            int readArgsCount = initArgs(_argList, it);
            list<const char*>::iterator last = it;
            for (int i = 0; i < readArgsCount && last != _argList.end(); i++)
                last++;
            _argList.erase(it, last);
            break;
        }
    }
}

int Options::Option::initArgs(list<const char*> const& _argList, list<const char*>::const_iterator _arg)
{
    m_inited = m_inited || false;
    auto throwIfNoArgumentFollows = [&_argList, this](list<const char*>::const_iterator _arg) {
        auto throwException = [this](){
            BOOST_THROW_EXCEPTION(InvalidOption("Error: `" + m_sOptionName + "` option is missing an argument."));
        };
        if (++_arg == _argList.end())
            throwException();
        string nextArg{*_arg};
        if (nextArg.substr(0, 1) == "-")
            throwException();
    };

    switch(m_argType)
    {
    case ARGS::NONE:
    {
        m_inited = true;
        return 1;
    }
    case ARGS::NONE_OPTIONAL:
    {
        m_inited = true;
        if (++_arg != _argList.end())
        {
            auto const arg = string{(*_arg)};
            if (arg.substr(0, 1) != "-")
            {
                initArg(arg);
                return 2;
            }
        }
        return 1;
        break;
    }
    case ARGS::NONE_OPTIONAL2:
    {
        m_inited = true;
        if (++_arg != _argList.end())
        {
            auto const arg = string{(*_arg)};
            if (arg.substr(0, 1) != "-")
            {
                initArg(arg);
                if (++_arg != _argList.end())
                {
                    auto const arg2 = string{(*_arg)};
                    if (arg2.substr(0, 1) != "-")
                    {
                        initArg2(arg2);
                        return 3;
                    }
                }
                return 2;
            }
        }
        return 1;
        break;
    }
    case ARGS::ONEMERGED:
    {
        size_t const optNameLength = m_sOptionName.length();
        string const inputOption = string(*_arg);
        if (inputOption.length() != optNameLength)
        {
            string mergedArg = inputOption.substr(optNameLength, inputOption.length());
            initArg(mergedArg);
            m_inited = true;
            return 1;
        }
        else
        {
            throwIfNoArgumentFollows(_arg);
            initArg(string{*(++_arg)});
            m_inited = true;
            return 2;
        }
        return 1;
    }
    case ARGS::ONE:
    {
        throwIfNoArgumentFollows(_arg);
        initArg(string{*(++_arg)});
        m_inited = true;
        return 2;
    }
    default:
        m_inited = false;
    }
    return 0;
}

void Options::stringosizet_opt::initArg(std::string const& _arg)
{
    DigitsType type = test::stringIntegerType(_arg);
    if (type == DigitsType::String)
        str = _arg;
    else if (type == DigitsType::Decimal)
        val = std::max(0, atoi(_arg.c_str()));
    else
        BOOST_THROW_EXCEPTION(InvalidOption("Error: `" + m_sOptionName + "` wrong option argument format: " + _arg));
}

void Options::vecstr_opt::initArg(std::string const& _arg)
{
    std::vector<std::string> elements;
    boost::split(elements, _arg, boost::is_any_of(", "));
    for (auto& it : elements)
    {
        boost::algorithm::trim(it);
        if (!it.empty())
            m_vector.push_back(it);
    }
}

void Options::vecaddr_opt::initArg(std::string const& _arg)
{
    for (auto const& el : explode(_arg, ','))
        m_vector.push_back(IPADDRESS(el));
}

void Options::singletest_opt::initArg(std::string const& _arg)
{
    name = _arg;

    size_t pos = name.find("Filler");
    if (pos != std::string::npos)
    {
        name = name.substr(0, pos);
        std::cout << "WARNING: Correcting filter to: `" + name + "`" << std::endl;
    }
    pos = name.find_last_of('/');
    if (pos != std::string::npos)
    {
        subname = name.substr(pos + 1);
        name = name.substr(0, pos);
    }
}

void Options::dataind_opt::initArg(std::string const& _arg)
{
    DigitsType type = test::stringIntegerType(_arg);
    switch (type)
    {
    case DigitsType::Decimal:
        index = atoi(_arg.c_str());
        break;
    case DigitsType::String:
        label = _arg;
        if (_arg.find(":label") == std::string::npos)
            label = ":label " + label;
        break;
    default:
        BOOST_THROW_EXCEPTION(InvalidOption("Error: `" + m_sOptionName + "` option has wrong argument format: " + _arg));
    }
}

void Options::fspath_opt::initArg(std::string const& _arg)
{
    string_opt::initArg(_arg);
    if (!boost::filesystem::exists(_arg))
        BOOST_THROW_EXCEPTION(InvalidOption("Error: `" + m_sOptionName + "` could not locate file or path: " + _arg));
}

void Options::statediff_opt::initArg(std::string const& _arg)
{
    string const del = "to";
    size_t const pos1 = _arg.find(":");
    if (pos1 != string::npos)
    {
        size_t const pos2 = _arg.find(del);
        size_t const pos3 = _arg.find(":", pos1 + 1);
        if (pos1 != string::npos && pos2 != string::npos && pos3 != string::npos)
        {
            isBlockSelected = true;
            isTransSelected = true;
            string const block1 = _arg.substr(0, pos1);
            string const trans1 = _arg.substr(pos1 + 1, pos2 - pos1 - 1);
            string const block2 = _arg.substr(pos2 + del.size(), pos3 - pos2 - del.size());
            string const trans2 = _arg.substr(pos3 + 1);
            firstBlock = atoi(block1.c_str());
            firstTrnsx = atoi(trans1.c_str());
            seconBlock = atoi(block2.c_str());
            seconTrnsx = atoi(trans2.c_str());
        }
        else
            BOOST_THROW_EXCEPTION(InvalidOption("Error: `" + m_sOptionName + "` option arg format is x:ytox2:y2"));
    }
    else
    {
        size_t const pos2 = _arg.find(del);
        if (pos2 != string::npos)
        {
            isBlockSelected = true;
            firstTrnsx = 0;
            seconTrnsx = 0;
            isTransSelected = false;
            string const block1 = _arg.substr(0, pos2);
            string const block2 = _arg.substr(pos2 + del.size(), _arg.size() - pos2 - del.size());
            firstBlock = atoi(block1.c_str());
            seconBlock = atoi(block2.c_str());
        }
        else
            BOOST_THROW_EXCEPTION(InvalidOption("Error: `" + m_sOptionName + "` option arg format is xtoy"));
    }

    if (firstBlock > seconBlock)
        BOOST_THROW_EXCEPTION(InvalidOption("Error: `" + m_sOptionName + "` option arg format is `xtoy` or `x:ytox2:y2`, where `y >= x` or `x2 >= x`"));
}

void Options::booloutpathselector_opt::parse2OptionalArgs(std::string const& _arg)
{
    // Can take 0 args, act as bool
    // 1 arg = either path or block selector
    // 2 arg = path and block selector in any order
    size_t pos = _arg.find(":");
    if (pos != string::npos && !isBlockSelected)
    {
        blockNumber = atoi(_arg.substr(0, pos).c_str());
        transactionNumber = atoi(_arg.substr(pos + 1).c_str());
        isBlockSelected = true;
    }
    else
        outpath = _arg;
}
