#pragma once
#include "../imgui/imgui.h"
#include "../imgui/imgui_impl_glfw.h"
#include "../imgui/imgui_impl_opengl3.h"

#include "../World.h"

class Editor{
    private:
    World& world;
    public:
        Editor(World& w) : world(w) {}
        void initialise(GLFWwindow* window);
        void begin_frame();
        void render();
        void end_frame();
        void render_menu();
        void render_object_list();
        void render_object_properties();
        void render_debug_console();
};