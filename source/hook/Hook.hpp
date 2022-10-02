#ifndef SWBF_TOOLS_HOOK_HPP
#define SWBF_TOOLS_HOOK_HPP

#include "MinHook.h"
#include "hook/DX9Hook.hpp"
#include "hook/LuaHook.hpp"
#include "hook/types.hpp"

namespace hook
{
    class Hook final
    {
    public:
        Hook(HMODULE module, const char* app_title, const char* executable);
        ~Hook();

        bool                startThread();
        static DWORD WINAPI runThread(LPVOID hook);
        bool                run();

        bool addHook(const Address vtable, const Address replacement, const Function original);
        bool enableHook(const Address vtable);
        bool enableAllHooks();
        bool addAndInitHook(const Address vtable, const Address replacement, const Function original);
        bool removeHook(const Address vtable);
        bool removeAllHooks();

    private:
        HMODULE     m_module  = nullptr;
        DX9Hook     m_dx9hook;
        LuaHook     m_luahook;
        const char* m_app_title;
        const char* m_executable;

        bool startMinHook();
        bool stopMinHook();
    };

}  // namespace hook

#endif  // SWBF_TOOLS_HOOK_HPP
