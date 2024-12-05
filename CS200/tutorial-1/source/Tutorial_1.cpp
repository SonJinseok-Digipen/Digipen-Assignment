/*
 * Rudy Castan
 * CS200
 * Fall 2022
 */

#include "Tutorial_1.h"

#include "color3.h"
#include "glCheck.h"
#include "vec2.h"

#include <GL/glew.h>
#include <SDL2/SDL.h>
#include <array>
#include <imgui.h>
#include <iostream>

util::owner<IProgram*> create_program(int viewport_width, int viewport_height)
{
    return new Tutorial_1(viewport_width, viewport_height);
}

Tutorial_1::Tutorial_1(int viewport_width, int viewport_height)
{
    // TODO
    print_and_save_opengl_settings();
    glCheck(glViewport(0, 0, viewport_width, viewport_height));
    glCheck(glClearColor(0.392f, 0.584f, 0.929f, 1));
    timing.start_time = SDL_GetTicks64();
    timing.prev_time  = timing.start_time;
}

void Tutorial_1::Update()
{
    update_timing();
    // TODO
}


void Tutorial_1::Draw()
{
    glCheck(glClear(GL_COLOR_BUFFER_BIT));
    // TODO
}

void Tutorial_1::ImGuiDraw()
{
    ImGui::Begin("Program Info");
    {
        ImGui::LabelText("FPS", "%.1f", timing.fps);

        for (const auto& [label, description] : settings_descriptions)
        {
            ImGui::LabelText(label.c_str(), "%s", description.c_str());
        }
        {
            //ImGui::LabelText("Background Color", "RGB(%.0f,%.0f,%.0f)", background.red * 255, background.green * 255, background.blue * 255);
        }
    }
    ImGui::End();
}

void Tutorial_1::HandleEvent(SDL_Window& sdl_window, const SDL_Event& event)
{
    switch (event.type)
    {
        case SDL_WINDOWEVENT:
        {
            switch (event.window.event)
            {
                case SDL_WINDOWEVENT_RESIZED:
                {
                    on_window_resized(sdl_window);
                }
                break;
                case SDL_WINDOWEVENT_SIZE_CHANGED:
                {
                    on_window_resized(sdl_window);
                }
                break;
            }
        }
    }
}

void Tutorial_1::create_1buffer_struct_of_arrays_style()
{
    constexpr std::array positions           = {vec2{-0.2f, 0.2f}, vec2{-0.2f, 0.6f}, vec2{-0.6f, 0.6f}, vec2{-0.6f, 0.2f}};
    constexpr auto       positions_byte_size = static_cast<long long>(sizeof(vec2) * positions.size());
    constexpr std::array colors              = {color3{1, 1, 1}, color3{1, 0, 0}, color3{0, 1, 0}, color3{0, 0, 1}};
    constexpr auto       colors_byte_size    = static_cast<long long>(sizeof(color3) * colors.size());
    constexpr auto       buffer_size         = positions_byte_size + colors_byte_size;
    constexpr std::array<unsigned, 6> indices = {0, 1, 2, 2, 3, 0};

    // todo
}

namespace
{
    struct Vertex
    {
        vec2   position{};
        color3 color{};
    };

}

void Tutorial_1::create_1buffer_array_of_structs_style()
{
    constexpr std::array vertices = {Vertex{vec2{0.6f, 0.2f}, color3{1, 0, 0}}, Vertex{vec2{0.6f, 0.6f}, color3{0, 1, 0}}, Vertex{vec2{0.2f, 0.6f}, color3{0, 0, 1}},
                                     Vertex{vec2{0.2f, 0.2f}, color3{1, 1, 1}}};
    constexpr std::array<unsigned short, 6> indices = {0, 1, 2, 2, 3, 0};
    
    // TODO
}

void Tutorial_1::create_parallel_buffers_style()
{
    constexpr std::array positions = {vec2{0.6f, -0.6f}, vec2{0.6f, -0.2f}, vec2{-0.6f, -0.2f}, vec2{-0.6f, -0.6f}};
    constexpr std::array colors    = {color3{0, 0, 1}, color3{1, 1, 1}, color3{1, 0, 0}, color3{0, 1, 0}};
    constexpr std::array<unsigned char, 6> indices = {0, 1, 2, 2, 3, 0};

    // TODO
}

void Tutorial_1::on_window_resized(SDL_Window& sdl_window) noexcept
{
    int width = 0, height = 0;
    SDL_GL_GetDrawableSize(&sdl_window, &width, &height);
    glCheck(glViewport(0, 0, width, height));
}

void Tutorial_1::update_timing() noexcept
{
    const auto current_time = SDL_GetTicks64();

    timing.prev_time      = current_time;
    const auto elapsed_ms = current_time - timing.start_time;

    timing.count++;
    if (elapsed_ms > 1'000)
    {
        timing.fps        = timing.count / (elapsed_ms / 1'000.0);
        timing.start_time = current_time;
        timing.count      = 0;
    }
}

void Tutorial_1::print_and_save_opengl_settings() noexcept
{
    //GL_VENDOR: Intel
    //GL_RENDERER : Intel(R) Iris(R) Graphics 540
    //GL_VERSION : 4.6.0 - Build 30.0.101.1660
    //GL_SHADING_LANGUAGE_VERSION : 4.60 - Build 30.0.101.1660
    //GL_MAJOR_VERSION : 4
    //GL_MINOR_VERSION : 6
    //GL_MAX_ELEMENTS_VERTICES : 1048576
    //GL_MAX_ELEMENTS_INDICES : 1048576
    //GL_MAX_TEXTURE_IMAGE_UNITS : 32
    //GL_MAX_TEXTURE_SIZE : 16384
    //GL_MAX_VIEWPORT_DIMS : 16384 x 16384
    //GL_DOUBLEBUFFER : true

    std::string GL_VENDOR_KEY = "GL_VENDOR";
    std::string GL_RENDERER_KEY = "GL_RENDERER";
    std::string GL_VERSION_KEY = "GL_VERSION";
    std::string GL_SHADING_LANGUAGE_VERSION_KEY = "GL_SHADING_LANGUAGE_VERSION";
    std::string GL_MAJOR_VERSION_KEY = "GL_MAJOR_VERSION";
    std::string GL_MINOR_VERSION_KEY = "GL_MINOR_VERSION";
    std::string GL_MAX_ELEMENTS_VERTICES_KEY = "GL_MAX_ELEMENTS_VERTICES";
    std::string GL_MAX_ELEMENTS_INDICES_KEY = "GL_MAX_ELEMENTS_INDICES";
    std::string GL_MAX_TEXTURE_IMAGE_UNITS_KEY = "GL_MAX_TEXTURE_IMAGE_UNITS";
    std::string GL_MAX_TEXTURE_IMAGE_SIZE_KEY = "GL_MAX_TEXTURE_SIZE";
    std::string GL_MAX_VIEWPORT_DIMS_KEY = "GL_MAX_VIEWPORT_DIMS";
    std::string GL_DOUBLEBUFFER_KEY = "GL_DOUBLEBUFFER";

    const GLubyte* GL_VENDOR_V;
    const GLubyte* GL_RENDERER_V;
    const GLubyte* GL_VERSION_V;
    const GLubyte* GL_SHADING_LANGUAGE_VERSION_V;
    GLint GL_MAJOR_VERSION_V;
    GLint GL_MINOR_VERSION_V;
    GLint GL_MAX_ELEMENTS_VERTICES_V;
    GLint GL_MAX_ELEMENTS_INDICES_V;
    GLint GL_MAX_TEXTURE_IMAGE_UNITS_V;
    GLint GL_MAX_TEXTURE_SIZE_V;

    GLboolean GL_DOUBLEBUFFER_V;


    glCheck(GL_VENDOR_V = glGetString(GL_VENDOR));
    settings_descriptions[GL_VENDOR_KEY] = static_cast<std::string>(reinterpret_cast<const char*>(GL_VENDOR_V));
    std::cout << settings_descriptions[GL_VENDOR_KEY] << " " << GL_VENDOR_V << '\n';

    glCheck(GL_RENDERER_V = glGetString(GL_RENDERER));
    settings_descriptions[GL_RENDERER_KEY] = static_cast<std::string>(reinterpret_cast<const char*>(GL_RENDERER_V));
    std::cout << GL_VENDOR_KEY << " " << GL_VENDOR_V << '\n';

    glCheck(GL_VERSION_V = glGetString(GL_VERSION));
    settings_descriptions[GL_VERSION_KEY] = static_cast<std::string>(reinterpret_cast<const char*>(GL_VERSION_V));
    std::cout << GL_VERSION_KEY << " " << GL_VERSION_V << '\n';

    glCheck(GL_SHADING_LANGUAGE_VERSION_V = glGetString(GL_SHADING_LANGUAGE_VERSION));
    settings_descriptions[GL_SHADING_LANGUAGE_VERSION_KEY] = static_cast<std::string>(reinterpret_cast<const char*>(GL_SHADING_LANGUAGE_VERSION_V));
    std::cout << GL_SHADING_LANGUAGE_VERSION_KEY << " " << GL_SHADING_LANGUAGE_VERSION_V << '\n';


    glCheck(glGetIntegerv(GL_MAJOR_VERSION, &GL_MAJOR_VERSION_V));
    settings_descriptions[GL_MAJOR_VERSION_KEY] = std::to_string(GL_MAJOR_VERSION_V);
    std::cout << GL_MAJOR_VERSION_KEY << " " << GL_MAJOR_VERSION_V << '\n';

    glCheck(glGetIntegerv(GL_MINOR_VERSION, &GL_MINOR_VERSION_V));
    settings_descriptions[GL_MINOR_VERSION_KEY] = std::to_string(GL_MINOR_VERSION_V);
    std::cout << GL_MINOR_VERSION_KEY << " " << GL_MINOR_VERSION_V << '\n';

    glCheck(glGetIntegerv(GL_MAX_ELEMENTS_VERTICES, &GL_MAX_ELEMENTS_VERTICES_V));
    settings_descriptions[GL_MAX_ELEMENTS_VERTICES_KEY] = std::to_string(GL_MAX_ELEMENTS_VERTICES_V);
    std::cout << GL_MAX_ELEMENTS_VERTICES_KEY << " " << GL_MAX_ELEMENTS_VERTICES_V << '\n';


    glCheck(glGetIntegerv(GL_MAX_ELEMENTS_INDICES, &GL_MAX_ELEMENTS_INDICES_V));
    settings_descriptions[GL_MAX_ELEMENTS_INDICES_KEY] = std::to_string(GL_MAX_ELEMENTS_INDICES_V);
    std::cout << GL_MAX_ELEMENTS_INDICES_KEY << " " << GL_MAX_ELEMENTS_INDICES_V << '\n';


    glCheck(glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &GL_MAX_TEXTURE_IMAGE_UNITS_V));
    settings_descriptions[GL_MAX_TEXTURE_IMAGE_UNITS_KEY] = std::to_string(GL_MAX_TEXTURE_IMAGE_UNITS_V);
    std::cout << GL_MAX_TEXTURE_IMAGE_UNITS_KEY << " " << GL_MAX_TEXTURE_IMAGE_UNITS_V << '\n';

    glCheck(glGetIntegerv(GL_MAX_TEXTURE_SIZE, &GL_MAX_TEXTURE_SIZE_V));
    settings_descriptions[GL_MAX_TEXTURE_IMAGE_SIZE_KEY] = std::to_string(GL_MAX_TEXTURE_SIZE_V);
    std::cout << GL_MAX_TEXTURE_IMAGE_SIZE_KEY << " " << GL_MAX_TEXTURE_SIZE_V << '\n';


    settings_descriptions[GL_MAX_VIEWPORT_DIMS_KEY] = std::to_string(GL_MAX_TEXTURE_SIZE_V) + " X " + std::to_string(GL_MAX_TEXTURE_SIZE_V);


    glCheck(glGetBooleanv(GL_DOUBLEBUFFER, &GL_DOUBLEBUFFER_V));
    if (static_cast<bool>(GL_DOUBLEBUFFER_V) == true)
    {
        settings_descriptions[GL_DOUBLEBUFFER_KEY] = static_cast<std::string>("true");
    }
    else
    {
        settings_descriptions[GL_DOUBLEBUFFER_KEY] = static_cast<std::string>("false");
    }

    std::cout << std::boolalpha << GL_DOUBLEBUFFER_KEY << " " << static_cast<bool>(GL_MAX_TEXTURE_SIZE_V) << '\n';






}