#include "Launcher.hpp"

#include <iostream>

int WINAPI WinMain(HINSTANCE instance, HINSTANCE prev_instance, LPSTR cmd_line, int shown)
{
    swbftools::Launcher launcher{instance};
    launcher.showSplashScreen();
    Sleep(1000);
    //launcher.startSWBF(__argc, __argv);
    launcher.injectDLL();
    launcher.hideSplashScreen();
    Sleep(1000);

    return 0;
}
