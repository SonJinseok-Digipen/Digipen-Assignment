/*
 * Rudy Castan
 * CS200
 * Fall 2022
 */

#include "mat3.h"

#include <cmath>
#include <limits>

mat3 mat3::build_rotation(float angle_in_radians) noexcept
{
    float cos_angle          = std::cos(angle_in_radians);
    float sin_angle          = std::sin(angle_in_radians);
    float negative_sin_angle = -sin_angle;
    if (std::abs(sin_angle) <= std::numeric_limits<float>::epsilon())
    {
        sin_angle = negative_sin_angle = 0.0f;
    }
    if (std::abs(cos_angle) <= std::numeric_limits<float>::epsilon())
    {
        cos_angle = 0.0f;
    }
    return {{cos_angle, sin_angle, 0}, {negative_sin_angle, cos_angle, 0}, {0, 0, 1}};
}
