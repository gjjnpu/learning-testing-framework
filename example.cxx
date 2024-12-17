#include <NXOpen/AutomatedTesting_CppUnitTest.hxx>
#include <NXOpen/AutomatedTesting_TestCaseManager.hxx>

using namespace NXOpen;
using namespace NXOpen::AutomatedTesting;

/*-------------------------------------------------------------------------------------------------------------*/
TestFixture::~TestFixture()
{
}

/*-------------------------------------------------------------------------------------------------------------*/
void TestFixture::SetUpClass()
{
}

/*-------------------------------------------------------------------------------------------------------------*/
void TestFixture::TearDownClass()
{
}

/*-------------------------------------------------------------------------------------------------------------*/
void TestFixture::SetUp()
{
}

/*-------------------------------------------------------------------------------------------------------------*/
void TestFixture::TearDown()
{
}

/*-------------------------------------------------------------------------------------------------------------*/
TestCase::TestCase(NXOpen::NXString funcName) :m_functioName(funcName), m_wrappedFixture(nullptr)
{
}

/*-------------------------------------------------------------------------------------------------------------*/
TestCase::~TestCase()
{
}

/*-------------------------------------------------------------------------------------------------------------*/
NXOpen::NXString TestCase::GetName()
{
    return m_functioName;
}

/*-------------------------------------------------------------------------------------------------------------*/
void TestCase::SetWrappedFixture(TestFixture* fixture)
{
    m_wrappedFixture = fixture;
}

/*-------------------------------------------------------------------------------------------------------------*/
TestFixture* TestCase::GetWrappedFixture()
{
    return m_wrappedFixture;
}

/*-------------------------------------------------------------------------------------------------------------*/
void TestCase::SetUp()
{
}

/*-------------------------------------------------------------------------------------------------------------*/
void TestCase::TearDown()
{
}

/*-------------------------------------------------------------------------------------------------------------*/
void TestCase::SetUpClass()
{
}

/*-------------------------------------------------------------------------------------------------------------*/
void TestCase::TearDownClass()
{
}

/*-------------------------------------------------------------------------------------------------------------*/
TestSuite::TestSuite()
{
}

/*-------------------------------------------------------------------------------------------------------------*/
TestSuite::~TestSuite()
{
}

/*-------------------------------------------------------------------------------------------------------------*/
void TestSuite::AddTest(TestCase* test)
{
    m_tests.push_back(test);
}

/*-------------------------------------------------------------------------------------------------------------*/
#if defined (WNT)
static void dothing()
{}
#endif

/*-------------------------------------------------------------------------------------------------------------*/
static NXOpen::NXString  getDllName()
{
#if defined (WNT)
    static char str[MAX_PATH];
    static bool first = true;
    if (first)
    {
        HMODULE hModule = nullptr;
        GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS,
            (LPCSTR)&dothing,
            &hModule);
        GetModuleFileNameA(hModule, str, sizeof str);
        first = false;
    }

    return NXOpen::NXString(str);
#else
    return NXOpen::NXString();
#endif
}

/*-------------------------------------------------------------------------------------------------------------*/
void TestSuite::Run()
{
    NXString dllName = getDllName();
  
    NXOpen::Session* theSession = NXOpen::Session::GetSession();
    AutomatedTesting::TestingManager* atsManager = theSession->AutomatedTestingManager();
    std::vector<NXString> subTestIds = {};
    std::vector<NXString> resourceFiles = {};
    NXString isManaged = "undefined";
    for (TestCase* testItem : m_tests)
    {
        TestFixture* wrappedFixture = testItem->GetWrappedFixture();
        resourceFiles = wrappedFixture->resourceFiles;
        NXString testCaseID = wrappedFixture->classId + "_" + testItem->GetName();
        NXString testCaseCategory = wrappedFixture->category + "." + wrappedFixture->displayName;
        isManaged = wrappedFixture->isManagedMode ? NXString("true") : NXString("false");

        std::vector<NXString> caseParams = { dllName,testCaseID,testCaseCategory,testItem->GetName(),
            wrappedFixture->description, wrappedFixture->nxVersion, isManaged, NXString(""),wrappedFixture->classId };

        Callback0MemberFunction<TestCase, void>   setUpMethod(testItem, &TestCase::SetUp);
        Callback0MemberFunction<TestCase, void>   runTestMethod(testItem, &TestCase::Run);
        Callback0MemberFunction<TestCase, void>   tearDownMethod(testItem, &TestCase::TearDown);

        std::vector<NXString> subTest = {};

        atsManager->TestCaseManager()->CreateTestDefinition(caseParams, resourceFiles, subTest, wrappedFixture->interactive, setUpMethod, runTestMethod, tearDownMethod);

        subTestIds.push_back(testCaseID);
    }

    TestCase * testOne = m_tests[0];

    TestFixture* wrappedFixtureOne = testOne->GetWrappedFixture();

    Callback0MemberFunction<TestCase, void>   setUpClassMethod(testOne, &TestCase::SetUpClass);
    Callback0MemberFunction<TestCase, void>   runNothing(testOne, &TestCase::Run);
    Callback0MemberFunction<TestCase, void>   tearDownClassMethod(testOne, &TestCase::TearDownClass);

    NXString isCloseAll = wrappedFixtureOne->closeAllParts ? NXString("true") : NXString("false");
    std::vector<NXString> suiteParams = { dllName,wrappedFixtureOne->classId, wrappedFixtureOne->category,
        wrappedFixtureOne->displayName, wrappedFixtureOne->description, wrappedFixtureOne->nxVersion,isManaged, isCloseAll,
        wrappedFixtureOne->stopOnOption };
    atsManager->TestCaseManager()->CreateTestDefinition(suiteParams, resourceFiles, subTestIds, wrappedFixtureOne->interactive,
        setUpClassMethod, runNothing, tearDownClassMethod);
}
