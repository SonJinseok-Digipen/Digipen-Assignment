/*
 * Rudy Castan
 * CS200
 * Fall 2022
 */

#pragma once

#include <GL/glew.h>
#include <GLHandle.h>
#include <span>


class [[nodiscard]] GLVertexBuffer
{
    GLsizei  size          = 0;
    GLHandle buffer_handle = 0;

public:
    explicit GLVertexBuffer(GLsizei size_in_bytes);

    template <typename T, auto N = std::dynamic_extent>
    GLVertexBuffer(std::span<T, N> buffer_data) : GLVertexBuffer(static_cast<GLsizei>(buffer_data.size_bytes()))
    {
        send_buffer_data_to_gpu(buffer_data.data(), size, 0);
    }

    ~GLVertexBuffer();

    GLVertexBuffer(const GLVertexBuffer&) = delete;
    GLVertexBuffer& operator=(const GLVertexBuffer&) = delete;
    GLVertexBuffer(GLVertexBuffer&& temp) noexcept;
    GLVertexBuffer& operator=(GLVertexBuffer&& temp) noexcept;

    void Use(bool bind = true) const;

    template <typename T, auto N = std::dynamic_extent>
    void SetData(std::span<T, N> buffer_data, GLsizei starting_offset = 0) const
    {
        send_buffer_data_to_gpu(buffer_data.data(), static_cast<GLsizei>(buffer_data.size_bytes()), starting_offset);
    }

    [[nodiscard]] GLHandle GetHandle() const noexcept
    {
        return buffer_handle;
    }

    [[nodiscard]] GLsizei GetSizeBytes() const noexcept
    {
        return size;
    }

private:
    void send_buffer_data_to_gpu(const void* data, GLsizei size_bytes, GLsizei starting_offset) const;
};
