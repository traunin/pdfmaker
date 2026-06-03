#include "pdfmaker/font.hpp"

namespace pdfmaker {

namespace {
    constexpr float FONT_UNITS_PER_EM = 1000.0f;
}

float Font::text_width(std::string_view text, float size) const {
    if (!handle_) return 0.0f;

    HPDF_TextWidth tw = HPDF_Font_TextWidth(handle_,
                                            reinterpret_cast<const HPDF_BYTE*>(text.data()),
                                            static_cast<HPDF_UINT>(text.size()));
    return static_cast<float>(tw.width) * size / FONT_UNITS_PER_EM;
}

int Font::ascent() const {
    if (!handle_) return 0;
    return HPDF_Font_GetAscent(handle_);
}

int Font::descent() const {
    if (!handle_) return 0;
    return HPDF_Font_GetDescent(handle_);
}

Font FontFamily::resolve(bool is_bold, bool is_italic, bool is_mono) const {
    if (is_mono && mono) return mono;
    if (is_bold && is_italic && bold_italic) return bold_italic;
    if (is_bold && bold) return bold;
    if (is_italic && italic) return italic;
    return regular;
}

} // namespace pdfmaker
