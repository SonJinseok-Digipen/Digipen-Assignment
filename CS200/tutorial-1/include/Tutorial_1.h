/*
 * Rudy Castan
 * CS200
 * Fall 2022
 */

#pragma once

#include "GLVertexArray.h"
#include "IProgram.h"
#include "GLShader.h"

#include <string>
#include <vector>
#include <unordered_map>

class Tutorial_1 : public IProgram
{
public:
    Tutorial_1(int viewport_width, int viewport_height);

    void Update() override;
    void Draw() override;
    void ImGuiDraw() override;
    void HandleEvent(SDL_Window& sdl_window, const SDL_Event& event) override;

private:
    struct
    {
        unsigned long long prev_time  = 0;
        unsigned long long start_time = 0;
        double             fps        = 0;
        int                count      = 0;
    } timing;

    // TODO
    std::unordered_map <std::string, std::string>settings_descriptions;

private:
    void create_1buffer_struct_of_arrays_style();
    void create_1buffer_array_of_structs_style();
    void create_parallel_buffers_style();
    void on_window_resized(SDL_Window& sdl_window) noexcept;
    void update_timing() noexcept;
    void print_and_save_opengl_settings() noexcept;
    // TODO
};
