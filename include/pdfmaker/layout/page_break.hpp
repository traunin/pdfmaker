#pragma once

#include "pdfmaker/layout/content_block.hpp"

namespace pdfmaker {

class PageBreakBlock : public ContentBlock {
public:
    LayoutResult layout(LayoutContext& ctx) override;
    float estimate_height(const LayoutContext& ctx) const override;
};

} // namespace pdfmaker
