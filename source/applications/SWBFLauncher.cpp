#include "modules/launcher/Launcher.hpp"

#include <iostream>
#include <cstdlib>

int WINAPI WinMain(HINSTANCE instance, HINSTANCE prev_instance, LPSTR cmd_line, int shown)
{
    swbftools::Launcher launcher{instance};
    launcher.showSplashScreen();
    Sleep(1000);
    launcher.startSWBF(__argc, __argv);
    launcher.hideSplashScreen();
    launcher.injectDLL();

    return 0;
}
