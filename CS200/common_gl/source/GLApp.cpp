/*
 * Rudy Castan
 * CS200
 * Fall 2022
 */

#include "GLApp.h"
#include "IProgram.h"
#include "ImGuiHelper.h"

#include <GL/glew.h>
#include <SDL2/SDL.h>
#include <iostream>
#include <sstream>


namespace
{
    void hint_gl(SDL_GLattr attr, int value)
    {
        // https://wiki.libsdl.org/SDL_GL_SetAttribute
        if (const auto success = SDL_GL_SetAttribute(attr, value); success != 0)
        {
            std::cerr << "Failed to Set GL Attribute: " << SDL_GetError() << '\n';
        }
    }

    template <typename... Messages> [[noreturn]] void throw_error_message(Messages&&... more_messages)
    {
        std::ostringstream sout;
        (sout << ... << more_messages);
        throw std::runtime_error{sout.str()};
    }
}


GLApp::GLApp(const char* title, int desired_width, int desired_height)
{
    if (title == nullptr || title[0] == '\0')
        throw_error_message("App title shouldn't be empty");

    // https://wiki.libsdl.org/SDL_Init
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        throw_error_message("Failed to init SDK error: ", SDL_GetError());
    }
    hint_gl(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    // hint_gl(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    // hint_gl(SDL_GL_CONTEXT_MINOR_VERSION, 5);
    hint_gl(SDL_GL_DOUBLEBUFFER, true);
    hint_gl(SDL_GL_STENCIL_SIZE, 8);
    hint_gl(SDL_GL_DEPTH_SIZE, 24);
    hint_gl(SDL_GL_RED_SIZE, 8);
    hint_gl(SDL_GL_GREEN_SIZE, 8);
    hint_gl(SDL_GL_BLUE_SIZE, 8);
    hint_gl(SDL_GL_ALPHA_SIZE, 8);
    hint_gl(SDL_GL_MULTISAMPLEBUFFERS, 1);
    hint_gl(SDL_GL_MULTISAMPLESAMPLES, 4);

    // https://wiki.libsdl.org/SDL_CreateWindow
    desired_width  = std::max(640, std::min(16384, desired_width));
    desired_height = std::max(480, std::min(16384, desired_height));
    ptr_window     = SDL_CreateWindow(title, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, desired_width, desired_height, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
    if (ptr_window == nullptr)
    {
        throw_error_message("Failed to create window: ", SDL_GetError());
    }

    // https://wiki.libsdl.org/SDL_GL_CreateContext
    if (gl_context = SDL_GL_CreateContext(ptr_window); gl_context == nullptr)
    {
        throw_error_message("Failed to create opengl context: ", SDL_GetError());
    }

    // https://wiki.libsdl.org/SDL_GL_MakeCurrent
    SDL_GL_MakeCurrent(ptr_window, gl_context);

    // http://glew.sourceforge.net/basic.html
    if (const auto result = glewInit(); GLEW_OK != result)
    {
        throw_error_message("Unable to initialize GLEW - error: ", glewGetErrorString(result));
    }

    // https://wiki.libsdl.org/SDL_GL_SetSwapInterval
    constexpr int ADAPTIVE_VSYNC = -1;
    constexpr int VSYNC          = 1;
    if (const auto result = SDL_GL_SetSwapInterval(ADAPTIVE_VSYNC); result != 0)
    {
        SDL_GL_SetSwapInterval(VSYNC);
    }

    ImGuiHelper::Initialize(ptr_window, gl_context);

    // https://wiki.libsdl.org/SDL_GL_GetDrawableSize
    int width = 0, height = 0;
    SDL_GL_GetDrawableSize(ptr_window, &width, &height);
    ptr_program = create_program(width, height);
}


GLApp::~GLApp()
{
    delete ptr_program;
    ImGuiHelper::Shutdown();
    SDL_GL_DeleteContext(gl_context);
    SDL_DestroyWindow(ptr_window);
    SDL_Quit();
}

void GLApp::Update()
{
    SDL_Event event{0};
    while (SDL_PollEvent(&event) != 0)
    {
        ImGuiHelper::FeedEvent(event);
        ptr_program->HandleEvent(*ptr_window, event);
        if (event.type == SDL_QUIT || (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE)) [[unlikely]]
        {
            is_done = true;
        }
    }
    ptr_program->Update();
    ptr_program->Draw();

    ImGuiHelper::Begin();
    ptr_program->ImGuiDraw();
    ImGuiHelper::End(ptr_window, gl_context);

    SDL_GL_SwapWindow(ptr_window);
}

bool GLApp::IsDone() const noexcept
{
    return is_done;
}
