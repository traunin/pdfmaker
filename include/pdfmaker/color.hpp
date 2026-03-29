#pragma once

#include <string_view>

namespace pdfmaker {

struct Color {
    float r = 0.0f;
    float g = 0.0f;
    float b = 0.0f;

    constexpr Color() = default;
    constexpr Color(float r, float g, float b) : r(r), g(g), b(b) {}

    static constexpr Color rgb(float r, float g, float b) { return {r, g, b}; }
    static constexpr Color black() { return {0.0f, 0.0f, 0.0f}; }
    static constexpr Color white() { return {1.0f, 1.0f, 1.0f}; }
    static constexpr Color gray(float v = 0.5f) { return {v, v, v}; }

    static Color from_hex(std::string_view hex);

    constexpr bool operator==(const Color& other) const {
        return r == other.r && g == other.g && b == other.b;
    }
    constexpr bool operator!=(const Color& other) const {
        return !(*this == other);
    }
};

} // namespace pdfmaker
