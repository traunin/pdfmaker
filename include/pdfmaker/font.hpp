#pragma once

#include <hpdf.h>
#include <string_view>

namespace pdfmaker {

class Font {
public:
    Font() = default;
    explicit Font(HPDF_Font handle) : handle_(handle) {}

    float text_width(std::string_view text, float size) const;

    int ascent() const;
    int descent() const;

    HPDF_Font raw() const noexcept { return handle_; }
    explicit operator bool() const noexcept { return handle_ != nullptr; }

private:
    HPDF_Font handle_ = nullptr;
};

struct FontFamily {
    Font regular;
    Font bold;
    Font italic;
    Font bold_italic;
    Font mono;

    Font resolve(bool is_bold, bool is_italic, bool is_mono) const;
};

} // namespace pdfmaker
