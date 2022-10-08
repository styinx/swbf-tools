#include "imgui.h"

#include <string>

namespace hook
{
    void Widget1()
    {
        ImGui::Begin("Widget 1");
        ImGui::Button("Test");

        ImGui::Text("Test");
        ImGui::SameLine();
        std::string dat;
        ImGui::InputText("##", dat.data(), 1);

        ImGui::End();
    }

    void LuaMenu()
    {
        ImGui::Begin("Lua");
        ImGui::BeginChild("Lua");
        for (int n = 0; n < 10; n++)
            ImGui::Text("%04d: Some text", n);
        ImGui::EndChild();
        ImGui::End();
    }
}
