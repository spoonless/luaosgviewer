#include <stdio.h>
#include <string.h>
#include "lua.hpp"

static void openlib(lua_State *L, const char* name, lua_CFunction func)
{
    lua_pushcfunction(L, func);
    lua_pushstring(L, name);
    lua_call(L, 1, 0);
}

int main (void) {
  char buff[256];
  int error;
  lua_State *L = luaL_newstate();
  /* opens Lua */
  //luaL_openlibs(L);  /* opens the standard libraries */
  openlib(L, "", luaopen_base);
  openlib(L, "math", luaopen_math);
  openlib(L, "string", luaopen_string);
  openlib(L, "table", luaopen_table);
  openlib(L, "debug", luaopen_debug);
  openlib(L, "jit", luaopen_jit);

  while (fgets(buff, sizeof(buff), stdin) != NULL) {
    error = luaL_loadstring(L, buff) || lua_pcall(L, 0, 0, 0);
    if (error) {
      fprintf(stderr, "%s\n", lua_tostring(L, -1));
      lua_pop(L, 1); /* pop error message from the stack */
    }
  }
  lua_close(L);
  return 0;
}
