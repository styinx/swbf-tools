#include "modules/hook/Hook.hpp"

#include "modules/hook/IMGUIWindow.hpp"
#include "modules/hook/menus/SWBFToolsMenu.hpp"
#include <fstream>

hook::Hook* g_hook = nullptr;

BOOL APIENTRY DllMain(HMODULE module, DWORD reason, LPVOID reserved)
{
    switch(reason)
    {
    case DLL_PROCESS_ATTACH:
    {
        g_hook = new hook::Hook(module, "Star Wars Battlefront", "battlefront.exe");

        hook::IMGUIWindow::getInstance().addMenu("Widget1", hook::Widget1);

        break;
    }
    case DLL_PROCESS_DETACH:
    {
        delete g_hook;
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