#include "IMGUIWindow.hpp"

#include "imgui.h"

namespace hook
{
    WNDPROC IMGUIWindow::s_windowProcessHandler = nullptr;

    void IMGUIWindow::setWindowProc(HWND window)
    {
        IMGUIWindow::s_windowProcessHandler =
            (WNDPROC)SetWindowLongPtr(window, GWLP_WNDPROC, (LRESULT)hookWindowProcessHandler);
    }

    LRESULT CALLBACK IMGUIWindow::hookWindowProcessHandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
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

    IMGUIWindow::IMGUIWindow()
    {
    }

    IMGUIWindow& IMGUIWindow::getInstance()
    {
        static IMGUIWindow s_window;
        return s_window;
    }

    void IMGUIWindow::setWindow(HWND window)
    {
        m_window = window;
    }

    void IMGUIWindow::setHandle(HANDLE handle)
    {
        m_process_handle = handle;
    }

    void IMGUIWindow::setAddress(std::uint32_t address)
    {
        m_process_address = address;
    }

    void IMGUIWindow::init(IDirect3DDevice9* device)
    {
        if(!m_ready && m_window)
        {
            ImGui::CreateContext();
            ImGui_ImplWin32_Init(m_window);
            ImGui_ImplDX9_Init(device);
            m_ready = true;

            setStyle();
            ImGuiIO& io = ImGui::GetIO();
            io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;
            io.BackendFlags |= ImGuiBackendFlags_RendererHasVtxOffset;
            io.BackendFlags |= ImGuiBackendFlags_HasSetMousePos;
            io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
            io.ConfigFlags |= ImGuiConfigFlags_NavEnableSetMousePos;

            ImFont* font =
                io.Fonts->AddFontFromFileTTF("SWBF1.ttf", 18.0f, NULL, io.Fonts->GetGlyphRangesJapanese());
        }
    }

    void IMGUIWindow::draw()
    {
        if(m_ready)
        {
            ImGuiIO& io = ImGui::GetIO();
            if(GetKeyState(VK_INSERT) == 1)
            {
                m_shown                = true;
                io.MouseDrawCursor     = true;
                io.WantCaptureKeyboard = true;
                io.WantCaptureMouse    = true;
            }
            else
            {
                m_shown                = false;
                io.MouseDrawCursor     = false;
                io.WantCaptureKeyboard = false;
                io.WantCaptureMouse    = false;
            }

            if(m_shown)
            {
                ImGui_ImplDX9_NewFrame();
                ImGui_ImplWin32_NewFrame();
                ImGui::NewFrame();

                ImGui::Begin("SWBF Tools");

                for(const auto& menu : m_menus)
                {
                    ImGui::SameLine();
                    ImGui::Button(menu.first);
                }

                ImGui::NewLine();

                std::string dat;
                ImGui::Text("Player Name: ");
                ImGui::SameLine();
                ImGui::InputText("##", dat.data(), 1);

                ImGui::NewLine();

                ImGui::Text("Players:");
                ImGui::Text("  Obi Wan");
                ImGui::Text("  Ki Adi Mundi");
                ImGui::Text("  Kit Fisto");
                ImGui::Text("  Plo Koon");
                ImGui::Text("  Shaak Ti");

                ImGui::End();

                for(const auto& menu : m_menus)
                {
                    menu.second();
                }

                ImGui::EndFrame();
                ImGui::Render();

                ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
            }
        }
    }

    void IMGUIWindow::addMenu(const char* name, const RenderFunction& menu)
    {
        m_menus[name] = menu;
    }

    void IMGUIWindow::removeMenu(const char* name)
    {
        m_menus.erase(name);
    }

    void IMGUIWindow::setStyle()
    {
        // clang-format off

        static ImVec3 text  = ImVec3(255.0f / 255.0f, 255.0f / 255.0f, 255.0f / 255.0f);
        static ImVec3 head  = ImVec3(255.0f / 255.0f, 110.0f / 255.0f,  50.0f / 255.0f);
        static ImVec3 area  = ImVec3(  0.0f / 255.0f,  50.0f / 255.0f, 130.0f / 255.0f);
        static ImVec3 body  = ImVec3(  0.0f / 255.0f,  30.0f / 255.0f,  60.0f / 255.0f);
        static ImVec3 popup = ImVec3( 30.0f / 255.0f,  40.0f / 255.0f,  80.0f / 255.0f);

        ImGuiStyle& style = ImGui::GetStyle();

        style.Colors[ImGuiCol_Text]                 = ImVec4(text.x, text.y, text.z, 1.00f);
        style.Colors[ImGuiCol_TextDisabled]         = ImVec4(text.x, text.y, text.z, 0.58f);
        style.Colors[ImGuiCol_WindowBg]             = ImVec4(body.x, body.y, body.z, 0.95f);
        // style.Colors[ImGuiCol_ChildWindowBg] = ImVec4( area.x, area.y, area.z, 0.58f );
        style.Colors[ImGuiCol_Border]               = ImVec4(body.x, body.y, body.z, 0.00f);
        style.Colors[ImGuiCol_BorderShadow]         = ImVec4(body.x, body.y, body.z, 0.00f);
        style.Colors[ImGuiCol_FrameBg]              = ImVec4(area.x, area.y, area.z, 1.00f);
        style.Colors[ImGuiCol_FrameBgHovered]       = ImVec4(head.x, head.y, head.z, 0.78f);
        style.Colors[ImGuiCol_FrameBgActive]        = ImVec4(head.x, head.y, head.z, 1.00f);
        style.Colors[ImGuiCol_TitleBg]              = ImVec4(area.x, area.y, area.z, 1.00f);
        style.Colors[ImGuiCol_TitleBgCollapsed]     = ImVec4(area.x, area.y, area.z, 0.75f);
        style.Colors[ImGuiCol_TitleBgActive]        = ImVec4(head.x, head.y, head.z, 1.00f);
        style.Colors[ImGuiCol_MenuBarBg]            = ImVec4(area.x, area.y, area.z, 0.47f);
        style.Colors[ImGuiCol_ScrollbarBg]          = ImVec4(area.x, area.y, area.z, 1.00f);
        style.Colors[ImGuiCol_ScrollbarGrab]        = ImVec4(head.x, head.y, head.z, 0.21f);
        style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(head.x, head.y, head.z, 0.78f);
        style.Colors[ImGuiCol_ScrollbarGrabActive]  = ImVec4(head.x, head.y, head.z, 1.00f);
        // style.Colors[ImGuiCol_ComboBg] = ImVec4( area.x, area.y, area.z, 1.00f );
        style.Colors[ImGuiCol_CheckMark]            = ImVec4(head.x, head.y, head.z, 0.80f);
        style.Colors[ImGuiCol_SliderGrab]           = ImVec4(head.x, head.y, head.z, 0.50f);
        style.Colors[ImGuiCol_SliderGrabActive]     = ImVec4(head.x, head.y, head.z, 1.00f);
        style.Colors[ImGuiCol_Button]               = ImVec4(head.x, head.y, head.z, 0.50f);
        style.Colors[ImGuiCol_ButtonHovered]        = ImVec4(head.x, head.y, head.z, 0.86f);
        style.Colors[ImGuiCol_ButtonActive]         = ImVec4(head.x, head.y, head.z, 1.00f);
        style.Colors[ImGuiCol_Header]               = ImVec4(head.x, head.y, head.z, 0.76f);
        style.Colors[ImGuiCol_HeaderHovered]        = ImVec4(head.x, head.y, head.z, 0.86f);
        style.Colors[ImGuiCol_HeaderActive]         = ImVec4(head.x, head.y, head.z, 1.00f);
        // style.Colors[ImGuiCol_Column] = ImVec4( head.x, head.y, head.z, 0.32f );
        // style.Colors[ImGuiCol_ColumnHovered] = ImVec4( head.x, head.y, head.z, 0.78f );
        // style.Colors[ImGuiCol_ColumnActive] = ImVec4( head.x, head.y, head.z, 1.00f );
        style.Colors[ImGuiCol_ResizeGrip]           = ImVec4(head.x, head.y, head.z, 0.15f);
        style.Colors[ImGuiCol_ResizeGripHovered]    = ImVec4(head.x, head.y, head.z, 0.78f);
        style.Colors[ImGuiCol_ResizeGripActive]     = ImVec4(head.x, head.y, head.z, 1.00f);
        // style.Colors[ImGuiCol_CloseButton] = ImVec4( text.x, text.y, text.z, 0.16f );
        // style.Colors[ImGuiCol_CloseButtonHovered] = ImVec4( text.x, text.y, text.z, 0.39f );
        // style.Colors[ImGuiCol_CloseButtonActive] = ImVec4( text.x, text.y, text.z, 1.00f );
        style.Colors[ImGuiCol_PlotLines]            = ImVec4(text.x, text.y, text.z, 0.63f);
        style.Colors[ImGuiCol_PlotLinesHovered]     = ImVec4(head.x, head.y, head.z, 1.00f);
        style.Colors[ImGuiCol_PlotHistogram]        = ImVec4(text.x, text.y, text.z, 0.63f);
        style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(head.x, head.y, head.z, 1.00f);
        style.Colors[ImGuiCol_TextSelectedBg]       = ImVec4(head.x, head.y, head.z, 0.43f);
        style.Colors[ImGuiCol_PopupBg]              = ImVec4(popup.x, popup.y, popup.z, 0.92f);
        // style.Colors[ImGuiCol_ModalWindowDarkening] = ImVec4( area.x, area.y, area.z, 0.73f );

        // clang-format on
    }
}  // namespace hook
