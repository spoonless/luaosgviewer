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
    IntegersScriptResultHandler() : ScriptResultHandler(2), _firstInteger(0), _secondInteger(0)
    {
    }

    void handle(lua_State *luaState)
    {
        _secondInteger = lua_tointeger(luaState, -1);
        _firstInteger = lua_tointeger(luaState, -2);
    }

    unsigned int _firstInteger;
    unsigned int _secondInteger;
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
    IntegersScriptResultHandler integersScriptResultHandler;
    osg::ref_ptr<ScriptEngine> scriptEngine = new ScriptEngine();
    std::istringstream stream("return 1, 2");

    ASSERT_TRUE(scriptEngine->exec(integersScriptResultHandler, stream));
    ASSERT_EQ(1, integersScriptResultHandler._firstInteger);
    ASSERT_EQ(2, integersScriptResultHandler._secondInteger);
}
