#ifndef SWBF_TOOLS_LUAHOOK_HPP
#define SWBF_TOOLS_LUAHOOK_HPP

#include <cstdint>

extern "C"
{
#include "ldebug.h"
#include "lstate.h"
#include "lua.h"
#include "lvm.h"
}

namespace hook
{
    class Hook;

    class LuaHook final
    {
    public:
        LuaHook(Hook* hook);

        void setProcessAddress(const std::uint32_t address);
        bool setHooks();

    private:
        Hook* m_hook;

        std::uint32_t m_address;
    };

}  // namespace hook

#endif  // SWBF_TOOLS_LUAHOOK_HPP
