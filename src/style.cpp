#include "pdfmaker/style.hpp"

namespace pdfmaker {

Font TextStyle::resolved_font() const {
    if (!font_family) return {};
    return font_family->resolve(bold, italic, mono);
}

float PageStyle::content_width(float page_width) const {
    return page_width - margin_left - margin_right;
}

float PageStyle::content_height(float page_height) const {
    return page_height - margin_top - margin_bottom;
}

} // namespace pdfmaker
