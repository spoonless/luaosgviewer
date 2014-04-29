#include <fstream>
#include "luaX.h"
#include "gtest/gtest.h"
#include "LuaState.h"

class SimpleLuaExtensionLib : public LuaExtensionLibrary
{
public:
    virtual int open(LuaState &luaState)
    {
        libname = lua_tostring(luaState, -1);
        return 0;
    }

    std::string libname;
};

class FailureLuaExtensionLib : public LuaExtensionLibrary
{
public:
    virtual int open(LuaState &luaState)
    {
        lua_pushstring(luaState, "FailedLuaExtensionLib");
        lua_error(luaState);
        return 0;
    }
};

class NoopScriptResultHandler : public LuaResultHandler
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

class IntegersScriptResultHandler : public LuaResultHandler
{
public:
    IntegersScriptResultHandler(unsigned int expectedResult = LuaResultHandler::UNLIMITED_RESULTS) :
        LuaResultHandler(expectedResult), _results(0), _nbResults(0)
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


TEST(LuaStateTest, createdWithoutError)
{
    LuaState luaState;

    ASSERT_EQ(std::string(), luaState.lastError());
}

TEST(LuaStateTest, canGetLuaStateFromLua_State)
{
    LuaState luaState;

    ASSERT_EQ(&luaState, LuaState::from(luaState));

    ASSERT_EQ(0, LuaState::from(0));
}

TEST(LuaStateTest, canOpenLibrary)
{
    LuaState luaState;

    bool result = luaState.openLibrary<SimpleLuaExtensionLib>("simpleLib");

    ASSERT_TRUE(result);

    SimpleLuaExtensionLib* lib = luaState.getLibrary<SimpleLuaExtensionLib>();

    ASSERT_TRUE(lib);
    ASSERT_EQ("simpleLib", lib->libname);
}

TEST(LuaStateTest, cannotOpenLibraryTwice)
{
    LuaState luaState;

    bool result = luaState.openLibrary<SimpleLuaExtensionLib>("simpleLib");

    ASSERT_TRUE(result);

    result = luaState.openLibrary<SimpleLuaExtensionLib>("anotherTry");

    ASSERT_FALSE(result);

    ASSERT_EQ("library already opened", luaState.lastError());
}

TEST(LuaStateTest, cannotOpenFailureLibrary)
{
    LuaState luaState;

    bool result = luaState.openLibrary<FailureLuaExtensionLib>("failureLib");

    ASSERT_FALSE(result);

    ASSERT_EQ("FailedLuaExtensionLib", luaState.lastError());

    FailureLuaExtensionLib* lib = luaState.getLibrary<FailureLuaExtensionLib>();

    ASSERT_FALSE(lib);
}

TEST(LuaStateTest, cannotGetUnopenedLibrary)
{
    LuaState luaState;

    SimpleLuaExtensionLib* lib = luaState.getLibrary<SimpleLuaExtensionLib>();

    ASSERT_FALSE(lib);
}

TEST(LuaStateTest, canExecuteLuaCode)
{
    LuaState luaState;
    std::istringstream stream("local a = 2+2");

    ASSERT_TRUE(luaState.exec(stream));
    ASSERT_EQ(std::string(), luaState.lastError());
}

TEST(LuaStateTest, canGetLastErrorWhenCompilationFailed)
{
    LuaState luaState;
    std::istringstream stream("non valid lua script!");

    EXPECT_FALSE(luaState.exec(stream, "invalid_script"));
    ASSERT_EQ("[string \"invalid_script\"]:1: '=' expected near 'valid'", luaState.lastError());
}

TEST(LuaStateTest, canGetLastErrorWhenFileDoesNotExist)
{
    LuaState luaState;
    std::ifstream nonExistingFile("non_existing_file");

    EXPECT_FALSE(luaState.exec(nonExistingFile, "nonExistingFile"));
    ASSERT_EQ("Error while reading 'nonExistingFile'!", luaState.lastError());
}

TEST(LuaStateTest, cannotExecScriptWithHandlerWhenFileDoesNotExist)
{
    LuaState luaState;
    std::ifstream nonExistingFile("non_existing_file");
    NoopScriptResultHandler handler;

    EXPECT_FALSE(luaState.exec(handler, nonExistingFile, "nonExistingFile"));
    ASSERT_EQ("Error while reading 'nonExistingFile'!", luaState.lastError());
    ASSERT_FALSE(handler._handleInvoked);
}

TEST(LuaStateTest, canExecuteLuaCodeWithNoopScriptHandler)
{
    LuaState luaState;
    NoopScriptResultHandler noopScriptResultHandler;
    std::istringstream stream("local a = 2+2");

    ASSERT_TRUE(luaState.exec(noopScriptResultHandler, stream));
    ASSERT_TRUE(noopScriptResultHandler._handleInvoked);
}

TEST(LuaStateTest, canExecuteLuaCodeAndUseHandlerToConvertResults)
{
    LuaState luaState;
    IntegersScriptResultHandler integersScriptResultHandler(3);
    std::istringstream stream("local result = {}    "
                              "for i = 1, 10 do     "
                              "  result[i] = i      "
                              "end                  "
                              "return unpack(result)");

    ASSERT_TRUE(luaState.exec(integersScriptResultHandler, stream));
    ASSERT_EQ(static_cast<unsigned int>(3), integersScriptResultHandler._nbResults);
    ASSERT_EQ(1, integersScriptResultHandler._results[0]);
    ASSERT_EQ(2, integersScriptResultHandler._results[1]);
    ASSERT_EQ(3, integersScriptResultHandler._results[2]);
}

TEST(LuaStateTest, canExecuteLuaCodeAndUseHandlerToConvertUnlimitedResults)
{
    LuaState luaState;
    IntegersScriptResultHandler integersScriptResultHandler;
    std::istringstream stream("return 1, 2");

    ASSERT_TRUE(luaState.exec(integersScriptResultHandler, stream));
    ASSERT_EQ(static_cast<unsigned int>(2), integersScriptResultHandler._nbResults);
    ASSERT_EQ(1, integersScriptResultHandler._results[0]);
    ASSERT_EQ(2, integersScriptResultHandler._results[1]);
}

TEST(LuaStateTest, canExecuteLuaScriptAndDetectFailure)
{
    LuaState luaState;
    NoopScriptResultHandler handler;
    std::istringstream stream("assert(false)");

    ASSERT_FALSE(luaState.exec(handler, stream, "failed script"));
    ASSERT_EQ("[string \"failed script\"]:1: assertion failed!", luaState.lastError());
    ASSERT_FALSE(handler._handleInvoked);
}
