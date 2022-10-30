#include "Hook.hpp"

#include "modules/hook/IMGUIWindow.hpp"

#include "spdlog/sinks/basic_file_sink.h"
#include "spdlog/spdlog.h"

#include <Psapi.h>
#include <TlHelp32.h>
#include <chrono>
#include <fstream>
#include <thread>

namespace hook
{
    Hook::Hook(
        HMODULE             module,
        const char*         app_title,
        const char*         executable,
        const std::uint32_t interval,
        const std::uint32_t timeout)
        : m_interval(interval)
        , m_timeout(timeout)
        , m_process_id(0)
        , m_process_address(0)
        , m_running(true)
        , m_process_handle(nullptr)
        , m_module(module)
        , m_dx9hook()
        , m_luahook()
        , m_app_title(app_title)
        , m_executable(executable)
    {
        auto logger = spdlog::basic_logger_mt("log_hook", "Hook.log", true);
        spdlog::set_level(spdlog::level::debug);
        logger->debug("Start hook");

        startThread();
    }

    Hook::~Hook()
    {

    }

    // Private

    DWORD WINAPI Hook::runThread(LPVOID hook)
    {
        if(hook)
        {
            auto* hook_instance = reinterpret_cast<Hook*>(hook);
            return hook_instance->run();
        }
        return FALSE;
    }

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

        if(findProcessID())
        {
            if(findProcessAddress())
            {
                auto& imgui = IMGUIWindow::getInstance();
                imgui.setWindow(window);
                imgui.setAddress(m_process_address);
                imgui.setHandle(m_process_handle);
                IMGUIWindow::setWindowProc(window);

                m_luahook.setProcessAddress(m_process_address);
                m_luahook.setHooks();

                m_dx9hook.setProcessAddress(m_process_address);
                m_dx9hook.setWindowAndModule(window, module);
                m_dx9hook.start();
            }
        }

        while(m_running)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }

        return true;
    }

    bool Hook::findProcessID()
    {
        std::uint32_t timer = 0;
        while(m_process_id == 0)
        {
            if(m_timeout > 0 && timer > m_timeout)
                break;

            HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
            if(snapshot == INVALID_HANDLE_VALUE)
                break;

            PROCESSENTRY32 process{};
            process.dwSize = sizeof(PROCESSENTRY32);

            if(Process32First(snapshot, &process))
            {
                do
                {
                    std::string proc_name = process.szExeFile;
                    if(proc_name == m_executable)
                    {
                        m_process_id = process.th32ProcessID;
                        break;
                    }
                } while(Process32Next(snapshot, &process));
            }

            CloseHandle(snapshot);
            Sleep(m_interval);
        }
        spdlog::get("log_hook")->debug("Found pid: {}", m_process_id);

        return m_process_id != 0;
    }

    bool Hook::findProcessAddress()
    {
        m_process_handle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, m_process_id);

        const UINT MAX_MODULES = 256;
        HMODULE    modules[256];
        TCHAR      module_name[MAX_PATH];
        DWORD      _module;

        EnumProcessModules(m_process_handle, modules, sizeof(HMODULE) * MAX_MODULES, &_module);

        if(EnumProcessModules(m_process_handle, modules, _module / sizeof(HMODULE), &_module))
        {
            for(unsigned i = 0; i < _module / sizeof(HMODULE); i++)
            {
                if(GetModuleBaseName(m_process_handle, modules[i], module_name, MAX_MODULES))
                {
                    std::string name{module_name};
                    if(name == m_executable)
                    {
                        m_process_address = (DWORD)(modules[i]);
                        break;
                    }
                }
            }
        }

        spdlog::get("log_hook")->debug("A Found address: {:x}", m_process_address);

        return m_process_address != 0;
    }
}  // namespace hook
