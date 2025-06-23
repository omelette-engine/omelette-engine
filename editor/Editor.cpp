#include "../imgui/imgui.h"

#include "Editor.h"
#include "../World.h"
#include "../print_helper.h"
#include "../primitive_generator.h"
#include "../omelette_style.h"


void Editor::initialise(GLFWwindow* window) {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImFont* victor_mono = io.Fonts->AddFontFromFileTTF("../third_party/fonts/victor_mono.ttf", 24.0f);
    
    if(victor_mono == nullptr){
        log_warning("failed to load victor mono! :(");
    } else{
        io.FontDefault = victor_mono;
    }

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    ImGuiStyle& style = ImGui::GetStyle();
    style.WindowBorderSize = 2.0f;
    style.WindowRounding = 8.0f;
    style.Colors[ImGuiCol_Border] = ImVec4(0.831f, 0.827f, 0.82f, 1.0f);        // window border colour
    style.Colors[ImGuiCol_WindowBg] = ImVec4(0.16f, 0.12f, 0.10f, 1.0f);        // window background colour
    style.Colors[ImGuiCol_Text] = ImVec4(0.16f, 0.12f, 0.10f, 1.0f);          // text colour
    style.Colors[ImGuiCol_TitleBg] = ImVec4(0.831f, 0.827f, 0.82f, 1.0f);         // inactive title bar colour
    style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.831f, 0.827f, 0.82f, 1.0f);   // active title bar colour
}

void Editor::begin_frame() {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

void Editor::render() {
    render_menu();
    render_object_list();
    render_object_properties();
    render_debug_console();
}

void Editor::end_frame() {
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

bool menu_button(const char* text, int index, int total_count) {
    ImVec2 text_size = ImGui::CalcTextSize(text);
    ImVec2 window_size = ImGui::GetWindowSize();
    
    float section_width = window_size.x / total_count;
    float text_x = (section_width * index) + (section_width - text_size.x) / 2.0f;
    float text_y = (window_size.y - text_size.y) / 2.0f;
    
    // Create invisible button for the entire section
    ImGui::SetCursorPos(ImVec2(section_width * index, 0));
    bool clicked = ImGui::InvisibleButton(text, ImVec2(section_width, window_size.y));
    
    // Check if we're hovering this section
    bool is_hovered = ImGui::IsItemHovered();
    
    // Style the background if hovered
    if (is_hovered) {
        ImDrawList* draw_list = ImGui::GetWindowDrawList();
        ImVec2 window_pos = ImGui::GetWindowPos();
        
        // Determine which corners to round
        ImDrawFlags corner_flags = 0;
        if (index == 0) {
            corner_flags = ImDrawFlags_RoundCornersLeft; // Left corners only
        } else if (index == total_count - 1) {
            corner_flags = ImDrawFlags_RoundCornersRight; // Right corners only
        }
        // Middle buttons get no rounding (corner_flags = 0)
        
        draw_list->AddRectFilled(
            ImVec2(window_pos.x + section_width * index, window_pos.y),
            ImVec2(window_pos.x + section_width * (index + 1), window_pos.y + window_size.y),
            IM_COL32(41, 30, 26, 255), // #291e1a
            11.0f,
            corner_flags
        );
    }
    
    // Draw the text with appropriate color
    ImGui::SetCursorPos(ImVec2(text_x, text_y));
    if (is_hovered) {
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.831f, 0.827f, 0.82f, 1.0f)); // #d4d3d1
    }
    ImGui::Text("%s", text);
    if (is_hovered) {
        ImGui::PopStyleColor();
    }
    
    return clicked;
}


void Editor::render_menu(){
    ImGuiIO& io = ImGui::GetIO();
    float window_width = io.DisplaySize.x;
    float window_height = io.DisplaySize.y;

    float menu_width = window_width * get_percentage(30);
    float menu_height = window_height * get_percentage(3);
    float padding = 40.0f; // top padding
    float available_width = window_width * get_percentage(80);
    float menu_x = (available_width - menu_width) / 2.0f;

    ImGui::SetNextWindowPos(ImVec2(menu_x, padding));
    ImGui::SetNextWindowSize(ImVec2(menu_width, menu_height));

    // imgui overrides
    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.831f, 0.827f, 0.82f, 1.0f));

    ImGuiWindowFlags toolbar_flags = ImGuiWindowFlags_NoMove | 
                                    ImGuiWindowFlags_NoResize | 
                                    ImGuiWindowFlags_NoCollapse |
                                    ImGuiWindowFlags_NoTitleBar |
                                    ImGuiWindowFlags_NoScrollbar;

    ImGui::Begin(" ", nullptr, toolbar_flags);

    // menu options
    if(menu_button("add", 0, 5)){
        Mesh* new_mesh = generate_primitive("cube");
        if(new_mesh){
            world.add_object("cube", new_mesh, vector3(0.0f, 0.0f, 0.0f), default_colour, default_shadow);
        }
    }
    if(menu_button("pack-ins", 1, 5)){
    }
    if(menu_button("all", 2, 5)){
    }
    if(menu_button("menu", 3, 5)){
    }
    if(menu_button("option", 4, 5)){
    }

    ImGui::End();
    ImGui::PopStyleColor();
}


void Editor::render_object_list(){
    ImGuiIO& io = ImGui::GetIO();
    float window_width = io.DisplaySize.x;
    float window_height = io.DisplaySize.y;

    float object_list_width = window_width * get_percentage(20);
    float object_list_height = window_height * get_percentage(39);

    ImGui::SetNextWindowPos(ImVec2(window_width - object_list_width, 0));
    ImGui::SetNextWindowSize(ImVec2(object_list_width, object_list_height));

    ImGuiWindowFlags object_list_flags = ImGuiWindowFlags_NoMove | 
                                     ImGuiWindowFlags_NoResize | 
                                     ImGuiWindowFlags_NoCollapse;

    ImGui::Begin("object list (every object in the scene)", nullptr, object_list_flags);
    ImGui::End();
}

void Editor::render_object_properties(){
    ImGuiIO& io = ImGui::GetIO();
    float window_width = io.DisplaySize.x;
    float window_height = io.DisplaySize.y;

    float object_properties_width = window_width * get_percentage(20);
    float object_properties_height = window_height * get_percentage(39);

    float object_list_height = window_height * get_percentage(40);
    ImGui::SetNextWindowPos(ImVec2(window_width - object_properties_width, object_list_height));
    ImGui::SetNextWindowSize(ImVec2(object_properties_width, object_properties_height));

    ImGuiWindowFlags properties_flags = ImGuiWindowFlags_NoMove | 
                                       ImGuiWindowFlags_NoResize | 
                                       ImGuiWindowFlags_NoCollapse;

    ImGui::Begin("properties (properties for selected object)", nullptr, properties_flags);
    ImGui::End();
}

void Editor::render_debug_console(){
    ImGuiIO& io = ImGui::GetIO();
    float window_width = io.DisplaySize.x;
    float window_height = io.DisplaySize.y;

    float console_width = window_width * get_percentage(20);
    float console_height = window_height * get_percentage(20);

    ImGui::SetNextWindowPos(ImVec2(window_width - console_width, window_height - console_height));
    ImGui::SetNextWindowSize(ImVec2(console_width, console_height));

    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.831f, 0.827f, 0.82f, 1.0f));


    ImGuiWindowFlags console_flags = ImGuiWindowFlags_NoMove | 
                                     ImGuiWindowFlags_NoResize | 
                                     ImGuiWindowFlags_NoCollapse;

    ImGui::Begin("console (watch for errors!)", nullptr, console_flags);
    for(const auto& message : get_console_messages()) {
        ImGui::Text("%s", message.c_str());
    }
    
    if(ImGui::GetScrollY() >= ImGui::GetScrollMaxY()){
        ImGui::SetScrollHereY(1.0f);
    }
    
    ImGui::End();
    ImGui::PopStyleColor();
}