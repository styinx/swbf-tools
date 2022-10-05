#ifndef SWBF_TOOLS_IMGUIWINDOW_HPP
#define SWBF_TOOLS_IMGUIWINDOW_HPP

#include "backends/imgui_impl_dx9.h"
#include "backends/imgui_impl_win32.h"
#include "imgui.h"

#include <Windows.h>
#include <fstream>
#include <functional>
#include <map>

extern IMGUI_IMPL_API LRESULT
ImGui_ImplWin32_WndProcHandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

namespace hook
{

    using RenderFunction = std::function<void(void)>;
    struct ImVec3
    {
        float x, y, z;
        ImVec3(float _x = 0.0f, float _y = 0.0f, float _z = 0.0f)
        {
            x = _x;
            y = _y;
            z = _z;
        }
    };

    class IMGUIWindow final
    {
    private:
        IMGUIWindow();

        bool m_ready = false;
        bool m_shown = false;

        HWND   m_window          = nullptr;
        HANDLE m_process_handle  = nullptr;
        std::uint32_t m_process_address = 0;

        static WNDPROC s_windowProcessHandler;

        std::map<const char*, RenderFunction> m_menus;

    public:
        static IMGUIWindow& getInstance();
        void                setWindow(HWND window);
        void                setHandle(HANDLE handle);
        void                setAddress(std::uint32_t address);
        void                init(IDirect3DDevice9* device);
        void                draw();
        void                addMenu(const char* name, const RenderFunction& menu);
        void                removeMenu(const char* name);
        void                setStyle();

        static void setWindowProc(HWND window)
        {
            IMGUIWindow::s_windowProcessHandler =
                (WNDPROC)SetWindowLongPtr(window, GWLP_WNDPROC, (LRESULT)hookWindowProcessHandler);
        }

        static LRESULT CALLBACK hookWindowProcessHandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
        {
            if(IMGUIWindow::getInstance().m_ready && IMGUIWindow::getInstance().m_shown)
            {
                ImGuiIO& io = ImGui::GetIO();
                io.AddMouseButtonEvent(1, true);
                ImGui_ImplWin32_WndProcHandler(hwnd, msg, wParam, lParam);

                return TRUE;
            }

            return CallWindowProcA(IMGUIWindow::s_windowProcessHandler, hwnd, msg, wParam, lParam);
        }
    };

}  // namespace hook

#endif  // SWBF_TOOLS_IMGUIWINDOW_HPP
