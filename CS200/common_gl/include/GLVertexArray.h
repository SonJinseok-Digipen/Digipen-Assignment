/*
 * Rudy Castan
 * CS200
 * Fall 2022
 */

#pragma once

#include "GLHandle.h"
#include "GLIndexBuffer.h"
#include "GLVertexBuffer.h"
#include <GL/glew.h>
#include <memory>
#include <vector>

struct GLAttributeLayout
{
    enum ComponentType : GLenum
    {
        Float = GL_FLOAT,
        Int   = GL_INT,
        Bool  = GL_BOOL
    };
    ComponentType component_type = ComponentType::Float;
    enum NumComponents : GLint
    {
        _1 = 1,
        _2 = 2,
        _3 = 3,
        _4 = 4
    };
    NumComponents component_dimension = NumComponents::_1;
    // Is this the 1st, 2nd, 3rd... (0, 1, 2...) "in attribute" of the vertex shader?
    GLuint vertex_layout_location = 0;
    // should be false for float types
    // if normalized is GL_TRUE, then integer data is normalized to the range [-1, 1]
    // If normalized is GL_FALSE then integer data is directly converted to floating point.
    GLboolean normalized = GL_FALSE;
    // byte offset to read the very 1st attribute
    // should be 0 for parallel array and struct of arrays
    // should be offsetof(Vertex, field) for array of structs
    GLuint relative_offset = 0;
    // byte offset into where we are in the buffer
    //  do we start at the beginning or somewhere in the middle for this attribute?
    GLintptr offset = 0;
    // how many bytes to step to the next attribute
    GLsizei stride = 0;
};
struct GLPrimitive
{
    enum Type : GLenum
    {
        Points        = GL_POINTS,
        Lines         = GL_LINES,
        LineLoop      = GL_LINE_LOOP,
        LineStrip     = GL_LINE_STRIP,
        Triangles     = GL_TRIANGLES,
        TriangleStrip = GL_TRIANGLE_STRIP,
        TriangleFan   = GL_TRIANGLE_FAN,
    };
};

class [[nodiscard]] GLVertexArray
{
    GLHandle                    vertex_array_handle = 0;
    std::vector<GLVertexBuffer> vertex_buffers;
    GLIndexBuffer               index_buffer;
    GLsizei                     num_indices       = 0;
    GLIndexElement::Type        indices_type      = GLIndexElement::None;
    GLPrimitive::Type           primitive_pattern = GLPrimitive::Triangles;
    GLsizei                     num_vertices      = 0;

public:
    explicit GLVertexArray(GLPrimitive::Type the_primitive_pattern = GLPrimitive::Triangles);
    ~GLVertexArray();

    GLVertexArray(const GLVertexArray&) = delete;
    GLVertexArray& operator=(const GLVertexArray&) = delete;
    GLVertexArray(GLVertexArray&& temp) noexcept;
    GLVertexArray& operator=(GLVertexArray&& temp) noexcept;

    void Use(bool bind = true) const;

    void AddVertexBuffer(GLVertexBuffer&& vertex_buffer, std::initializer_list<GLAttributeLayout> buffer_layout);
    void SetIndexBuffer(GLIndexBuffer&& the_indices);

    [[nodiscard]] GLHandle GetHandle() const noexcept
    {
        return vertex_array_handle;
    }

    [[nodiscard]] const auto& GetVertexBuffers() const
    {
        return vertex_buffers;
    }

    [[nodiscard]] const auto& GetIndexBuffer() const
    {
        return index_buffer;
    }

    [[nodiscard]] GLsizei GetIndicesCount() const noexcept
    {
        return num_indices;
    }

    [[nodiscard]] GLIndexElement::Type GetIndicesType() const noexcept
    {
        return indices_type;
    }

    [[nodiscard]] GLPrimitive::Type GetPrimitivePattern() const
    {
        return primitive_pattern;
    }
    void SetPrimitivePattern(GLPrimitive::Type the_primitive_pattern)
    {
        primitive_pattern = the_primitive_pattern;
    }

    [[nodiscard]] GLsizei GetVertexCount() const
    {
        return num_vertices;
    }
    void SetVertexCount(int vertex_count)
    {
        num_vertices = vertex_count;
    }
};

void GLDrawIndexed(const GLVertexArray& vertex_array) noexcept;
void GLDrawVertices(const GLVertexArray& vertex_array) noexcept;
