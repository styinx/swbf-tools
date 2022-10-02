#include "IMGUIWindow.hpp"

#include "imgui.h"

namespace hook
{
    WNDPROC IMGUIWindow::s_windowProcessHandler = nullptr;

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

    void IMGUIWindow::init(IDirect3DDevice9* device)
    {
        if(!m_ready && m_window)
        {
            ImGui::CreateContext();
            ImGui_ImplWin32_Init(m_window);
            ImGui_ImplDX9_Init(device);
            setStyle();
            m_ready = true;
        }
    }

    void IMGUIWindow::draw()
    {
        if(m_ready)
        {
            ImGui_ImplDX9_NewFrame();
            ImGui_ImplWin32_NewFrame();
            ImGui::NewFrame();

            ImGui::Begin("SWBF-Tools");

            ImGui::SameLine();
            for(const auto& menu : m_menus)
            {
                ImGui::Button(menu.first);
            }

            for(const auto& menu : m_menus)
            {
                menu.second();
            }

            ImGui::End();

            ImGui::EndFrame();
            ImGui::Render();

            ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
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
        static ImVec3 text_col       = ImVec3(255.f / 255.f, 255.f / 255.f, 255.f / 255.f);
        static ImVec3 head_col       = ImVec3(41.f / 255.f, 128.f / 255.f, 185.f / 255.f);
        static ImVec3 area_col       = ImVec3(0.f / 255.f, 60.f / 255.f, 136.f / 255.f);
        static ImVec3 body_col       = ImVec3(44.f / 255.f, 62.f / 255.f, 80.f / 255.f);
        static ImVec3 popup_col      = ImVec3(33.f / 255.f, 46.f / 255.f, 60.f / 255.f);

        ImGuiStyle& style = ImGui::GetStyle();

        // clang-format off
        style.Colors[ImGuiCol_Text] = ImVec4( text_col.x, text_col.y, text_col.z, 1.00f );
        style.Colors[ImGuiCol_TextDisabled] = ImVec4( text_col.x, text_col.y, text_col.z, 0.58f );
        style.Colors[ImGuiCol_WindowBg] = ImVec4( body_col.x, body_col.y, body_col.z, 0.95f );
        //style.Colors[ImGuiCol_ChildWindowBg] = ImVec4( color_for_area.x, color_for_area.y, color_for_area.z, 0.58f );
        style.Colors[ImGuiCol_Border] = ImVec4( body_col.x, body_col.y, body_col.z, 0.00f );
        style.Colors[ImGuiCol_BorderShadow] = ImVec4( body_col.x, body_col.y, body_col.z, 0.00f );
        style.Colors[ImGuiCol_FrameBg] = ImVec4( area_col.x, area_col.y, area_col.z, 1.00f );
        style.Colors[ImGuiCol_FrameBgHovered] = ImVec4( head_col.x, head_col.y, head_col.z, 0.78f );
        style.Colors[ImGuiCol_FrameBgActive] = ImVec4( head_col.x, head_col.y, head_col.z, 1.00f );
        style.Colors[ImGuiCol_TitleBg] = ImVec4( area_col.x, area_col.y, area_col.z, 1.00f );
        style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4( area_col.x, area_col.y, area_col.z, 0.75f );
        style.Colors[ImGuiCol_TitleBgActive] = ImVec4( head_col.x, head_col.y, head_col.z, 1.00f );
        style.Colors[ImGuiCol_MenuBarBg] = ImVec4( area_col.x, area_col.y, area_col.z, 0.47f );
        style.Colors[ImGuiCol_ScrollbarBg] = ImVec4( area_col.x, area_col.y, area_col.z, 1.00f );
        style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4( head_col.x, head_col.y, head_col.z, 0.21f );
        style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4( head_col.x, head_col.y, head_col.z, 0.78f );
        style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4( head_col.x, head_col.y, head_col.z, 1.00f );
        //style.Colors[ImGuiCol_ComboBg] = ImVec4( color_for_area.x, color_for_area.y, color_for_area.z, 1.00f );
        style.Colors[ImGuiCol_CheckMark] = ImVec4( head_col.x, head_col.y, head_col.z, 0.80f );
        style.Colors[ImGuiCol_SliderGrab] = ImVec4( head_col.x, head_col.y, head_col.z, 0.50f );
        style.Colors[ImGuiCol_SliderGrabActive] = ImVec4( head_col.x, head_col.y, head_col.z, 1.00f );
        style.Colors[ImGuiCol_Button] = ImVec4( head_col.x, head_col.y, head_col.z, 0.50f );
        style.Colors[ImGuiCol_ButtonHovered] = ImVec4( head_col.x, head_col.y, head_col.z, 0.86f );
        style.Colors[ImGuiCol_ButtonActive] = ImVec4( head_col.x, head_col.y, head_col.z, 1.00f );
        style.Colors[ImGuiCol_Header] = ImVec4( head_col.x, head_col.y, head_col.z, 0.76f );
        style.Colors[ImGuiCol_HeaderHovered] = ImVec4( head_col.x, head_col.y, head_col.z, 0.86f );
        style.Colors[ImGuiCol_HeaderActive] = ImVec4( head_col.x, head_col.y, head_col.z, 1.00f );
        //style.Colors[ImGuiCol_Column] = ImVec4( color_for_head.x, color_for_head.y, color_for_head.z, 0.32f );
        //style.Colors[ImGuiCol_ColumnHovered] = ImVec4( color_for_head.x, color_for_head.y, color_for_head.z, 0.78f );
        //style.Colors[ImGuiCol_ColumnActive] = ImVec4( color_for_head.x, color_for_head.y, color_for_head.z, 1.00f );
        style.Colors[ImGuiCol_ResizeGrip] = ImVec4( head_col.x, head_col.y, head_col.z, 0.15f );
        style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4( head_col.x, head_col.y, head_col.z, 0.78f );
        style.Colors[ImGuiCol_ResizeGripActive] = ImVec4( head_col.x, head_col.y, head_col.z, 1.00f );
        //style.Colors[ImGuiCol_CloseButton] = ImVec4( color_for_text.x, color_for_text.y, color_for_text.z, 0.16f );
        //style.Colors[ImGuiCol_CloseButtonHovered] = ImVec4( color_for_text.x, color_for_text.y, color_for_text.z, 0.39f );
        //style.Colors[ImGuiCol_CloseButtonActive] = ImVec4( color_for_text.x, color_for_text.y, color_for_text.z, 1.00f );
        style.Colors[ImGuiCol_PlotLines] = ImVec4( text_col.x, text_col.y, text_col.z, 0.63f );
        style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4( head_col.x, head_col.y, head_col.z, 1.00f );
        style.Colors[ImGuiCol_PlotHistogram] = ImVec4( text_col.x, text_col.y, text_col.z, 0.63f );
        style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4( head_col.x, head_col.y, head_col.z, 1.00f );
        style.Colors[ImGuiCol_TextSelectedBg] = ImVec4( head_col.x, head_col.y, head_col.z, 0.43f );
        style.Colors[ImGuiCol_PopupBg] = ImVec4( popup_col.x, popup_col.y, popup_col.z, 0.92f );
        //style.Colors[ImGuiCol_ModalWindowDarkening] = ImVec4( color_for_area.x, color_for_area.y, color_for_area.z, 0.73f );
        // clang-format on
    }
}
