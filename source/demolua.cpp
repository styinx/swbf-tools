#include <d3d9.h>
#include <windows.h>

extern "C" {
#include "lvm.h"
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
}

LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

class DX9
{
private:
    IDirect3D9*       m_d3d    = nullptr;
    IDirect3DDevice9* m_device = nullptr;

public:
    bool createDevice(HWND hWnd);
    void cleanupDevice();

    IDirect3DDevice9* getDevice();
};

bool DX9::createDevice(HWND hWnd)
{
    m_d3d = Direct3DCreate9(D3D_SDK_VERSION);

    D3DPRESENT_PARAMETERS d3dpp{};

    ZeroMemory(&d3dpp, sizeof(d3dpp));
    d3dpp.Windowed      = TRUE;
    d3dpp.SwapEffect    = D3DSWAPEFFECT_DISCARD;
    d3dpp.hDeviceWindow = hWnd;

    m_d3d->CreateDevice(
        D3DADAPTER_DEFAULT,
        D3DDEVTYPE_HAL,
        hWnd,
        D3DCREATE_SOFTWARE_VERTEXPROCESSING,
        &d3dpp,
        &m_device);

    return true;
}

void DX9::cleanupDevice()
{
    if(m_device)
    {
        m_device->Release();
        m_device = nullptr;
    }

    if(m_d3d)
    {
        m_d3d->Release();
        m_d3d = nullptr;
    }
}

IDirect3DDevice9* DX9::getDevice()
{
    return m_device;
}

DX9 dx9;

void hkf(lua_State* L, lua_Debug* D)
{
    printf("hookf\n");
}

lua_TObject o {

};

int main(int, char**)
{
    lua_State* L;
    L = lua_open(64);

    dx9 = DX9{};

    WNDCLASSEX wc = {
        sizeof(WNDCLASSEX),
        CS_CLASSDC,
        WndProc,
        0L,
        0L,
        GetModuleHandle(nullptr),
        nullptr,
        nullptr,
        nullptr,
        nullptr,
        TEXT("Demo"),
        nullptr};

    ::RegisterClassEx(&wc);
    HWND hwnd = ::CreateWindow(
        wc.lpszClassName,
        TEXT("Demo"),
        WS_OVERLAPPEDWINDOW,
        100,
        100,
        700,
        400,
        nullptr,
        nullptr,
        wc.hInstance,
        nullptr);

    ::ShowWindow(hwnd, SW_SHOWDEFAULT);
    ::UpdateWindow(hwnd);

    dx9.createDevice(hwnd);

    int i = 0;

    bool done = false;
    while(!done)
    {
        MSG msg;
        while(::PeekMessage(&msg, nullptr, 0U, 0U, PM_REMOVE))
        {
            ::TranslateMessage(&msg);
            ::DispatchMessage(&msg);
            if(msg.message == WM_QUIT)
                done = true;
        }
        if(done)
            break;

        // Clear
        dx9.getDevice()->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0, 40, 100), 1.0f, 0);

        // Draw
        dx9.getDevice()->BeginScene();
        dx9.getDevice()->EndScene();

        // Present
        dx9.getDevice()->Present(nullptr, nullptr, nullptr, nullptr);

        if(i++ > 100)
        {
            lua_setcallhook(L, hkf);
            lua_setlinehook(L, hkf);
            luaD_checkstack(L, 1);
            luaD_lineHook(L, &o, 0, hkf);
            const auto r = &luaD_checkstack;
            //luaD_call(L, nullptr, 1);
            i = 0;
        }
    }

    dx9.cleanupDevice();

    ::DestroyWindow(hwnd);
    ::UnregisterClass(wc.lpszClassName, wc.hInstance);

    return 0;
}

LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch(msg)
    {
    case WM_DESTROY:
        ::PostQuitMessage(0);
        return 0;
    }

    return ::DefWindowProc(hWnd, msg, wParam, lParam);
}