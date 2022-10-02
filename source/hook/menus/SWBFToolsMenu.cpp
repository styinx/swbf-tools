#include "imgui.h"

namespace hook
{
    void SWBFToolsMenu()
    {
        ImGui::Begin("SWBF Tools");
        ImGui::Button("Test");
        ImGui::End();
    }

    void LuaMenu()
    {
        ImGui::BeginChild("Lua");
        for (int n = 0; n < 50; n++)
            ImGui::Text("%04d: Some text", n);
        ImGui::EndChild();
    }
}
