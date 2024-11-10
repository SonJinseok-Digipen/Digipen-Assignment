/*
 * Rudy Castan
 * CS200
 * Fall 2022
 */

#include "GLShader.h"

#include "glCheck.h"

#include <GL/glew.h>
#include <array>
#include <fstream>
#include <iostream>
#include <vector>


namespace
{
    [[nodiscard]] bool Compile(GLuint& shader, GLShader::Type type, std::string_view glsl_text, std::string& error_log)
    {
        if (shader <= 0)
        {
            glCheck(shader = glCreateShader(type));
            if (shader == 0)
            {
                error_log = "Unable to create shader\n";
                return false;
            }
        }
        GLchar const* source[]{glsl_text.data()};
        glCheck(glShaderSource(shader, 1, source, nullptr));
        glCheck(glCompileShader(shader));

        GLint is_compiled = 0;
        glCheck(glGetShaderiv(shader, GL_COMPILE_STATUS, &is_compiled));
        if (is_compiled == GL_FALSE)
        {
            GLint log_length = 0;
            glCheck(glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &log_length));
            error_log.resize(static_cast<std::string::size_type>(log_length) + 1);
            glCheck(glGetShaderInfoLog(shader, log_length, nullptr, error_log.data()));
            glCheck(glDeleteShader(shader));
            shader = 0;
            return false;
        }
        return true;
    }


    [[nodiscard]] bool Compile(GLuint& shader, GLShader::Type type, const std::filesystem::path& file_path, std::string& error_log)
    {
        if (!std::filesystem::exists(file_path))
        {
            error_log = "Cannot find " + file_path.string() + "\n";
            return false;
        }
        std::ifstream ifs(file_path, std::ios::in);
        if (!ifs)
        {
            error_log = "Cannot open " + file_path.string() + "\n";
            return false;
        }
        std::string glsl_text;
        glsl_text.reserve(std::filesystem::file_size(file_path));
        std::copy((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>(), std::back_insert_iterator(glsl_text));
        return Compile(shader, type, std::string_view(glsl_text), error_log);
    }
}


GLShader::GLShader(std::string_view the_shader_name, const std::initializer_list<std::pair<GLShader::Type, std::filesystem::path>>& shader_paths)
    : program_handle(0), shader_name(the_shader_name), uniforms()
{
    try
    {
        // Compile all shaders and attach them to the program
        std::vector<GLuint>::size_type index = 0;
        std::vector<GLuint>            shader(shader_paths.size());
        for (const auto& [shader_type, shader_path] : shader_paths)
        {
            std::string error;
            const bool  is_compiled = Compile(shader[index], shader_type, shader_path, error);
            // if any shader is failed to compile -> delete all shaders and program
            if (!is_compiled)
            {
                for (auto& shdr : shader)
                {
                    if (shdr > 0)
                    {
                        glCheck(glDeleteShader(shdr));
                    }
                }
                throw std::runtime_error(error);
            }
            index++;
        }
        link_program(shader);
#if defined(_DEBUG) || defined(DEBUG)
        print_active_attributes();
        print_active_uniforms();
#endif
    }
    catch (...)
    {
        delete_program();
        throw;
    }
}

GLShader::GLShader(std::string_view the_shader_name, std::string_view vertex_shader_source, std::string_view fragment_shader_source)
    : program_handle(0), shader_name(the_shader_name), uniforms()
{
    std::vector<GLuint>  shader(2);
    const std::array     sources = {vertex_shader_source, fragment_shader_source};
    constexpr std::array types   = {GLShader::Type::VERTEX, GLShader::Type::FRAGMENT};
    std::string          error;
    for (unsigned index = 0; index < 2; ++index)
    {
        const bool is_compiled = Compile(shader[index], types[index], sources[index], error);
        // if any shader is failed to compile -> delete all shaders and program
        if (!is_compiled)
        {
            for (auto shdr : shader)
            {
                if (shdr > 0)
                {
                    glCheck(glDeleteShader(shdr));
                }
            }
            throw std::runtime_error(error);
        }
    }
    link_program(shader);
#if defined(_DEBUG) || defined(DEBUG)
    print_active_attributes();
    print_active_uniforms();
#endif
}

GLShader::~GLShader()
{
    delete_program();
}

GLShader::GLShader(GLShader&& temp) noexcept : program_handle{temp.program_handle}, shader_name{std::move(temp.shader_name)}, uniforms{std::move(temp.uniforms)}
{
    temp.program_handle = 0;
}

GLShader& GLShader::operator=(GLShader&& temp) noexcept
{
    std::swap(program_handle, temp.program_handle);
    std::swap(shader_name, temp.shader_name);
    std::swap(uniforms, temp.uniforms);
    return *this;
}

void GLShader::Use(bool bind) const noexcept
{
    glCheck(glUseProgram(bind ? program_handle : 0));
}

bool GLShader::IsValidWithVertexArrayObject(unsigned int vertex_array_object_handle) const
{
    if (program_handle == 0)
    {
        return false;
    }
    Use();
    glCheck(glBindVertexArray(vertex_array_object_handle));
    // Check validate
    glCheck(glValidateProgram(program_handle));
    GLint is_validate = 0;
    glCheck(glGetProgramiv(program_handle, GL_VALIDATE_STATUS, &is_validate));
    glCheck(glBindVertexArray(0));
    Use(false);
    if (is_validate == GL_FALSE)
    {
        GLint log_length = 0;
        glCheck(glGetProgramiv(program_handle, GL_INFO_LOG_LENGTH, &log_length));
        std::string error;
        error.resize(static_cast<unsigned>(log_length) + 1);
        glCheck(glGetProgramInfoLog(program_handle, log_length, nullptr, error.data()));
        std::cerr << error << '\n';
        return false;
    }
    return true;
}

void GLShader::SendUniform(std::string_view name, bool value) const
{
    glCheck(glUniform1i(get_uniform_location(name), static_cast<int>(value)));
}
void GLShader::SendUniform(std::string_view name, int value) const
{
    glCheck(glUniform1i(get_uniform_location(name), value));
}
void GLShader::SendUniform(std::string_view name, float value) const
{
    glCheck(glUniform1f(get_uniform_location(name), value));
}
void GLShader::SendUniform(std::string_view name, std::span<const float, 2> value) const
{
    glCheck(glUniform2fv(get_uniform_location(name), 1, value.data()));
}
void GLShader::SendUniform(std::string_view name, float x, float y) const
{
    glCheck(glUniform2f(get_uniform_location(name), x, y));
}
void GLShader::SendUniform(std::string_view name, std::span<const float, 3> value) const
{
    glCheck(glUniform3fv(get_uniform_location(name), 1, value.data()));
}
void GLShader::SendUniform(std::string_view name, float x, float y, float z) const
{
    glCheck(glUniform3f(get_uniform_location(name), x, y, z));
}
void GLShader::SendUniform(std::string_view name, std::span<const float, 4> value) const
{
    glCheck(glUniform4fv(get_uniform_location(name), 1, value.data()));
}
void GLShader::SendUniform(std::string_view name, float x, float y, float z, float w) const
{
    glCheck(glUniform4f(get_uniform_location(name), x, y, z, w));
}
void GLShader::SendUniform(std::string_view name, std::span<const float, 2 * 2> mat, GLShader::Transpose transpose) const
{
    glCheck(glUniformMatrix2fv(get_uniform_location(name), 1, (transpose) ? GL_TRUE : GL_FALSE, mat.data()));
}
void GLShader::SendUniform(std::string_view name, std::span<const float, 3 * 3> mat, GLShader::Transpose transpose) const
{
    glCheck(glUniformMatrix3fv(get_uniform_location(name), 1, (transpose) ? GL_TRUE : GL_FALSE, mat.data()));
}
void GLShader::SendUniform(std::string_view name, std::span<const float, 4 * 4> mat, GLShader::Transpose transpose) const
{
    glCheck(glUniformMatrix4fv(get_uniform_location(name), 1, (transpose) ? GL_TRUE : GL_FALSE, mat.data()));
}

void GLShader::link_program(const std::vector<unsigned int>& shader)
{
    glCheck(program_handle = glCreateProgram());
    if (program_handle == 0)
    {
        throw std::runtime_error("Unable to create program\n");
    }

    for (const auto shader_handle : shader)
    {
        glCheck(glAttachShader(program_handle, shader_handle));
    }

    // Link shaders to the program
    glCheck(glLinkProgram(program_handle));
    for (auto& shdr : shader)
    {
        glCheck(glDeleteShader(shdr));
    }
    GLint is_linked = 0;
    glCheck(glGetProgramiv(program_handle, GL_LINK_STATUS, &is_linked));
    if (is_linked == GL_FALSE)
    {
        GLint log_length = 0;
        glCheck(glGetProgramiv(program_handle, GL_INFO_LOG_LENGTH, &log_length));
        std::string error;
        error.resize(static_cast<unsigned>(log_length) + 1);
        glCheck(glGetProgramInfoLog(program_handle, log_length, nullptr, error.data()));
        throw std::runtime_error(error);
    }
}

int GLShader::get_uniform_location(std::string_view uniform_name) const noexcept
{
    const auto iter_location = uniforms.find(uniform_name);
    if (iter_location == uniforms.end())
    {
        glCheck(GLint location = glGetUniformLocation(program_handle, uniform_name.data()));
        if (location < 0)
        {
            std::cerr << "Uniform variable(" << uniform_name << ") does not exist" << std::endl;
            location = -1;
        }
        uniforms[std::string(uniform_name)] = location;
        return location;
    }
    return iter_location->second;
}

void GLShader::delete_program() noexcept
{
    if (program_handle > 0)
    {
        glCheck(glDeleteProgram(program_handle));
        program_handle = 0;
    }
}

void GLShader::print_active_uniforms() const
{
    GLint max_length;
    glCheck(glGetProgramiv(program_handle, GL_ACTIVE_UNIFORM_MAX_LENGTH, &max_length));
    std::string name;
    name.resize(static_cast<unsigned>(max_length) + 1);
    GLint num_uniforms;
    glCheck(glGetProgramiv(program_handle, GL_ACTIVE_UNIFORMS, &num_uniforms));
    std::cout << "----------------------------------------------------------------------" << '\r' << shader_name << " uniforms\n";
    std::cout << "Location\t|\tName\n";
    for (GLint i = 0; i < num_uniforms; ++i)
    {
        GLsizei written;
        GLint   size;
        GLenum  type;
        name.clear();
        name.resize(static_cast<unsigned>(max_length) + 1);
        glCheck(glGetActiveUniform(program_handle, i, max_length, &written, &size, &type, name.data()));
        name.resize(static_cast<std::string::size_type>(written));
        const auto loc = get_uniform_location(name);
        std::cout << loc << "\t\t" << name << '\n';
    }
    std::cout << "----------------------------------------------------------------------\n";
}

void GLShader::print_active_attributes() const
{
    GLint max_length, num_attribs;
    glCheck(glGetProgramiv(program_handle, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, &max_length));
    glCheck(glGetProgramiv(program_handle, GL_ACTIVE_ATTRIBUTES, &num_attribs));
    std::string name;
    name.resize(static_cast<unsigned>(max_length) + 1);
    std::cout << "----------------------------------------------------------------------" << '\r' << shader_name << " vertex attributes\n";
    std::cout << "Index\t|\tName\n";

    for (GLint i = 0; i < num_attribs; ++i)
    {
        GLsizei written;
        GLint   size;
        GLenum  type;
        name.clear();
        name.resize(static_cast<unsigned>(max_length) + 1);
        glCheck(glGetActiveAttrib(program_handle, i, max_length, &written, &size, &type, name.data()));
        name.resize(static_cast<std::string::size_type>(written));
        glCheck(GLint loc = glGetAttribLocation(program_handle, name.c_str()));
        std::cout << loc << "\t\t" << name << '\n';
    }
    std::cout << "----------------------------------------------------------------------\n";
}
