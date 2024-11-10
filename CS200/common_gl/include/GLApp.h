/*
 * Rudy Castan
 * CS200
 * Fall 2022
 */

#pragma once

#include "owner.h"

struct SDL_Window;
typedef void* SDL_GLContext;
class IProgram;

class [[nodiscard]] GLApp
{
public:
    GLApp(const char* title = "OpenGL App", int desired_width = 800, int desired_height = 600);
    ~GLApp();

    GLApp(const GLApp&) = delete;
    GLApp& operator=(const GLApp&) = delete;
    GLApp(GLApp&&) noexcept        = delete;
    GLApp& operator=(GLApp&&) noexcept = delete;

    void Update();
    bool IsDone() const noexcept;

private:
    util::owner<IProgram*>     ptr_program = nullptr;
    util::owner<SDL_Window*>   ptr_window  = nullptr;
    util::owner<SDL_GLContext> gl_context  = nullptr;
    bool                       is_done     = false;
};
