#include <fstream>
#include "gtest/gtest.h"
#include "scriptengine.h"


TEST(ScriptEngine, createdWithoutError)
{
    ScriptEngine scriptEngine;

    ASSERT_EQ(std::string(), scriptEngine.lastError());
}

TEST(ScriptEngine, canExecuteLuaCode)
{
    ScriptEngine scriptEngine;
    std::istringstream stream("local a = 2+2");

    ASSERT_TRUE(scriptEngine.exec(stream));
    ASSERT_EQ(std::string(), scriptEngine.lastError());
}

TEST(ScriptEngine, canGetLastErrorWhenCompilationFailed)
{
    ScriptEngine scriptEngine;
    std::istringstream stream("non valid lua script!");

    EXPECT_FALSE(scriptEngine.exec(stream, "invalid_script"));
    ASSERT_EQ("[string \"invalid_script\"]:1: '=' expected near 'valid'", scriptEngine.lastError());
}

TEST(ScriptEngine, canGetLastErrorWhenFileDoesNotExist)
{
    ScriptEngine scriptEngine;
    std::ifstream nonExistingFile("non_existing_file");

    EXPECT_FALSE(scriptEngine.exec(nonExistingFile, "nonExistingFile"));
    ASSERT_EQ("Error while reading 'nonExistingFile'!", scriptEngine.lastError());
}
