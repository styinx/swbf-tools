extern "C"
{
#include <lauxlib.h>
#include <lua.h>
#include <lualib.h>
}

int add(lua_State* L)
{
    if(lua_gettop(L) < 2)
        return -1;

    if(lua_isnumber(L, -1) == 1)
    {
        auto x = lua_tonumber(L, -1);
        lua_pop(L, 1);
        if(lua_isnumber(L, -1) == 1)
        {
            auto y = lua_tonumber(L, -1);
            lua_pop(L, 1);
            lua_pushnumber(L, x + y);
            return 1;
        }
        else
        {
            printf("Argument 2 is no number.");
        }
    }
    else
    {
        printf("Argument 1 is no number.");
    }
    return -1;
}

int main(int argc, char** argv)
{
    auto L = lua_open(255);
    lua_baselibopen(L);

    lua_pushcfunction(L, add);
    lua_setglobal(L, "add");

    lua_dostring(L, "print(add(2, 3))");

    return 0;
}
