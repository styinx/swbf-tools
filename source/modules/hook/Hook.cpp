#include "Hook.hpp"

#include "modules/hook/IMGUIWindow.hpp"

#include "detours.h"
#include "spdlog/sinks/basic_file_sink.h"
#include "spdlog/spdlog.h"

#include <Psapi.h>
#include <TlHelp32.h>
#include <chrono>

#include <fstream>
#include <thread>

namespace hook
{
    lua_State*      g_L              = nullptr;
    func_luaD_call* g_orig_luaD_call = nullptr;
    func_luaD_call* g_hook_luaD_call = nullptr;
    func_endScene*  g_orig_endScene  = nullptr;
    func_endScene*  g_hook_endScene  = nullptr;

    void call_hook_func(lua_State* L, lua_Debug* ar)
    {
        auto logger = spdlog::get(__FILE__);
        logger->debug("{:>10}: {:>10} ({:>10}) {:>10}", ar->event, ar->currentline, ar->linedefined, ar->nups);
    }

    void hook_luaD_call(lua_State* L, StkId func, int nResults)
    {
        auto logger = spdlog::get(__FILE__);
        logger->debug("Call hook_luaD_call");
        g_L = L;

        // logger->debug("set hook");
        // lua_setcallhook(L, call_hook_func);

        // Detach the hook once we get the lua state.
        DetourTransactionBegin();
        DetourUpdateThread(GetCurrentThread());
        DetourDetach(&g_orig_luaD_call, g_hook_luaD_call);
        DetourTransactionCommit();

        if(g_orig_luaD_call)
            g_orig_luaD_call(L, func, nResults);
    }

    HRESULT __stdcall hook_endScene(IDirect3DDevice9* device)
    {
        spdlog::get(__FILE__)->debug("Draw");

        IMGUIWindow::getInstance().init(device);
        IMGUIWindow::getInstance().draw();

        if(g_orig_endScene)
            return g_orig_endScene(device);
        return TRUE;
    }

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
        , m_app_title(app_title)
        , m_executable(executable)
    {
        auto logger = spdlog::basic_logger_mt(__FILE__, "Hook.log", true);
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
        if(!m_module)
        {
            return false;
        }

        m_window = reinterpret_cast<HWND>(FindWindow(nullptr, m_app_title));
        if(!m_window)
        {
            return false;
        }

        if(findProcessID())
        {
            if(findProcessAddress())
            {
                auto& imgui = IMGUIWindow::getInstance();
                imgui.setWindow(m_window);
                IMGUIWindow::setWindowProc(m_window);

                bool success = true;
                success &= hookLua();
                success &= hookDX9();

                spdlog::get(__FILE__)->debug("Adding hooks was {}", success ? "successful" : "not sucessful");
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
        spdlog::get(__FILE__)->debug("Found PID: {}", m_process_id);

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

        spdlog::get(__FILE__)->debug("Found base address: 0x{:x}", m_process_address);

        return m_process_address != 0;
    }

    bool Hook::hookLua()
    {
        auto logger = spdlog::get(__FILE__);
        logger->debug("Set Hooks");

        bool success = false;

        if(DetourTransactionBegin() == NO_ERROR)
        {
            DWORD luaD_call_address = m_process_address + LUAD_CALL_ADDRESS;
            g_orig_luaD_call  = reinterpret_cast<func_luaD_call*>(luaD_call_address);
            g_hook_luaD_call  = hook_luaD_call;

            logger->debug("Attempt hooking luaD call at address 0x{:x}", luaD_call_address);
            if(DetourAttach(&reinterpret_cast<PVOID&>(g_orig_luaD_call), &g_hook_luaD_call) == NO_ERROR)
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

    bool Hook::hookDX9()
    {
        auto logger = spdlog::get(__FILE__);
        logger->debug("Set Hooks");

        auto windowed = ((GetWindowLongPtr(m_window, GWL_STYLE) & WS_POPUP) != 0) ? FALSE : TRUE;

        D3DPRESENT_PARAMETERS d3dpp         = D3DPRESENT_PARAMETERS{};
        IDirect3D9*           d3d           = Direct3DCreate9(D3D_SDK_VERSION);
        IDirect3DDevice9*     device        = nullptr;
        DWORD*                device_vtable = nullptr;

        ZeroMemory(&d3dpp, sizeof(d3dpp));
        d3dpp.BackBufferCount            = 1;
        d3dpp.MultiSampleType            = D3DMULTISAMPLE_NONE;
        d3dpp.SwapEffect                 = D3DSWAPEFFECT_DISCARD;
        d3dpp.hDeviceWindow              = m_window;
        d3dpp.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;
        d3dpp.PresentationInterval       = D3DPRESENT_INTERVAL_IMMEDIATE;
        d3dpp.BackBufferFormat           = D3DFMT_UNKNOWN;
        d3dpp.Windowed                   = windowed;

        d3d->CreateDevice(
            D3DADAPTER_DEFAULT,
            D3DDEVTYPE_HAL,
            m_window,
            D3DCREATE_HARDWARE_VERTEXPROCESSING,
            &d3dpp,
            &device);

        device_vtable = reinterpret_cast<DWORD*>(device);
        device_vtable = reinterpret_cast<DWORD*>(device_vtable[0]);

        bool success = false;

        if(DetourTransactionBegin() == NO_ERROR)
        {
            DWORD end_scene_address = (DWORD)(device_vtable[END_SCENE_VTABLE_POS]);
            g_orig_endScene   = reinterpret_cast<func_endScene*>(end_scene_address);
            g_hook_endScene   = hook_endScene;

            logger->debug("Attempt hooking endScene at address 0x{:x}", end_scene_address);
            if(DetourAttach(&reinterpret_cast<PVOID&>(g_orig_endScene), &g_hook_endScene) == NO_ERROR)
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
