#include "imgui.h"

#include <stdio.h>

namespace hook
{
    void SWBFToolsMenu()
    {
        ImGui::Begin("Widget 1");
        ImGui::Button("Test");
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
