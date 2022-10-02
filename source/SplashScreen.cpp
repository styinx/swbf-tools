#include "SplashScreen.hpp"

#include "resources/SplashImage.hpp"

#include <Windows.h>

namespace swbftools
{
    SplashScreen* SplashScreen::s_splash_screen = nullptr;
    HBITMAP       SplashScreen::s_splash_image  = nullptr;

    SplashScreen::SplashScreen(HINSTANCE instance)
    {
        createWindow(instance);
    }

    SplashScreen::~SplashScreen()
    {
        close();
    }

    LRESULT CALLBACK SplashScreen::WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
    {
        switch(msg)
        {
        case WM_CREATE:
            SplashScreen::s_splash_screen = reinterpret_cast<SplashScreen*>(lParam);
            break;
        case WM_PAINT:
            if(SplashScreen::s_splash_screen)
            {
                SplashScreen::s_splash_screen->OnPaint(hwnd);
            }
            break;
        case WM_DESTROY:
            PostQuitMessage(0);
            break;
        default:
            return DefWindowProc(hwnd, msg, wParam, lParam);
        }
        return 0;
    }

    void SplashScreen::OnPaint(HWND hwnd)
    {
        if(!s_splash_image)
            return;

        PAINTSTRUCT ps;
        HDC         device_context = BeginPaint(hwnd, &ps);
        HDC         compatible_dc  = CreateCompatibleDC(device_context);
        HBITMAP     bitmap = reinterpret_cast<HBITMAP>(SelectObject(compatible_dc, s_splash_image));

        BitBlt(device_context, 0, 0, SPLASH_IMAGE_WIDTH, SPLASH_IMAGE_HEIGHT, compatible_dc, 0, 0, SRCCOPY);
        SelectObject(compatible_dc, bitmap);
        DeleteDC(compatible_dc);
        EndPaint(hwnd, &ps);
    }

    void SplashScreen::createWindow(HINSTANCE instance)
    {
        const char class_name[4] = "cls";

        WNDCLASSEX wc;
        wc.cbSize        = sizeof(WNDCLASSEX);
        wc.style         = CS_BYTEALIGNCLIENT | CS_BYTEALIGNWINDOW;
        wc.lpfnWndProc   = SplashScreen::WndProc;
        wc.cbClsExtra    = 0;
        wc.cbWndExtra    = DLGWINDOWEXTRA;
        wc.hInstance     = instance;
        wc.hIcon         = nullptr;
        wc.hCursor       = LoadCursor(nullptr, IDC_WAIT);
        wc.hbrBackground = nullptr;
        wc.lpszMenuName  = nullptr;
        wc.lpszClassName = class_name;
        wc.hIconSm       = nullptr;

        if(!RegisterClassEx(&wc))
        {
            return;
        }

        s_splash_image = CreateBitmap(
            SPLASH_IMAGE_WIDTH,
            SPLASH_IMAGE_HEIGHT,
            1,
            SPLASH_IMAGE_BPP,
            reinterpret_cast<const void*>(SPLASH_IMAGE));

        int image_x = static_cast<int>((GetSystemMetrics(SM_CXFULLSCREEN) - SPLASH_IMAGE_WIDTH) / 2);
        int image_y = static_cast<int>((GetSystemMetrics(SM_CYFULLSCREEN) - SPLASH_IMAGE_HEIGHT) / 2);

        m_window = CreateWindowEx(
            WS_EX_TOPMOST | WS_EX_TOOLWINDOW,
            class_name,
            nullptr,
            WS_POPUP,
            image_x,
            image_y,
            SPLASH_IMAGE_WIDTH,
            SPLASH_IMAGE_HEIGHT,
            nullptr,
            nullptr,
            instance,
            nullptr);

        if(m_window == nullptr)
        {
            return;
        }

        SetWindowLong(m_window, GWL_EXSTYLE, GetWindowLong(m_window, GWL_EXSTYLE) | WS_EX_LAYERED);
        SetLayeredWindowAttributes(m_window, SPLASH_IMAGE_COLOR_KEY, 0, LWA_COLORKEY);
    }

    void SplashScreen::runMessageLoop()
    {
        MSG msg;
        while(GetMessage(&msg, NULL, 0, 0) > 0)
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    void SplashScreen::show(const DWORD show) const
    {
        if(m_window == nullptr)
        {
            return;
        }

        ShowWindow(m_window, show);
        UpdateWindow(m_window);
    }

    void SplashScreen::close() const
    {
        PostMessage(m_window, WM_DESTROY, 0, 0);
    }

}  // namespace swbftools
