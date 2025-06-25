#pragma once
#include "../imgui/imgui.h"
#include "../imgui/imgui_impl_glfw.h"
#include "../imgui/imgui_impl_opengl3.h"

#include "../World.h"
#include "../Object.h"

class Editor{
    public:
        Camera& camera;
        World& world;
        std::string selected_object_name;
        Editor(World& w, Camera& cam) : world(w), camera(cam) {}
        void initialise(GLFWwindow* window);
        void begin_frame();
        void render();
        void end_frame();
        void render_menu();
        void render_object_list();
        void render_object_properties();
        void render_debug_console();
        void handle_clicks(double x_position, double y_position);
        void test_ray_against_objects(vector3 ray_origin, vector3 ray_direction);
        const std::string& get_selected_object() const { return selected_object_name; }
        void clear_selection() { selected_object_name.clear(); }
        bool has_selection() const { return !selected_object_name.empty(); }
        bool gizmos_visible = false;
        void render_selected_outline(Shader& shader, Camera& camera);
        void render_selected_gizmos(Shader& shader, Camera& camera);
};