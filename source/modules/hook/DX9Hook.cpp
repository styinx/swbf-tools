#include "DX9Hook.hpp"

#include "hook/Hook.hpp"
#include "hook/IMGUIWindow.hpp"

namespace hook
{
    HRESULT __stdcall hookEndScene(IDirect3DDevice9* device)
    {
        IMGUIWindow::getInstance().init(device);
        IMGUIWindow::getInstance().draw();

        if(originalEndScene)
            return originalEndScene(device);
        return TRUE;
    }

    DX9Hook::DX9Hook(Hook* hook)
        : m_hook(hook)
    {
    }

    DX9Hook::~DX9Hook()
    {
    }

    void DX9Hook::start()
    {
        addDX9();
        addHooks();
    }

    void DX9Hook::stop()
    {
        removeHooks();

        m_device->Release();
        FreeLibraryAndExitThread(m_target_module, 0);
    }

    void DX9Hook::setWindowAndModule(HWND const& window, HMODULE const& module)
    {
        m_target_window = window;
        m_target_module = module;
    }

    IDirect3DDevice9* DX9Hook::getDevice()
    {
        return m_device;
    }

    // Private

    bool DX9Hook::addDX9()
    {
        auto windowed = ((GetWindowLongPtr(m_target_window, GWL_STYLE) & WS_POPUP) != 0) ? FALSE : TRUE;

        D3DPRESENT_PARAMETERS d3dpp = D3DPRESENT_PARAMETERS{};
        IDirect3D9*           d3d   = Direct3DCreate9(D3D_SDK_VERSION);

        ZeroMemory(&d3dpp, sizeof(d3dpp));
        d3dpp.BackBufferCount            = 1;
        d3dpp.MultiSampleType            = D3DMULTISAMPLE_NONE;
        d3dpp.SwapEffect                 = D3DSWAPEFFECT_DISCARD;
        d3dpp.hDeviceWindow              = m_target_window;
        d3dpp.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;
        d3dpp.PresentationInterval       = D3DPRESENT_INTERVAL_IMMEDIATE;
        d3dpp.BackBufferFormat           = D3DFMT_UNKNOWN;
        d3dpp.Windowed                   = windowed;

        d3d->CreateDevice(
            D3DADAPTER_DEFAULT,
            D3DDEVTYPE_HAL,
            m_target_window,
            D3DCREATE_HARDWARE_VERTEXPROCESSING,
            &d3dpp,
            &m_device);

        m_device_vtable = reinterpret_cast<Address>(m_device);
        m_device_vtable = reinterpret_cast<Address>(m_device_vtable[0]);
        return true;  // todo
    }

    bool DX9Hook::addHooks()
    {
        bool success = true;

        success &= m_hook->addAndInitHook(
            reinterpret_cast<Address>(m_device_vtable[END_SCENE_VTABLE_POS]),
            reinterpret_cast<Address>(hookEndScene),
            reinterpret_cast<Function>(&originalEndScene));

        return success;
    }

    bool DX9Hook::removeHooks()
    {
        bool success = true;

        success &= m_hook->removeHook(reinterpret_cast<Address>(m_device_vtable[END_SCENE_VTABLE_POS]));

        return success;
    }

}  // namespace hook
