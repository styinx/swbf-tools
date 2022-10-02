#include "Hook.hpp"

#include "hook/IMGUIWindow.hpp"
#include "hook/menus/Menus.hpp"

hook::Hook* ghook = nullptr;

BOOL APIENTRY DllMain(HMODULE module, DWORD reason, LPVOID reserved)
{
    switch(reason)
    {
    case DLL_PROCESS_ATTACH:
    {
        ghook = new hook::Hook(module, "Star Wars Battlefront", "battlefront.exe");
        ghook->startThread();

        hook::IMGUIWindow::getInstance().addMenu("SWBF Tools", hook::SWBFToolsMenu);

        break;
    }
    case DLL_PROCESS_DETACH:
    {
        delete ghook;
        break;
    }
    case DLL_THREAD_ATTACH:
        break;
    case DLL_THREAD_DETACH:
        break;
    default:
        break;
    }

    return TRUE;
}