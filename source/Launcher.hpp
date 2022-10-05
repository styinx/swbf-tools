#ifndef SWBF_TOOLS_LAUNCHER_HPP
#define SWBF_TOOLS_LAUNCHER_HPP

#include "SplashScreen.hpp"

#include <memory>
#include <string>
#include <thread>

namespace swbftools
{

    class Launcher final
    {
    private:
        std::shared_ptr<SplashScreen> m_splash_screen;
        bool                          m_splash_ready;
        std::thread                   m_splash_thread;
        std::string                   m_swbf_path;
        std::string                   m_dll_path;
        CHAR                          m_cmd_arguments[512] = {0};
        CHAR                          m_working_dir[256] = {0};
        STARTUPINFO                   m_startup_info;
        PROCESS_INFORMATION           m_process_info;

        void startSplashScreen(HINSTANCE instance);
        void setCMDArgs(int argc, char** argv);

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
