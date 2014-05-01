#ifndef LUASTATE_H
#define LUASTATE_H
#include <typeinfo>
#include <climits>
#include <fstream>
#include <string>

extern "C"
{
    struct lua_State;
}

class LuaState;

/**
 * @brief The ScriptResultHandler class
 */
class LuaResultHandler
{
public:
    static const unsigned int UNLIMITED_RESULTS = UINT_MAX;

    LuaResultHandler(unsigned int getExpectedResults = UNLIMITED_RESULTS);
    virtual ~LuaResultHandler();

    virtual void handle(LuaState &luaState, unsigned int nbResults) = 0;

    unsigned int getExpectedResults()
    {
        return _expectedResults;
    }

protected:
    unsigned int _expectedResults;
};

class LuaExtensionLibrary
{
public:
    virtual ~LuaExtensionLibrary();

    virtual int open(LuaState&) = 0;

};

/**
 * @brief The LuaState class
 */
class LuaState
{
public:
    static LuaState* from(lua_State*);

    LuaState();
    virtual ~LuaState();

    inline operator lua_State*() { return _luaState; }

    inline bool good() { return _luaState != 0; }

    template<class T> bool openLibrary(const char *libname);

    template<class T> T* getLibrary();

    bool exec(LuaResultHandler &handler, std::istream &istream, const char *streamname = 0);

    bool exec(std::istream &istream, const char *streamname = 0)
    {
        return this->exec(0, istream, streamname);
    }

    const std::string lastError() const {
        return _lastError;
    }

private:
    LuaState(const LuaState&);
    const LuaState& operator == (const LuaState&);
    bool assertEngineReady();
    LuaExtensionLibrary* getLibrary(const char* internalLibname);
    bool openLibrary(LuaExtensionLibrary *library, const char *internalLibname, const char *libname);
    void pushExtensionLibraryRegistry();
    void deleteExtensionLibraryRegistry();
    bool load(std::istream &istream, const char *streamname);
    bool exec(unsigned int nbExpectedResults, std::istream &istream, const char *streamname = 0);

    lua_State *_luaState;
    std::string _lastError;
};

template<class T> T* LuaState::getLibrary()
{
    static const char * const internalLibraryName = typeid(T).name();
    T* t = 0 ;
    if (this->assertEngineReady())
    {
        t = static_cast<T*>(this->getLibrary(internalLibraryName));
    }
    return t;
}

template<class T> bool LuaState::openLibrary(const char *libname)
{
    bool result = false;
    if (assertEngineReady())
    {
        T* t = this->getLibrary<T>();
        if (t)
        {
            _lastError = "library already opened";
        }
        else {
            t = new T;
            result = this->openLibrary(t, typeid(T).name(), libname);
            if (!result)
            {
                delete t;
            }
        }
    }
    return result;
}

#endif // LUASTATE_H
