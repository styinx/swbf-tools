#include "Launcher.hpp"

#include "DLLInject.hpp"
#include "SplashScreen.hpp"

#include <cstring>

namespace swbftools
{
    Launcher::Launcher(HINSTANCE instance)
        : m_swbf_path("battlefront.exe")
        , m_dll_path("swbf-tools.dll")
    {
        freopen("SWBF-Launcher.txt", "w", stdout);
        printf("Start Launcher\n");

        m_splash_thread = std::thread(&Launcher::startSplashScreen, this, instance);

        // Wait until the splash screen is ready
        while(m_splash_screen == nullptr || !m_splash_ready)
        {
            Sleep(100);
        }
    }

    Launcher::~Launcher()
    {
        printf("Stop Launcher\n");

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

        printf("- Set CMD Args\n");
        for(int i = 1; i < argc; ++i)
        {
            auto v = argv[i];

            if(strcmp(argv[i], "--dir") == 0)
            {
                ++i;
                strcpy(m_working_dir, argv[i]);

                printf("  - Path: '%s'\n", argv[i]);
            }

            else if(strcmp(argv[i], "--dll") == 0)
            {
                ++i;
                m_dll_path = argv[i];

                printf("  - DLL: '%s'\n", argv[i]);
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

                printf("  - Executable: '%s'\n", argv[i]);
            }

            else
            {
                printf("  - '%s'\n", argv[i]);

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

        printf("- Start SWBF\n");
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
        printf("- Inject DLL\n");
        DLLInject injector{"battlefront.exe", std::move(m_dll_path), 1000, 5000};
        injector.run();
    }
}  // namespace swbftools
