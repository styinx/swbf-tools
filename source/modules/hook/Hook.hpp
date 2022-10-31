#ifndef SWBF_TOOLS_HOOK_HPP
#define SWBF_TOOLS_HOOK_HPP

extern "C"
{
#include "lauxlib.h"
#include "ldebug.h"
#include "ldo.h"
#include "lstate.h"
#include "lua.h"
#include "luadebug.h"
#include "lualib.h"
#include "lvm.h"
}

#include <cstdint>
#include <d3d9.h>
#include <windows.h>

namespace hook
{
    using func_luaD_call                         = void(lua_State*, StkId, int);
    using func_endScene                          = HRESULT __stdcall(IDirect3DDevice9*);
    constexpr std::uint32_t LUAD_CALL_ADDRESS    = 0x00226320;
    constexpr unsigned      END_SCENE_VTABLE_POS = 42;

    class Hook final
    {
    public:
        Hook(
            HMODULE             module,
            const char*         app_title,
            const char*         executable,
            const std::uint32_t interval = 100,
            const std::uint32_t timeout  = 0);
        ~Hook();

    private:
        std::uint32_t m_interval;
        std::uint32_t m_timeout;
        std::uint32_t m_process_id;
        std::uint32_t m_process_address;
        bool          m_running;
        HANDLE        m_process_handle;
        HMODULE       m_module;
        HWND          m_window;
        const char*   m_app_title;
        const char*   m_executable;

        static DWORD WINAPI runThread(LPVOID hook);

        bool startThread();
        bool run();

        bool findProcessID();
        bool findProcessAddress();

        bool hookLua();
        bool hookDX9();
    };

}  // namespace hook

#endif  // SWBF_TOOLS_HOOK_HPP
