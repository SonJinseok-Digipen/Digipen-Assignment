/*
 * Rudy Castan
 * CS200
 * Fall 2022
 */

#pragma once

#pragma once

#include "GLHandle.h"
#include <GL/glew.h>
#include <filesystem>
#include <initializer_list>
#include <map>
#include <span>
#include <string>
#include <string_view>


class [[nodiscard]] GLShader
{
public:
    enum Type : GLenum
    {
        VERTEX                  = GL_VERTEX_SHADER,
        FRAGMENT                = GL_FRAGMENT_SHADER,
        GEOMETRY                = GL_GEOMETRY_SHADER,
        TESSELLATION_CONTROL    = GL_TESS_CONTROL_SHADER,
        TESSELLATION_EVALUATION = GL_TESS_EVALUATION_SHADER,
        COMPUTE                 = GL_COMPUTE_SHADER
    };

public:
    GLShader() = default;

    GLShader(std::string_view shader_name, const std::initializer_list<std::pair<GLShader::Type, std::filesystem::path>>& shader_paths);
    GLShader(std::string_view shader_name, std::string_view vertex_shader_source, std::string_view fragment_shader_source);
    ~GLShader();

    GLShader(const GLShader&) = delete;
    GLShader(GLShader&&) noexcept;
    GLShader& operator=(const GLShader&) = delete;
    GLShader& operator                   =(GLShader&&) noexcept;

    void Use(bool bind = true) const noexcept;

    bool IsValidWithVertexArrayObject(unsigned int vertex_array_object_handle) const;

    void SendUniform(std::string_view name, bool value) const;
    void SendUniform(std::string_view name, int value) const;
    void SendUniform(std::string_view name, float value) const;
    void SendUniform(std::string_view name, std::span<const float, 2> value) const;
    void SendUniform(std::string_view name, float x, float y) const;
    void SendUniform(std::string_view name, std::span<const float, 3> value) const;
    void SendUniform(std::string_view name, float x, float y, float z) const;
    void SendUniform(std::string_view name, std::span<const float, 4> value) const;
    void SendUniform(std::string_view name, float x, float y, float z, float w) const;
    enum Transpose : GLboolean
    {
        NO_TRANSPOSE = GL_FALSE,
        TRANSPOSE    = GL_TRUE
    };
    void SendUniform(std::string_view name, std::span<const float, 2 * 2> mat, Transpose transpose) const;
    void SendUniform(std::string_view name, std::span<const float, 3 * 3> mat, Transpose transpose = NO_TRANSPOSE) const;
    void SendUniform(std::string_view name, std::span<const float, 4 * 4> mat, Transpose transpose = NO_TRANSPOSE) const;

private:
    GLHandle                                        program_handle = 0;
    std::string                                     shader_name{};
    mutable std::map<std::string, int, std::less<>> uniforms{};

private:
    void              link_program(const std::vector<unsigned int>& shader);
    [[nodiscard]] int get_uniform_location(std::string_view uniform_name) const noexcept;
    void              delete_program() noexcept;
    void              print_active_uniforms() const;
    void              print_active_attributes() const;
};
