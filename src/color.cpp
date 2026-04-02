#include "pdfmaker/color.hpp"

#include <stdexcept>

namespace pdfmaker {

Color Color::from_hex(std::string_view hex) {
    if (hex.empty()) {
        throw std::invalid_argument("Empty hex");
    }
    if (hex[0] == '#') {
        hex.remove_prefix(1);
    }
    if (hex.size() != 6) {
        throw std::invalid_argument("Hex must be 6 characters");
    }

    auto parse_byte = [](char hi, char lo) -> uint8_t {
        auto nibble = [](char c) -> uint8_t {
            if (c >= '0' && c <= '9') return static_cast<uint8_t>(c - '0');
            if (c >= 'a' && c <= 'f') return static_cast<uint8_t>(c - 'a' + 10);
            if (c >= 'A' && c <= 'F') return static_cast<uint8_t>(c - 'A' + 10);
            throw std::invalid_argument("Invalid hex character");
        };
        return static_cast<uint8_t>((nibble(hi) << 4) | nibble(lo));
    };

    float r = static_cast<float>(parse_byte(hex[0], hex[1])) / 255.0f;
    float g = static_cast<float>(parse_byte(hex[2], hex[3])) / 255.0f;
    float b = static_cast<float>(parse_byte(hex[4], hex[5])) / 255.0f;
    return {r, g, b};
}

} // namespace pdfmaker
