/*
 * Rudy Castan
 * CS200
 * Fall 2022
 */

#pragma once

#include "IProgram.h"

#include <GL/glew.h>
#include <string>

class Tutorial_0 : public IProgram
{
public:
    Tutorial_0(int viewport_width, int viewport_height);
    ~Tutorial_0();

    Tutorial_0(const Tutorial_0&) = delete;
    Tutorial_0& operator=(const Tutorial_0&) = delete;
    Tutorial_0(Tutorial_0&&)                 = delete;
    Tutorial_0& operator=(Tutorial_0&&) = delete;

    void Update() override;
    void Draw() override;
    void ImGuiDraw() override;
    void HandleEvent(SDL_Window& sdl_window, const SDL_Event& event) override;

private:
    struct
    {
        int   MouseX{0};
        int   MouseY{0};
        int   MouseVelocityX{0};
        int   MouseVelocityY{0};
        int   WindowWidth{0};
        int   WindowHeight{0};
        int   FrameCount{0};
        float DeltaTime{0};
        float ElapsedTime{0};

    } environment;

    struct
    {
        unsigned long long prev_time  = 0;
        unsigned long long start_time = 0;
        float              fps        = 0;
        int                count;
    } timing;

    float background_color[3] = {0.392f, 0.584f, 0.929f}; // https://www.colorhexa.com/6495ed

    std::string label_keys{"No keys have been pressed"};

    struct
    {
        GLuint handle = 0;
        int    width  = 0;
        int    height = 0;
        bool   loaded = false;
    } example_image;

private:
    void on_window_resized(SDL_Window& sdl_window) noexcept;
    void update_timing() noexcept;
};
