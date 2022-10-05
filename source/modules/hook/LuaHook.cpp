#include "LuaHook.hpp"

#include "modules/hook/Hook.hpp"

#include <detours.h>
#include <fstream>

extern "C"
{
    //void luaD_call (lua_State *L, StkId func, int nResults);
    const int lua_calloffset = 0x00626320; // 0x00226320;
    void(__cdecl* originallua_call)(lua_State *, StkId, int) = (void(__cdecl*)(lua_State *, StkId , int))(lua_calloffset);
    void  __cdecl hookluaD_call(lua_State *L, StkId func, int nResults);

    //void luaB_call (lua_State *L);
    //typedef void(__cdecl *originallua_call2)(lua_State *, StkId, int);
    //originallua_call2 originallua_call_func;

    static bool init = false;
    void __cdecl callHook(lua_State* L, lua_Debug* D)
    {
        if(D)
        {

        }
    }

    void __cdecl  hookluaD_call(lua_State* L, StkId func, int nResults)
    {
        if(!init) {
            lua_setcallhook(L, callHook);
            init = true;
        }
        originallua_call(L, func, nResults);
    }
}

namespace hook
{


    LuaHook::LuaHook(Hook* hook)
        : m_hook(hook)
    {
    }

    void LuaHook::setProcessAddress(const std::uint32_t address)
    {
        m_address = address;
    }

    bool LuaHook::setHooks()
    {
        bool success = false;

        if (DetourTransactionBegin() == NO_ERROR) {
            if (DetourAttach((PVOID*)(&originallua_call), (PVOID)hookluaD_call) == NO_ERROR) {
                if (DetourTransactionCommit() == NO_ERROR) {
                    success = true;
                }
            }
        }

        return success;
    }
}  // namespace hook
