#include "EthChecks.h"
#include "Options.h"
#include "TestHelper.h"
#include "TestSuite.h"
#include "TestSuiteHelperFunctions.h"
#include <retesteth/TestOutputHelper.h>

using namespace std;
using namespace test;
using namespace test::debug;
using namespace test::testsuite;
namespace fs = boost::filesystem;

namespace
{
string const getTestNameFilter()
{
    test::Options const& opt = test::Options::get();
    string const testNameFilter = opt.singletest.name.empty() ? string() : opt.singletest.name;
    string filter = testNameFilter;
    filter += opt.singleTestNet.empty() ? string() : " " + opt.singleTestNet;
    filter += opt.getGStateTransactionFilter();
    ETH_DC_MESSAGE(
        DC::TESTLOG, "Checking test filler hashes for " + boost::unit_test::framework::current_test_case().full_name());
    if (!filter.empty())
        ETH_DC_MESSAGE(DC::STATS, "Filter: '" + filter + "'");
    return testNameFilter;
}

TestSuite::AbsoluteFilledTestPath createPathIfNotExist(TestSuite::AbsoluteFilledTestPath const& _path)
{
    if (!fs::exists(_path.path()))
    {
        ETH_DC_MESSAGE(
            DC::WARNING, "Tests folder does not exists, creating test folder: '" + string(_path.path().c_str()) + "'");
        fs::create_directories(_path.path());
    }
    return _path;
}

std::vector<fs::path> checkIfThereAreUnfilledTests(
    fs::path const& _fullPathToFillers, vector<fs::path> const& _compiledTests, string const& _testNameFilter)
{
    std::vector<fs::path> notFilledTests;
    vector<fs::path> fillerFiles = test::getFiles(_fullPathToFillers, {".json", ".yml"}, _testNameFilter);
    if (fillerFiles.size() > _compiledTests.size())
    {
        string message = "Tests are not generated: ";
        for (auto const& filler : fillerFiles)
        {
            bool found = false;
            for (auto const& filled : _compiledTests)
            {
                string const fillerName = filler.stem().string();
                if (fillerName.substr(0, fillerName.size() - 6) == filled.stem().string())
                {
                    found = true;
                    break;
                }
            }
            if (!found)
            {
                if (!Options::get().filloutdated)
                    message += "\n " + string(filler.c_str());
                notFilledTests.emplace_back(filler);
            }
        }
        if (!Options::get().filloutdated)
            ETH_ERROR_MESSAGE(message + "\n");
    }
    return notFilledTests;
}

bool fakeFilledTestsIfThereAreNone(
    vector<fs::path>& _compiledTests, string const& _testNameFilter, fs::path const& _fullPathToFillers)
{
    if (_testNameFilter.empty())
    {
        // No tests generated, check at least one filler existence
        vector<fs::path> existingFillers = test::getFiles(_fullPathToFillers, {".json", ".yml"});
        for (auto const& filler : existingFillers)
        {
            // put filler names as if it was actual tests
            string fillerName(filler.stem().c_str());
            string fillerSuffix = fillerName.substr(fillerName.size() - 6);
            if (fillerSuffix == c_fillerPostf || fillerSuffix == c_copierPostf)
                _compiledTests.emplace_back(fillerName.substr(0, fillerName.size() - 6));
        }
        return false;
    }
    else
    {
        // No tests generated and filter is set, check that filler for filter is exist
        _compiledTests.emplace_back(fs::path(_testNameFilter));  // put the test name as if it was compiled.
        return true;
    }
    return false;
}

}  // namespace

namespace test
{
std::vector<fs::path> TestSuite::checkFillerExistance(string const& _testFolder, string& testNameFilter) const
{
    testNameFilter = getTestNameFilter();
    std::vector<fs::path> outdatedTests;
    AbsoluteFilledTestPath filledTestsPath = createPathIfNotExist(getFullPathFilled(_testFolder));
    vector<fs::path> compiledTests = test::getFiles(filledTestsPath.path(), {".json", ".yml"}, testNameFilter);
    AbsoluteFillerPath fullPathToFillers = getFullPathFiller(_testFolder);

    auto const& opt = Options::get();
    if (opt.checkhash || opt.filloutdated)
        outdatedTests = checkIfThereAreUnfilledTests(fullPathToFillers.path(), compiledTests, testNameFilter);

    bool checkFillerWhenFilterIsSetButNoTestsFilled = false;
    if (compiledTests.size() == 0)
        checkFillerWhenFilterIsSetButNoTestsFilled =
            fakeFilledTestsIfThereAreNone(compiledTests, testNameFilter, fullPathToFillers.path());

    for (auto const& test : compiledTests)
    {
        fs::path const expectedFillerName = fullPathToFillers.path() / fs::path(test.stem().string() + c_fillerPostf + ".json");
        fs::path const expectedFillerName2 = fullPathToFillers.path() / fs::path(test.stem().string() + c_fillerPostf + ".yml");
        fs::path const expectedCopierName = fullPathToFillers.path() / fs::path(test.stem().string() + c_copierPostf + ".json");

        string const exceptionStr =
            checkFillerWhenFilterIsSetButNoTestsFilled ?
                "Could not find a filler for provided --singletest filter: '" + test.filename().string() + "'" :
                "Compiled test folder contains test without Filler: " + test.filename().string();

        {
            TestInfo errorInfo("CheckFillers", test.stem().string());
            TestOutputHelper::get().setCurrentTestInfo(errorInfo);
        }

        ETH_ERROR_REQUIRE_MESSAGE(
            fs::exists(expectedFillerName) || fs::exists(expectedFillerName2) || fs::exists(expectedCopierName), exceptionStr);
        ETH_ERROR_REQUIRE_MESSAGE(
            !(fs::exists(expectedFillerName) && fs::exists(expectedFillerName2) && fs::exists(expectedCopierName)),
            "Src test could either be Filler.json, Filler.yml or Copier.json: " + test.filename().string());

        // Check that filled tests created from actual fillers depenging on a test type

        auto fillerVerifier = [&testNameFilter, &outdatedTests](
                                  fs::path const& _test, fs::path const& _expectedFillerName, string const& _addPostfix) {
            auto const& opt = Options::get();
            if (!opt.filltests || opt.filloutdated)
            {
                if (checkFillerHash(_test, _expectedFillerName))
                    outdatedTests.emplace_back(_expectedFillerName);
            }
            if (!testNameFilter.empty())
            {
                testNameFilter += _addPostfix;
                return true;
            }
            return false;
        };

        if (fs::exists(expectedFillerName))
        {
            if (fillerVerifier(test, expectedFillerName, c_fillerPostf))
                return outdatedTests;
        }
        else if (fs::exists(expectedFillerName2))
        {
            if (fillerVerifier(test, expectedFillerName2, c_fillerPostf))
                return outdatedTests;
        }
        else if (fs::exists(expectedCopierName))
        {
            if (fillerVerifier(test, expectedCopierName, c_copierPostf))
                return outdatedTests;
        }
    }

    // No compiled test files. Filter is empty
    return outdatedTests;
}

}  // namespace test
