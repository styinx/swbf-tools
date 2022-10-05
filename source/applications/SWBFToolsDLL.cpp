#include "Hook.hpp"

#include "hook/IMGUIWindow.hpp"
#include "hook/menus/SWBFToolsMenu.hpp"
#include <fstream>

hook::Hook* g_hook = nullptr;
BOOL APIENTRY DllMain(HMODULE module, DWORD reason, LPVOID reserved)
{
    switch(reason)
    {
    case DLL_PROCESS_ATTACH:
    {
        g_hook = new hook::Hook(module, "Star Wars Battlefront", "battlefront.exe");
        g_hook->startThread();

        hook::IMGUIWindow::getInstance().addMenu("SWBF Tools", hook::SWBFToolsMenu);
        hook::IMGUIWindow::getInstance().addMenu("Test", hook::LuaMenu);

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