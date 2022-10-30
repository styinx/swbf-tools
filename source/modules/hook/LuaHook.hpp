#ifndef SWBF_TOOLS_LUAHOOK_HPP
#define SWBF_TOOLS_LUAHOOK_HPP

#include <cstdint>

extern "C"
{
#include "ldebug.h"
#include "lauxlib.h"
#include "lstate.h"
#include "luadebug.h"
#include "lualib.h"
#include "lua.h"
#include "ldo.h"
#include "lvm.h"
}

extern lua_State* gL;

namespace hook
{
    class LuaHook final
    {
    public:
        LuaHook();

        void setProcessAddress(const std::uint32_t address);
        bool setHooks();

    private:
        std::uint32_t m_address;
    };

}  // namespace hook

#endif  // SWBF_TOOLS_LUAHOOK_HPP
