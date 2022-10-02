#ifndef SWBF_TOOLS_SPLASHSCREEN_HPP
#define SWBF_TOOLS_SPLASHSCREEN_HPP

#include <Windows.h>

namespace swbftools
{
    /**
     * @brief   Creates a splash screen that should run in separate message loop in a separate
     *          thread. The message loop handles CREATE, PAINT, and DESTROY events. The image
     *          is created in the constructor. The calling class of the splash screen should
     *          start a new thread running the message loop. The calling class should manually
     *          call the destructor of the splash screen class or call close.
     *
     * @note    The MessageLoop and the constructor must be called from the same thread.
     *          This implementation uses a static image defined in SplashImage.hpp.
     */
    class SplashScreen final
    {
    private:
        static SplashScreen* s_splash_screen;
        static HBITMAP       s_splash_image;
        HWND                 m_window = nullptr;
        bool                 m_shown  = true;
        bool                 m_ready  = true;

        static LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
        void                    OnPaint(HWND hwnd);
        void                    createWindow(HINSTANCE instance);

    public:
        explicit SplashScreen(HINSTANCE instance);
        ~SplashScreen();

        void show(const DWORD show = SW_SHOW) const;
        void close() const;
        void runMessageLoop();
    };

}  // namespace swbftools

#endif  // SWBF_TOOLS_SPLASHSCREEN_HPP
