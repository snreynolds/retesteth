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
 * Base functions for all test suites
 */

#include "TestSuiteHelperFunctions.h"
#include <libdevcore/CommonIO.h>
#include <retesteth/EthChecks.h>
#include <retesteth/ExitHandler.h>
#include <retesteth/Options.h>
#include <retesteth/TestHelper.h>
#include <retesteth/TestOutputHelper.h>
#include <retesteth/session/Session.h>
#include <retesteth/session/ThreadManager.h>
#include <retesteth/testSuiteRunner/TestSuite.h>
#include <retesteth/testSuites/TestFixtures.h>

using namespace std;
using namespace dev;
using namespace test;
using namespace test::testsuite;
using namespace test::debug;
using namespace test::session;
namespace fs = boost::filesystem;

namespace
{
string getTestNameFromFillerFilename(fs::path const& _fillerTestFilePath)
{
    string const fillerName = _fillerTestFilePath.stem().string();
    size_t pos = fillerName.rfind(c_fillerPostf);
    if (pos != string::npos)
        return fillerName.substr(0, pos);
    else
    {
        pos = fillerName.rfind(c_copierPostf);
        if (pos != string::npos)
            return fillerName.substr(0, pos);
        else
        {
            static string const requireStr = " require: Filler.json/Filler.yml/Copier.json";
            ETH_FAIL_REQUIRE_MESSAGE(
                false, "Incorrect file suffix in the filler folder! " + _fillerTestFilePath.string() + requireStr);
        }
    }
    return fillerName;
}
}  // namespace

namespace test
{

void TestSuite::runAllTestsInFolder(string const& _testFolder) const
{
    Options::getDynamicOptions().getClientConfigs();
    if (ExitHandler::receivedExitSignal())
        return;

    // check that destination folder test files has according Filler file in src folder
    string filter;
    std::vector<fs::path> outdatedTests;
    try
    {
        TestOutputHelper::get().setCurrentTestInfo(TestInfo("checkFillerExistance", _testFolder));
        outdatedTests = checkFillerExistance(_testFolder, filter);
    }
    catch (std::exception const&)
    {
        TestOutputHelper::get().initTest(1);
        TestOutputHelper::get().finishTest();
        return;
    }

    // run all tests
    AbsoluteFillerPath fillerPath = getFullPathFiller(_testFolder);
    if (!fs::exists(fillerPath.path()))
        ETH_WARNING(string(fillerPath.path().c_str()) + " does not exist!");
    vector<fs::path> const testFillers =
        Options::get().filloutdated ? outdatedTests : test::getFiles(fillerPath.path(), {".json", ".yml"}, filter);
    if (testFillers.size() == 0)
    {
        TestOutputHelper::get().currentTestRunPP();
        ETH_WARNING(_testFolder + " no tests detected in folder!");
    }


    // repeat this part for all connected clients
    auto thisPart = [this, &testFillers, &_testFolder]() {
        auto& testOutput = test::TestOutputHelper::get();

        if (RPCSession::isRunningTooLong() || TestChecker::isTimeConsumingTest(_testFolder.c_str()))
            RPCSession::restartScripts(true);

        testOutput.initTest(testFillers.size());
        for (auto const& testFillerPath : testFillers)
        {
            if (ExitHandler::receivedExitSignal())
                break;
            if (Options::get().lowcpu && TestChecker::isCPUIntenseTest(testFillerPath.stem().string()))
            {
                ETH_WARNING("Skipping " + testFillerPath.stem().string() + " because --lowcpu option was specified.\n");
                continue;
            }

            testOutput.showProgress();
            if (ExitHandler::receivedExitSignal())
                break;

            auto job = [this, &_testFolder, &testFillerPath]() { executeTest(_testFolder, testFillerPath); };
            ThreadManager::addTask(job);
        }
        ThreadManager::joinThreads();
        testOutput.finishTest();
    };
    runFunctionForAllClients(thisPart);
}


void TestSuite::runFunctionForAllClients(std::function<void()> _func)
{
    for (auto const& config : Options::getDynamicOptions().getClientConfigs())
    {
        Options::getDynamicOptions().setCurrentConfig(config);
        ETH_DC_MESSAGE(
            DC::STATS, "Running tests for config '" + config.cfgFile().name() + "' " + test::fto_string(config.getId().id()));

        // Run tests
        _func();

        // Disconnect threads from the client
        if (Options::getDynamicOptions().getClientConfigs().size() > 1)
            RPCSession::clear();
    }
}

void TestSuite::executeTest(string const& _testFolder, fs::path const& _fillerTestFilePath) const
{
    try
    {
        _executeTest(_testFolder, _fillerTestFilePath);
    }
    catch (std::exception const& _ex)
    {
        RPCSession::sessionEnd(TestOutputHelper::getThreadID(), RPCSession::SessionStatus::HasFinished);
    }
}

void TestSuite::_executeTest(string const& _testFolder, fs::path const& _fillerTestFilePath) const
{
    TestOutputHelper::get().setCurrentTestInfo(TestInfo("TestSuite::executeTest"));
    RPCSession::sessionStart(TestOutputHelper::getThreadID());

    // Construct output test file name
    string const testName = getTestNameFromFillerFilename(_fillerTestFilePath);
    size_t const threadID = std::hash<std::thread::id>()(TestOutputHelper::getThreadID());
    ETH_DC_MESSAGE(DC::STATS2, "Running " + testName + ": " + "(" + test::fto_string(threadID) + ")");
    AbsoluteFilledTestPath const filledTestPath = getFullPathFilled(_testFolder).path() / fs::path(testName + ".json");

    bool wereFillerErrors = Options::get().filltests;
    TestSuite::TestSuiteOptions _opt;
    if (Options::get().filltests)
        wereFillerErrors = _fillTest(_opt, _fillerTestFilePath, filledTestPath.path());

    bool disableSecondRun = false;
    auto noSecondRunConditions = [](){
        bool condition = true;
        auto const& opt = Options::get();
        condition = condition && opt.getGStateTransactionFilter().empty();
        condition = condition && !opt.vmtrace.initialized();
        condition = condition && !opt.singleTestNet.initialized();
        condition = condition && !opt.poststate.initialized();
        condition = condition && !opt.statediff.initialized();
        return !condition;
    };
    if (noSecondRunConditions() && Options::get().filltests)
    {
        ETH_WARNING("Test filter or log is set. Disabling generated test run!");
        disableSecondRun = true;
    }

    if (!wereFillerErrors && !disableSecondRun)
        _runTest(filledTestPath);

    RPCSession::sessionEnd(TestOutputHelper::getThreadID(), RPCSession::SessionStatus::HasFinished);
}

void TestSuite::_runTest(AbsoluteFilledTestPath const& _filledTestPath) const
{
    try
    {
        TestOutputHelper::get().setCurrentTestFile(_filledTestPath.path());
        executeFile(_filledTestPath.path());
    }
    catch (test::EthError const& _ex)
    {
        // Something went wrong inside the test. skip the test.
        // (error message is stored at TestOutputHelper. EthError is via ETH_ERROR_())
    }
    catch (test::UpwardsException const& _ex)
    {
        // UpwardsException is thrown upwards in tests for debug info
        // And it should be catched on upper level for report till this point
        ETH_ERROR_MESSAGE(string("Unhandled UpwardsException: ") + _ex.what());
    }
    catch (std::exception const& _ex)
    {
        if (!ExitHandler::receivedExitSignal())
            ETH_ERROR_MESSAGE("ERROR OCCURED RUNNING TESTS: " + string(_ex.what()));
        RPCSession::sessionEnd(TestOutputHelper::getThreadID(), RPCSession::SessionStatus::HasFinished);
    }
}

void TestSuite::executeFile(boost::filesystem::path const& _file) const
{
    TestSuiteOptions opt;
    static bool isLegacy = Options::get().rCurrentTestSuite.find("LegacyTests") != string::npos;
    opt.isLegacyTests = isLegacy || legacyTestSuiteFlag();

    if (_file.extension() != ".json")
        ETH_ERROR_MESSAGE("The generated test must have `.json` format!");

    ETH_DC_MESSAGE(DC::TESTLOG, "Read json structure " + string(_file.filename().c_str()));
    spDataObject res = test::readJsonData(_file);
    ETH_DC_MESSAGE(DC::TESTLOG, "Read json finish");
    doTests(res, opt);
}

TestSuite::AbsoluteFillerPath TestSuite::getFullPathFiller(string const& _testFolder) const
{
    return TestSuite::AbsoluteFillerPath(test::getTestPath() / suiteFillerFolder().path() / _testFolder);
}

TestSuite::AbsoluteFilledTestPath TestSuite::getFullPathFilled(string const& _testFolder) const
{
    return TestSuite::AbsoluteFilledTestPath(test::getTestPath() / suiteFolder().path() / _testFolder);
}

}  // namespace test
