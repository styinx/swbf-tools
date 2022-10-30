#ifndef SWBF_TOOLS_HOOK_HPP
#define SWBF_TOOLS_HOOK_HPP

#include "modules/hook/DX9Hook.hpp"
#include "modules/hook/LuaHook.hpp"
#include "modules/hook/types.hpp"

namespace hook
{
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
        DX9Hook       m_dx9hook;
        LuaHook       m_luahook;
        const char*   m_app_title;
        const char*   m_executable;

        static DWORD WINAPI runThread(LPVOID hook);

        bool startThread();
        bool run();

        bool findProcessID();
        bool findProcessAddress();
    };

}  // namespace hook

#endif  // SWBF_TOOLS_HOOK_HPP
