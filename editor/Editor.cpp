#include "../imgui/imgui.h"

#include "Editor.h"
#include "../print_helper.h"


void Editor::render(){
    render_debug_console();
}

void Editor::render_debug_console(){
    ImGuiIO& io = ImGui::GetIO();
    float window_width = io.DisplaySize.x;
    float window_height = io.DisplaySize.y;

    float console_width = 400;
    float console_height = 200;

    ImGui::SetNextWindowPos(ImVec2(window_width - console_width, window_height - console_height));
    ImGui::SetNextWindowSize(ImVec2(console_width, console_height));
    // ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);


    ImGuiWindowFlags console_flags = ImGuiWindowFlags_NoMove | 
                                     ImGuiWindowFlags_NoResize | 
                                     ImGuiWindowFlags_NoCollapse;

    ImGui::Begin("Debug Console", nullptr, console_flags);
    for(const auto& message : get_console_messages()) {
        ImGui::Text("%s", message.c_str());
    }
    
    if(ImGui::GetScrollY() >= ImGui::GetScrollMaxY()){
        ImGui::SetScrollHereY(1.0f);
    }

    // ImGui::Text("Message count: %zu", console_log_messages.size());
    
    ImGui::End();
}