#include "LuaHook.hpp"

#include "modules/hook/types.hpp"

#include "detours.h"
#include "spdlog/sinks/basic_file_sink.h"
#include "spdlog/spdlog.h"

lua_State* gL = nullptr;

namespace hook
{
    extern "C"
    {
        void call_hook_func(lua_State* L, lua_Debug* ar)
        {
            auto logger = spdlog::get(__FILE__);
            logger->debug("{:>10}: {:>10} ({:>10}) {:>10}", ar->event, ar->currentline, ar->linedefined, ar->nups);
        }

        using func_luaD_call = void(lua_State*, StkId, int);
        func_luaD_call*         orig_luaD_call;
        constexpr std::uint32_t luaD_call_address = 0x00226320;
        void                    hook_luaD_call(lua_State* L, StkId func, int nResults)
        {
            auto logger = spdlog::get(__FILE__);
            logger->debug("Call hook_luaD_call");

            logger->debug("Get state");
            gL = L;

            logger->debug("set hook");
            //lua_setcallhook(L, call_hook_func);

            // Detach the hook once we get the lua state.
            DetourTransactionBegin();
            DetourUpdateThread(GetCurrentThread());
            DetourDetach(&orig_luaD_call, hook_luaD_call);
            DetourTransactionCommit();

            if(orig_luaD_call)
                orig_luaD_call(L, func, nResults);
        }
    }

    LuaHook::LuaHook()
    {
        auto logger = spdlog::basic_logger_mt(__FILE__, "LuaHook.log", true);
        spdlog::set_level(spdlog::level::debug);
        logger->debug("Start Lua hook");
    }

    void LuaHook::setProcessAddress(const std::uint32_t address)
    {
        m_address = address;
    }

    bool LuaHook::setHooks()
    {
        auto logger = spdlog::get(__FILE__);
        logger->debug("Set Hooks");

        bool success = false;

        if(DetourTransactionBegin() == NO_ERROR)
        {
            logger->debug("Attempt hooking luaD call ");
            orig_luaD_call = reinterpret_cast<func_luaD_call*>(m_address + luaD_call_address);
            if(DetourAttach(&reinterpret_cast<PVOID&>(orig_luaD_call), &hook_luaD_call) == NO_ERROR)
            {
                logger->debug("Found");
                if(DetourTransactionCommit() == NO_ERROR)
                {
                    logger->debug("Done");
                    success = true;
                }
            }
        }

        return success;
    }
}  // namespace hook
