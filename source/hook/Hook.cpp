#include "Hook.hpp"

#include "hook/IMGUIWindow.hpp"

#include <Psapi.h>
#include <TlHelp32.h>

#include <fstream>

namespace hook
{
    Hook::Hook(HMODULE module, const char* app_title, const char* executable)
        : m_module(module)
        , m_dx9hook(this)
        , m_luahook(this)
        , m_app_title(app_title)
        , m_executable(executable)
    {
        startMinHook();
    }

    Hook::~Hook()
    {
        removeAllHooks();
        stopMinHook();
    }

    // Private

    bool Hook::startMinHook()
    {
        return MH_Initialize() == MH_OK;
    }

    bool Hook::stopMinHook()
    {
        return MH_Uninitialize() == MH_OK;
    }

    // Public

    bool Hook::startThread()
    {
        HANDLE thread = CreateThread(nullptr, 0, &hook::Hook::runThread, this, 0, nullptr);
        if(thread != nullptr)
        {
            CloseHandle(thread);
            return true;
        }
        return false;
    }

    DWORD WINAPI Hook::runThread(LPVOID hook)
    {
        if(hook)
        {
            auto* hook_instance = reinterpret_cast<Hook*>(hook);
            return hook_instance->run();
        }
        return FALSE;
    }

    bool Hook::run()
    {
        auto module = reinterpret_cast<HMODULE>(m_module);
        if(!module)
        {
            return false;
        }

        auto window = reinterpret_cast<HWND>(FindWindow(nullptr, m_app_title));
        if(!window)
        {
            return false;
        }

        uint32_t     interval        = 500;
        uint32_t m_process_id = 0;
        uint32_t m_process_address = 0;
        HANDLE m_process_handle = nullptr;

        while(m_process_id == 0)
        {
            HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
            if(hSnap == INVALID_HANDLE_VALUE)
                break;

            PROCESSENTRY32 process{};
            process.dwSize = sizeof(PROCESSENTRY32);

            if(Process32First(hSnap, &process))
            {
                do
                {
                    std::string  proc_name = process.szExeFile;
                    if(proc_name == m_executable)
                    {
                        m_process_id = process.th32ProcessID;
                        break;
                    }
                } while(Process32Next(hSnap, &process));
            }

            CloseHandle(hSnap);
            Sleep(interval);
        }
        m_process_handle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, m_process_id);

        HMODULE modules[256];
        TCHAR   szBuf[MAX_PATH];
        DWORD   cModules;

        EnumProcessModules(m_process_handle, modules, sizeof(HMODULE) * 256, &cModules);

        if(EnumProcessModules(m_process_handle, modules, cModules / sizeof(HMODULE), &cModules))
        {
            for(int i = 0; i < cModules / sizeof(HMODULE); i++)
            {
                if(GetModuleBaseName(m_process_handle, modules[i], szBuf, 256))
                {
                    std::string  l{szBuf};
                    if(l == m_executable)
                    {
                        m_process_address = *reinterpret_cast<uint32_t*>(modules[i]);
                        break;
                    }
                }
            }
        }

        auto& gimgui = IMGUIWindow::getInstance();
        gimgui.setWindow(window);
        IMGUIWindow::setWindowProc(window);

        m_dx9hook.setWindowAndModule(window, module);
        m_dx9hook.start();

        m_luahook.setProcessAddress(m_process_address);
        m_luahook.setHooks();

        return true;
    }

    bool Hook::addHook(const Address vtable, const Address replacement, const Function original)
    {
        std::fstream out("C:\\Users\\Chris\\Kruschd\\swbf-tools\\test.txt", std::ios::app);
        auto res = MH_CreateHook(vtable, replacement, original);

        if(res == MH_OK)
        {
            out << "ok\n";
        }
        else
        {
            out << "not ok: " << res << " " << (unsigned)vtable << "\n";
        }
        out.close();

        return res == MH_OK;
    }

    bool Hook::enableHook(const Address vtable)
    {
        return MH_EnableHook(vtable) == MH_OK;
    }

    bool Hook::enableAllHooks()
    {
        return MH_EnableHook(MH_ALL_HOOKS) == MH_OK;
    }

    bool Hook::addAndInitHook(const Address vtable, const Address replacement, const Function original)
    {
        return addHook(vtable, replacement, original) && enableHook(vtable);
    }

    bool Hook::removeHook(const Address vtable)
    {
        return MH_DisableHook(vtable) == MH_OK;
    }

    bool Hook::removeAllHooks()
    {
        return MH_DisableHook(MH_ALL_HOOKS) == MH_OK;
    }
}  // namespace hook
