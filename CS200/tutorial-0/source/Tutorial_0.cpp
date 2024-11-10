/*
 * Rudy Castan
 * CS200
 * Fall 2022
 */

#include "Tutorial_0.h"

#include "glCheck.h"
#include <GL/glew.h>
#include <SDL2/SDL.h>
#include <filesystem>
#include <imgui.h>
#include <iostream>
#include <string>

util::owner<IProgram*> create_program(int viewport_width, int viewport_height)
{
    return new Tutorial_0(viewport_width, viewport_height);
}

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

namespace
{
    bool LoadTextureFromFile(const std::filesystem::path& filename, GLuint& out_texture, int& out_width, int& out_height);
}

Tutorial_0::Tutorial_0(int viewport_width, int viewport_height)
{
    environment.WindowWidth  = viewport_width;
    environment.WindowHeight = viewport_height;
    glCheck(glViewport(0, 0, viewport_width, viewport_height));
    glCheck(glClearColor(background_color[0], background_color[1], background_color[2], 1.0f));

    std::cout << std::filesystem::current_path() << '\n';

    example_image.loaded = LoadTextureFromFile("assets/images/DigiPen_Dragon.png", example_image.handle, example_image.width, example_image.height);

    timing.start_time = SDL_GetTicks64();
    timing.prev_time  = timing.start_time;
}

Tutorial_0::~Tutorial_0()
{
    if (example_image.loaded)
        glCheck(glDeleteTextures(1, &example_image.handle));
}

void Tutorial_0::Update()
{
    environment.FrameCount++;

    update_timing();
}

void Tutorial_0::Draw()
{
    glCheck(glClearColor(background_color[0], background_color[1], background_color[2], 1.0f));
    glCheck(glClear(GL_COLOR_BUFFER_BIT));
}

void Tutorial_0::ImGuiDraw()
{
    ImGui::Begin("Program Settings");
    {
        ImGui::ColorEdit3("Background Color", background_color);
    }
    ImGui::End();

    ImGui::Begin("Program Info");
    {
        ImGui::LabelText("FPS", "%.1f", static_cast<double>(timing.fps));
        ImGui::LabelText("Elapsed time", "%.1f seconds", static_cast<double>(environment.ElapsedTime));
        ImGui::LabelText("Delta time", "%.4f seconds", static_cast<double>(environment.DeltaTime));
        ImGui::LabelText("Frame Count", "%.d", environment.FrameCount);
        ImGui::LabelText("Mouse State", "Pos (%d, %d) Vel (%d, %d)", environment.MouseX, environment.MouseY, environment.MouseVelocityX, environment.MouseVelocityY);
        ImGui::LabelText("Last Key State", "%s", label_keys.c_str());
    }
    ImGui::End();

    if (example_image.loaded)
    {
        ImGui::Begin("OpenGL Texture Test");
        ImGui::Text("handle = %d", example_image.handle);
        ImGui::Text("size = %d x %d", example_image.width, example_image.height);
        ImGui::Image(reinterpret_cast<void*>(static_cast<intptr_t>(example_image.handle)),
                     ImVec2(static_cast<float>(example_image.width), static_cast<float>(example_image.height)));
        ImGui::End();
    }

    ImGui::ShowDemoWindow();
}

void Tutorial_0::HandleEvent(SDL_Window& sdl_window, const SDL_Event& event)
{
    switch (event.type)
    {
        case SDL_QUIT << '\n':
            break;
        case SDL_KEYDOWN:
        case SDL_KEYUP:
        {
            using namespace std;
            const int   scan_code     = event.key.keysym.scancode;
            const char  key_sym       = static_cast<char>(event.key.keysym.sym);
            const char* pressed_state = nullptr;
            if (event.key.state == SDL_PRESSED)
                pressed_state = "Pressed";
            else // SDL_RELEASED
                pressed_state = "Released";
            label_keys = string("Key ") + key_sym + " (" + to_string(scan_code) + ") is " + pressed_state;
            std::clog << label_keys << '\n';
        }
        break;
        case SDL_MOUSEMOTION:
            environment.MouseX         = event.motion.x;
            environment.MouseY         = event.motion.y;
            environment.MouseVelocityX = event.motion.xrel;
            environment.MouseVelocityY = event.motion.yrel;
            break;
        case SDL_MOUSEWHEEL:
            std::clog << "Mouse Wheel (" << event.wheel.preciseX << ", " << event.wheel.preciseY << ")\n";
            break;
        case SDL_MOUSEBUTTONDOWN:
        case SDL_MOUSEBUTTONUP:
        {
            const char* button_text = "";
            switch (event.button.button)
            {
                case SDL_BUTTON_LEFT:
                    button_text = "Left";
                    break;
                case SDL_BUTTON_MIDDLE:
                    button_text = "Middle";
                    break;
                case SDL_BUTTON_RIGHT:
                    button_text = "Right";
                    break;
                default: // SDL_BUTTON_X1, SDL_BUTTON_X2
                    button_text = "Extra Button";
                    break;
            }
            const char* pressed_state = "";
            if (event.key.state == SDL_PRESSED)
                pressed_state = "Pressed";
            else // SDL_RELEASED
                pressed_state = "Released";
            const char* click = "";
            if (event.button.clicks == 1)
                click = "Single Click ";
            else if (event.button.clicks == 2)
                click = "Double Click ";
            std::clog << click << pressed_state << " on " << button_text << " at (" << event.button.x << "," << event.button.y << ")\n";
        }
        break;
        case SDL_WINDOWEVENT:
        {
            switch (event.window.event)
            {
                case SDL_WINDOWEVENT_SHOWN:
                    std::clog << "Window " << event.window.windowID << " shown" << '\n';
                    break;
                case SDL_WINDOWEVENT_HIDDEN:
                    std::clog << "Window " << event.window.windowID << " hidden" << '\n';
                    break;
                case SDL_WINDOWEVENT_EXPOSED:
                    std::clog << "Window " << event.window.windowID << " exposed" << '\n';
                    break;
                case SDL_WINDOWEVENT_MOVED:
                    std::clog << "Window " << event.window.windowID << " moved to " << event.window.data1 << ',' << event.window.data2 << '\n';
                    break;
                case SDL_WINDOWEVENT_RESIZED:
                {
                    std::clog << "Window " << event.window.windowID << " resized to " << event.window.data1 << 'x' << event.window.data2 << '\n';
                    on_window_resized(sdl_window);
                }
                break;
                case SDL_WINDOWEVENT_SIZE_CHANGED:
                {

                    std::clog << "Window " << event.window.windowID << " size changed to " << event.window.data1 << 'x' << event.window.data2 << '\n';
                    on_window_resized(sdl_window);
                }
                break;
                case SDL_WINDOWEVENT_MINIMIZED:
                    std::clog << "Window " << event.window.windowID << " minimized" << '\n';
                    break;
                case SDL_WINDOWEVENT_MAXIMIZED:
                    std::clog << "Window " << event.window.windowID << " maximized" << '\n';
                    break;
                case SDL_WINDOWEVENT_RESTORED:
                    std::clog << "Window " << event.window.windowID << " restored" << '\n';
                    break;
                case SDL_WINDOWEVENT_ENTER:
                    std::clog << "Mouse entered window " << event.window.windowID << '\n';
                    break;
                case SDL_WINDOWEVENT_LEAVE:
                    std::clog << "Mouse left window " << event.window.windowID << '\n';
                    break;
                case SDL_WINDOWEVENT_FOCUS_GAINED:
                    std::clog << "Window " << event.window.windowID << " gained keyboard focus" << '\n';
                    break;
                case SDL_WINDOWEVENT_FOCUS_LOST:
                    std::clog << "Window " << event.window.windowID << " lost keyboard focus" << '\n';
                    break;
                case SDL_WINDOWEVENT_CLOSE:
                    std::clog << "Window " << event.window.windowID << " closed" << '\n';
                    break;
                case SDL_WINDOWEVENT_TAKE_FOCUS:
                    std::clog << "Window " << event.window.windowID << " is offered a focus" << '\n';
                    break;
                case SDL_WINDOWEVENT_HIT_TEST:
                    std::clog << "Window " << event.window.windowID << " has a special hit test" << '\n';
                    break;
                default:
                    std::clog << "Window " << event.window.windowID << " got unknown event " << event.window.event << '\n';
                    break;
            }
        }
    }
}

void Tutorial_0::on_window_resized(SDL_Window& sdl_window) noexcept
{
    SDL_GL_GetDrawableSize(&sdl_window, &environment.WindowWidth, &environment.WindowHeight);
    glCheck(glViewport(0, 0, environment.WindowWidth, environment.WindowHeight));
}

void Tutorial_0::update_timing() noexcept
{
    const auto current_time = SDL_GetTicks64();
    const auto delta_ms     = current_time - timing.prev_time;
    environment.DeltaTime   = delta_ms / 1'000.0f;
    environment.ElapsedTime += environment.DeltaTime;

    timing.prev_time      = current_time;
    const auto elapsed_ms = current_time - timing.start_time;

    timing.count++;
    if (elapsed_ms > 1'000)
    {
        timing.fps        = timing.count / (elapsed_ms / 1'000.0f);
        timing.start_time = current_time;
        timing.count      = 0;
    }
}


namespace
{
    // https://github.com/ocornut/imgui/wiki/Image-Loading-and-Displaying-Examples
    bool LoadTextureFromFile(const std::filesystem::path& filename, GLuint& out_texture, int& out_width, int& out_height)
    {
        // Load from file
        int            image_width  = 0;
        int            image_height = 0;
        unsigned char* image_data   = stbi_load(filename.string().c_str(), &image_width, &image_height, NULL, 4);
        if (image_data == NULL)
            return false;

        // Create a OpenGL texture identifier
        GLuint image_texture;
        glCheck(glGenTextures(1, &image_texture));
        glBindTexture(GL_TEXTURE_2D, image_texture);

        // Setup filtering parameters for display
        glCheck(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
        glCheck(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
        glCheck(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE)); // This is required on WebGL for non power-of-two textures
        glCheck(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE)); // Same

        // Upload pixels into texture
#if defined(GL_UNPACK_ROW_LENGTH) && !defined(__EMSCRIPTEN__)
        glCheck(glPixelStorei(GL_UNPACK_ROW_LENGTH, 0));
#endif
        glCheck(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image_width, image_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image_data));
        stbi_image_free(image_data);

        out_texture = image_texture;
        out_width   = image_width;
        out_height  = image_height;

        return true;
    }
}
