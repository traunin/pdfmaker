#include "pdfmaker/layout/page_break.hpp"

namespace pdfmaker {

LayoutResult PageBreakBlock::layout(LayoutContext& ctx) {
    ctx.new_page();
    return {0.0f, true};
}

float PageBreakBlock::estimate_height(const LayoutContext& /*ctx*/) const {
    return 0.0f;
}

} // namespace pdfmaker
