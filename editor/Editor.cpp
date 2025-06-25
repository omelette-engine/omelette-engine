#include "../imgui/imgui.h"

#include "Editor.h"
#include "../World.h"
#include "../Camera.h"
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
    
    ImGui::SetCursorPos(ImVec2(section_width * index, 0));
    bool clicked = ImGui::InvisibleButton(text, ImVec2(section_width, window_size.y));
    
    bool is_hovered = ImGui::IsItemHovered();
    
    if (is_hovered) {
        ImDrawList* draw_list = ImGui::GetWindowDrawList();
        ImVec2 window_pos = ImGui::GetWindowPos();
        
        ImDrawFlags corner_flags = 0;
        if (index == 0) {
            corner_flags = ImDrawFlags_RoundCornersLeft;
        } else if (index == total_count - 1) {
            corner_flags = ImDrawFlags_RoundCornersRight;
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

// bool dropdown_button(const char* text) {
//     if(ImGui::IsItemHovered()) {
//         ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.831f, 0.827f, 0.82f, 1.0f)); // white text on hover
//     } else {
//         ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.161f, 0.118f, 0.102f, 1.0f)); // dark text normally
//     }
    
//     bool clicked = ImGui::Button(text);
//     ImGui::PopStyleColor(3);
    
//     return clicked;
// }


void Editor::render_menu(){
    ImGuiIO& io = ImGui::GetIO();
    float window_width = io.DisplaySize.x;
    float window_height = io.DisplaySize.y;

    float menu_width = window_width * get_percentage(30);
    float menu_height = window_height * get_percentage(3);
    float padding = 40.0f; // top padding
    float available_width = window_width * get_percentage(80);
    float menu_x = (available_width - menu_width) / 2.0f;

    static bool show_add_menu = false;
    static const char* primitives[] = {"cube", "sphere", "cylinder", "plane", "torus", "cone"};
    static int selected_primitive = 0;

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
        show_add_menu = !show_add_menu;
    }

    if(show_add_menu) {
        ImGui::SetNextWindowPos(ImVec2(menu_x, padding + menu_height));
        ImGuiWindowFlags toolbar_flags = ImGuiWindowFlags_NoMove | 
                                    ImGuiWindowFlags_NoResize | 
                                    ImGuiWindowFlags_NoCollapse |
                                    ImGuiWindowFlags_NoScrollbar;
        
        // ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.161f, 0.118f, 0.102f, 0.95f));
        ImGui::Begin("Add Primitive", &show_add_menu, toolbar_flags);

        // ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.831f, 0.827f, 0.820f, 1.0f));
        // ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.286f, 0.400f, 0.412f, 1.0f));
        
        for(int i = 0; i < IM_ARRAYSIZE(primitives); i++) {
            ImVec2 cursor_pos = ImGui::GetCursorScreenPos();
            ImVec2 button_size = ImGui::CalcTextSize(primitives[i]);
            button_size.x += ImGui::GetStyle().FramePadding.x * 2;
            button_size.y += ImGui::GetStyle().FramePadding.y * 2;
            bool will_be_hovered = ImGui::IsMouseHoveringRect(cursor_pos, ImVec2(cursor_pos.x + button_size.x, cursor_pos.y + button_size.y));


            if(ImGui::Button(primitives[i])) {
                Mesh* new_mesh = generate_primitive(primitives[i]);
                if(new_mesh) {
                    world.add_object(primitives[i], new_mesh, vector3(0.0f, 0.0f, 0.0f), default_colour, default_shadow);
                }
                show_add_menu = false;
                break;
            }
        }
        ImGui::End();
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
    
    if (has_selection()) {
        auto* selected_obj = world.get_object(selected_object_name);
        if (selected_obj) {
            ImGui::Text("Object: %s", selected_object_name.c_str());
            ImGui::Separator();
            
            if (selected_obj->type == object_type::MESH) {
                vector3 lit_color = selected_obj->get_lit_colour();
                if (ImGui::ColorEdit3("Lit Color", &lit_color.x)) {
                    selected_obj->set_lit_colour(lit_color);
                }
                
                vector3 shadow_color = selected_obj->get_shadow_colour();
                if (ImGui::ColorEdit3("Shadow Color", &shadow_color.x)) {
                    selected_obj->set_shadow_colour(shadow_color);
                }
            } else if (selected_obj->type == object_type::LIGHT) {
                vector3 light_color = selected_obj->get_light_colour();
                if (ImGui::ColorEdit3("Light Color", &light_color.x)) {
                    selected_obj->set_light_colour(light_color);
                }
                
                float intensity = selected_obj->get_intensity();
                if (ImGui::SliderFloat("Intensity", &intensity, 0.0f, 10.0f)) {
                    selected_obj->set_intensity(intensity);
                }
            }
        }
    } else {
        ImGui::Text("No object selected");
    }
    
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

    ImGui::PushStyleColor(ImGuiCol_TitleBgActive, ImVec4(0.831f, 0.827f, 0.82f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_TitleBg, ImVec4(0.831f, 0.827f, 0.82f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_TitleBgCollapsed, ImVec4(0.831f, 0.827f, 0.82f, 1.0f));
    
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.161f, 0.118f, 0.102f, 1.0f));

    ImGuiWindowFlags console_flags = ImGuiWindowFlags_NoMove | 
                                     ImGuiWindowFlags_NoResize | 
                                     ImGuiWindowFlags_NoCollapse;

    ImGui::Begin("console (watch for errors!)", nullptr, console_flags);
    
    // Override text color for console messages only
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.831f, 0.827f, 0.82f, 1.0f));
    for(const auto& message : get_console_messages()) {
        ImGui::Text("%s", message.c_str());
    }
    ImGui::PopStyleColor();
    
    if(ImGui::GetScrollY() >= ImGui::GetScrollMaxY()){
        ImGui::SetScrollHereY(1.0f);
    }
    
    ImGui::End();
    ImGui::PopStyleColor(4);
}

void Editor::handle_clicks(double x_position, double y_position) {
    ImGuiIO& io = ImGui::GetIO();
    if (io.WantCaptureMouse) {
        return;
    }
    
    float screen_width = io.DisplaySize.x;
    float screen_height = io.DisplaySize.y;

    float normalised_coordinates_x = (2.0f * x_position) / screen_width - 1.0f;
    float normalised_coordinates_y = 1.0f - (2.0f * y_position) / screen_height;

    matrix4 inverse_camera = glm::inverse(camera.camera_matrix);

    vector4 near_plane = vector4(normalised_coordinates_x, normalised_coordinates_y, -1.0f, 1.0f);
    vector4 far_plane = vector4(normalised_coordinates_x, normalised_coordinates_y, 1.0f, 1.0f);

    vector4 world_space_near_plane = inverse_camera * near_plane;
    vector4 world_space_far_plane = inverse_camera * far_plane;

    world_space_near_plane /= world_space_near_plane.w;
    world_space_far_plane /= world_space_far_plane.w;

    vector3 ray_origin = vector3(world_space_near_plane);
    vector3 ray_direction = normalise(vector3(world_space_far_plane) - vector3(world_space_near_plane));

    test_ray_against_objects(ray_origin, ray_direction);
}

void Editor::test_ray_against_objects(vector3 ray_origin, vector3 ray_direction) {
    float closest_distance = std::numeric_limits<float>::max();
    std::string closest_object_name;
    
    for (const auto& object : world.get_objects()) {
        AABB world_aabb = object.get_world_aabb();
        float hit_distance;
        
        if (world_aabb.intersects_ray(ray_origin, ray_direction, hit_distance)) {
            if (hit_distance < closest_distance) {
                closest_distance = hit_distance;
                closest_object_name = object.name;
            }
        }
    }
    
    if (!closest_object_name.empty()) {
        selected_object_name = closest_object_name;
        log_info("selected " + closest_object_name);
    } else {
        clear_selection();
        log_info("nothing selected");
    }
}

void Editor::render_selected_outline(Shader& shader, Camera& camera) {
    if(selected_object_name.empty()) return;
    
    for(const auto& obj : world.get_objects()) {
        if(obj.name == selected_object_name && obj.type == object_type::MESH) {
            // disable depth writing for outline
            glDepthMask(GL_FALSE);
            
            shader.Activate();
            camera.Matrix(shader, "camera_matrix");
            
            matrix4 outline_matrix = glm::scale(obj.model_matrix, vector3(1.02f));
            glUniformMatrix4fv(glGetUniformLocation(shader.ID, "model"), 1, GL_FALSE, get_raw_data(outline_matrix));
            
            // yellow outline color
            glUniform3f(glGetUniformLocation(shader.ID, "lit_colour"), 0.851f, 0.643f, 0.263f);
            glUniform3f(glGetUniformLocation(shader.ID, "shadow_colour"), 0.851f, 0.643f, 0.263f);
            
            obj.mesh_data.mesh->render();
            
            // re-enable depth writing
            glDepthMask(GL_TRUE);
            break;
        }
    }
}

void Editor::render_selected_gizmos(Shader& shader, Camera& camera){
    if(selected_object_name.empty()){
        gizmos_visible = false;
        return;
    }

    auto* selected_obj = world.get_object(selected_object_name);
    if (!selected_obj) {
        return;
    }

    // we're getting the object position!
    vector3 object_position = selected_obj->position;
    if(!gizmos_visible){
        for(int i = 0; i < 2; i++){
            log_info("gizmo!\n");
            Mesh* new_mesh = generate_primitive("cone");
            world.add_object("cone", new_mesh, object_position, default_colour, default_shadow);
        }
        gizmos_visible = true;
    }
}