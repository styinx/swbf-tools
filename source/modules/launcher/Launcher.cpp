#include "Launcher.hpp"

#include "SplashScreen.hpp"

#include "DLLInject.hpp"
#include "spdlog/sinks/basic_file_sink.h"
#include "spdlog/spdlog.h"

#include <cstring>

namespace swbftools
{
    Launcher::Launcher(HINSTANCE instance)
        : m_swbf_path("battlefront.exe")
        , m_dll_path("swbf-tools.dll")
    {
        auto logger = spdlog::basic_logger_mt("log_swbf_launcher", "SWBFLauncher.log", true);
        spdlog::set_level(spdlog::level::debug);

        logger->debug("Start Launcher");

        m_splash_thread = std::thread(&Launcher::startSplashScreen, this, instance);

        // Wait until the splash screen is ready
        while(m_splash_screen == nullptr || !m_splash_ready)
        {
            Sleep(100);
        }
    }

    Launcher::~Launcher()
    {
        spdlog::get("log_swbf_launcher")->debug("Stop Launcher");

        if(m_splash_thread.joinable())
        {
            m_splash_screen->close();
            m_splash_thread.join();
        }
    }

    void Launcher::startSplashScreen(HINSTANCE instance)
    {
        m_splash_screen = std::make_shared<SplashScreen>(instance);
        m_splash_ready  = true;
        m_splash_screen->runMessageLoop();
    }

    void Launcher::setCMDArgs(int argc, char** argv)
    {
        CHAR  space = ' ';
        CHAR* index = m_cmd_arguments;

        auto logger = spdlog::get("log_swbf_launcher");
        if(argc > 1)
            logger->info("Parse CMD Arguments");

        for(int i = 1; i < argc; ++i)
        {
            auto v = argv[i];

            if(strcmp(argv[i], "--dir") == 0)
            {
                ++i;
                strcpy(m_working_dir, argv[i]);

                logger->info(" - Path: '{}'", argv[i]);
            }

            else if(strcmp(argv[i], "--dll") == 0)
            {
                ++i;
                m_dll_path = argv[i];

                logger->info(" - DLL: '{}'", argv[i]);
            }

            // Must come before swbf args
            else if(strcmp(argv[i], "--exe") == 0)
            {
                ++i;
                m_swbf_path = argv[i];
                size_t len  = strlen(argv[i]);
                memcpy(index, argv[i], len);
                index += len;
                memcpy(index, &space, 1);
                index += 1;

                logger->info(" - Executable: '{}'", argv[i]);
            }

            else
            {
                logger->info(" - '{}'", argv[i]);

                size_t len = strlen(argv[i]);
                memcpy(index, argv[i], len);
                index += len;
                memcpy(index, &space, 1);
                index += 1;
            }
        }

        m_cmd_arguments[strlen(m_cmd_arguments)] = '\0';
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
        setCMDArgs(argc, argv);

        spdlog::get("log_swbf_launcher")->debug(" - Start SWBF");

        ZeroMemory(&m_startup_info, sizeof(m_startup_info));
        ZeroMemory(&m_process_info, sizeof(m_process_info));

        const auto res = CreateProcess(
            NULL,
            m_cmd_arguments,
            NULL,
            NULL,
            TRUE,
            0,
            NULL,
            m_working_dir,
            &m_startup_info,
            &m_process_info);

        // Close process and thread handles.
        // CloseHandle(m_process_info.hProcess);
        // CloseHandle(m_process_info.hThread);
    }

    void Launcher::injectDLL()
    {
        spdlog::get("log_swbf_launcher")->debug(" - Start Injector");
        DLLInject injector{"battlefront.exe", std::move(m_dll_path), 1000, 5000};
        injector.run();
        spdlog::get("log_swbf_launcher")->debug(" - Finished Injector");
    }
}  // namespace swbftools
