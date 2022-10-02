#include "Launcher.hpp"

#include "DLLInject.hpp"
#include "SplashScreen.hpp"

namespace swbftools
{
    Launcher::Launcher(HINSTANCE instance)
    {
        m_splash_thread = std::thread(&Launcher::startSplashScreen, this, instance);

        // Wait until the splash screen is ready
        while(m_splash_screen == nullptr || !m_splash_ready) {
            Sleep(100);
        }
    }

    Launcher::~Launcher()
    {
        if(m_splash_thread.joinable())
        {
            m_splash_screen->close();
            m_splash_thread.join();
        }
    }

    void Launcher::startSplashScreen(HINSTANCE instance)
    {
        m_splash_screen = std::make_shared<SplashScreen>(instance);
        m_splash_ready = true;
        m_splash_screen->runMessageLoop();
    }

    void Launcher::showSplashScreen()
    {
        m_splash_screen->show(SW_SHOW);
    }

    void Launcher::hideSplashScreen()
    {
        m_splash_screen->show(SW_HIDE);
    }

    void Launcher::startSWBF(int argc, char** argv)
    {
        for(int i = 0; i < argc; ++i) {
            const char* arg = argv[i];
        }

        LPSTR executable = "C:\\Users\\Chris\\Spiele\\SWBF\\GameData\\battlefront.exe\0";
        LPSTR dir        = "C:\\Users\\Chris\\Spiele\\SWBF\\GameData\0";
        LPSTR args       = "/win /resolution 800 800 /nointro /nostartupmusic\0";
        LPSTR exe_args   = "C:\\Users\\Chris\\Spiele\\SWBF\\GameData\\battlefront.exe /win "
                           "/resolution 800 800 /nointro /nostartupmusic\0";

        // set the size of the structures
        ZeroMemory(&si, sizeof(si));
        ZeroMemory(&pi, sizeof(pi));

        // start the program up
        const auto res = CreateProcess(NULL, exe_args, NULL, NULL, TRUE, 0, NULL, dir, &si, &pi);

        // Close process and thread handles.
        // CloseHandle(pi.hProcess);
        // CloseHandle(pi.hThread);
    }

    void Launcher::injectDLL()
    {
        DLLInject injector{"battlefront.exe", "swbf-tools.dll", 1000, 5000};
        injector.run();
    }
}  // namespace swbftools
