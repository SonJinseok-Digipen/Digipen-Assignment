/*
 * Rudy Castan
 * CS200
 * Fall 2022
 */

#pragma once

#include "GLHandle.h"
#include <GL/glew.h>
#include <span>

struct GLIndexElement
{
    enum Type : unsigned
    {
        UInt   = GL_UNSIGNED_INT,
        UShort = GL_UNSIGNED_SHORT,
        UByte  = GL_UNSIGNED_BYTE,
        None   = 0
    };
};

class [[nodiscard]] GLIndexBuffer
{
    GLIndexElement::Type element_type   = GLIndexElement::None;
    GLsizei              count          = 0;
    GLHandle             indices_handle = 0;

public:
    GLIndexBuffer() = default;
    GLIndexBuffer(std::span<const unsigned int> indices);
    GLIndexBuffer(std::span<const unsigned short> indices);
    GLIndexBuffer(std::span<const unsigned char> indices);
    ~GLIndexBuffer();

    GLIndexBuffer(const GLIndexBuffer&) = delete;
    GLIndexBuffer& operator=(const GLIndexBuffer&) = delete;
    GLIndexBuffer(GLIndexBuffer&& temp) noexcept;
    GLIndexBuffer& operator=(GLIndexBuffer&& temp) noexcept;

    void Use(bool bind = true) const;

    [[nodiscard]] GLHandle GetHandle() const noexcept
    {
        return indices_handle;
    }

    [[nodiscard]] GLsizei GetCount() const noexcept
    {
        return count;
    }

    [[nodiscard]] GLIndexElement::Type GetElementType() const noexcept
    {
        return element_type;
    }
};
