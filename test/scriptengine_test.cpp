#include <fstream>
#include "osg/ref_ptr"
#include "gtest/gtest.h"
#include "scriptengine.h"
#include "lua.hpp"

class NoopScriptResultHandler : public ScriptResultHandler
{
public:
    NoopScriptResultHandler() : _handleInvoked(false)
    {
    }

    void handle(lua_State *luaState)
    {
        this->_handleInvoked = true;
    }

    bool _handleInvoked;
};

class IntegersScriptResultHandler : public ScriptResultHandler
{
public:
    IntegersScriptResultHandler(unsigned int expectedResult) : ScriptResultHandler(expectedResult)
    {
        _results = new int[expectedResult];
    }

    ~IntegersScriptResultHandler() {
        delete[] _results;
    }

    void handle(lua_State *luaState)
    {
        for (unsigned int i = 1; i <= this->expectedResults(); ++i)
        {
            _results[this->expectedResults() - i] = lua_tointeger(luaState, -i);
        }
    }

    int* _results;
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

TEST(ScriptEngine, canExecuteLuaCodeWithNoopScriptHandler)
{
    NoopScriptResultHandler noopScriptResultHandler;
    osg::ref_ptr<ScriptEngine> scriptEngine = new ScriptEngine();
    std::istringstream stream("local a = 2+2");

    ASSERT_TRUE(scriptEngine->exec(noopScriptResultHandler, stream));
    ASSERT_TRUE(noopScriptResultHandler._handleInvoked);
}

TEST(ScriptEngine, canExecuteLuaCodeAndUseHandlerToConvertResult)
{
    IntegersScriptResultHandler integersScriptResultHandler(3);
    osg::ref_ptr<ScriptEngine> scriptEngine = new ScriptEngine();
    std::istringstream stream("local result = {}    "
                              "for i = 1, 10 do     "
                              "  result[i] = i      "
                              "end                  "
                              "return unpack(result)");

    ASSERT_TRUE(scriptEngine->exec(integersScriptResultHandler, stream));
    ASSERT_EQ(1, integersScriptResultHandler._results[0]);
    ASSERT_EQ(2, integersScriptResultHandler._results[1]);
    ASSERT_EQ(3, integersScriptResultHandler._results[2]);
}
