#include <fstream>
#include "lua.hpp"
#include "gtest/gtest.h"
#include "osg/ref_ptr"
#include "ScriptEngine.h"

class NoopScriptResultHandler : public ScriptResultHandler
{
public:
    NoopScriptResultHandler() : _handleInvoked(false)
    {
    }

    void handle(LuaState &luaState, unsigned int nbResults)
    {
        this->_handleInvoked = true;
    }

    bool _handleInvoked;
};

class IntegersScriptResultHandler : public ScriptResultHandler
{
public:
    IntegersScriptResultHandler(unsigned int expectedResult = ScriptResultHandler::UNLIMITED_RESULTS) :
        ScriptResultHandler(expectedResult), _results(0), _nbResults(0)
    {
    }

    ~IntegersScriptResultHandler() {
        delete[] _results;
    }

    void handle(LuaState &luaState, unsigned int nbResults)
    {
        delete[] _results;
        _results = new int[nbResults];
        this->_nbResults = nbResults;
        for (unsigned int i = 1; i <= nbResults; ++i)
        {
            _results[nbResults - i] = lua_tointeger(luaState, -i);
        }
    }

    int* _results;
    unsigned int _nbResults;
};


TEST(ScriptEngine, createdWithoutError)
{
    osg::ref_ptr<ScriptEngine> scriptEngine = new ScriptEngine();

    ASSERT_EQ(std::string(), scriptEngine->lastError());
}

TEST(ScriptEngine, canExecuteLuaCode)
{
    osg::ref_ptr<ScriptEngine> scriptEngine = new ScriptEngine();
    std::istringstream stream("local a = 2+2");

    ASSERT_TRUE(scriptEngine->exec(stream));
    ASSERT_EQ(std::string(), scriptEngine->lastError());
}

TEST(ScriptEngine, canGetLastErrorWhenCompilationFailed)
{
    osg::ref_ptr<ScriptEngine> scriptEngine = new ScriptEngine();
    std::istringstream stream("non valid lua script!");

    EXPECT_FALSE(scriptEngine->exec(stream, "invalid_script"));
    ASSERT_EQ("[string \"invalid_script\"]:1: '=' expected near 'valid'", scriptEngine->lastError());
}

TEST(ScriptEngine, canGetLastErrorWhenFileDoesNotExist)
{
    osg::ref_ptr<ScriptEngine> scriptEngine = new ScriptEngine();
    std::ifstream nonExistingFile("non_existing_file");

    EXPECT_FALSE(scriptEngine->exec(nonExistingFile, "nonExistingFile"));
    ASSERT_EQ("Error while reading 'nonExistingFile'!", scriptEngine->lastError());
}

TEST(ScriptEngine, cannotExecScriptWithHandlerWhenFileDoesNotExist)
{
    osg::ref_ptr<ScriptEngine> scriptEngine = new ScriptEngine();
    std::ifstream nonExistingFile("non_existing_file");
    NoopScriptResultHandler handler;

    EXPECT_FALSE(scriptEngine->exec(handler, nonExistingFile, "nonExistingFile"));
    ASSERT_EQ("Error while reading 'nonExistingFile'!", scriptEngine->lastError());
    ASSERT_FALSE(handler._handleInvoked);
}

TEST(ScriptEngine, canExecuteLuaCodeWithNoopScriptHandler)
{
    NoopScriptResultHandler noopScriptResultHandler;
    osg::ref_ptr<ScriptEngine> scriptEngine = new ScriptEngine();
    std::istringstream stream("local a = 2+2");

    ASSERT_TRUE(scriptEngine->exec(noopScriptResultHandler, stream));
    ASSERT_TRUE(noopScriptResultHandler._handleInvoked);
}

TEST(ScriptEngine, canExecuteLuaCodeAndUseHandlerToConvertResults)
{
    IntegersScriptResultHandler integersScriptResultHandler(3);
    osg::ref_ptr<ScriptEngine> scriptEngine = new ScriptEngine();
    std::istringstream stream("local result = {}    "
                              "for i = 1, 10 do     "
                              "  result[i] = i      "
                              "end                  "
                              "return unpack(result)");

    ASSERT_TRUE(scriptEngine->exec(integersScriptResultHandler, stream));
    ASSERT_EQ(static_cast<unsigned int>(3), integersScriptResultHandler._nbResults);
    ASSERT_EQ(1, integersScriptResultHandler._results[0]);
    ASSERT_EQ(2, integersScriptResultHandler._results[1]);
    ASSERT_EQ(3, integersScriptResultHandler._results[2]);
}

TEST(ScriptEngine, canExecuteLuaCodeAndUseHandlerToConvertUnlimitedResults)
{
    IntegersScriptResultHandler integersScriptResultHandler;
    osg::ref_ptr<ScriptEngine> scriptEngine = new ScriptEngine();
    std::istringstream stream("return 1, 2");

    ASSERT_TRUE(scriptEngine->exec(integersScriptResultHandler, stream));
    ASSERT_EQ(static_cast<unsigned int>(2), integersScriptResultHandler._nbResults);
    ASSERT_EQ(1, integersScriptResultHandler._results[0]);
    ASSERT_EQ(2, integersScriptResultHandler._results[1]);
}

TEST(ScriptEngine, canExecuteLuaScriptAndDetectFailure)
{
    NoopScriptResultHandler handler;
    osg::ref_ptr<ScriptEngine> scriptEngine = new ScriptEngine();
    std::istringstream stream("assert(false)");

    ASSERT_FALSE(scriptEngine->exec(handler, stream, "failed script"));
    ASSERT_EQ("[string \"failed script\"]:1: assertion failed!", scriptEngine->lastError());
    ASSERT_FALSE(handler._handleInvoked);
}
