#ifndef SWBF_TOOLS_LAUNCHER_HPP
#define SWBF_TOOLS_LAUNCHER_HPP

#include "SplashScreen.hpp"

#include <memory>
#include <thread>

namespace swbftools
{

    class Launcher final
    {
    private:
        std::shared_ptr<SplashScreen> m_splash_screen;
        bool                          m_splash_ready;
        std::thread                   m_splash_thread;

        STARTUPINFO         si;
        PROCESS_INFORMATION pi;

        void startSplashScreen(HINSTANCE instance);

    public:
        Launcher(HINSTANCE instance);
        ~Launcher();

        void showSplashScreen();
        void hideSplashScreen();
        void startSWBF(int argc, char** argv);
        void injectDLL();
    };

}  // namespace swbftools

#endif  // SWBF_TOOLS_LAUNCHER_HPP
