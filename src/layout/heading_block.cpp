#include "pdfmaker/layout/heading_block.hpp"

namespace pdfmaker {

HeadingBlock::HeadingBlock(int level, const std::string& text, ParagraphStyle style)
    : TextBlock(text, std::move(style))
    , level_(level)
{
}

std::string HeadingBlock::outline_text() const {
    std::string full_text;
    for (const auto& run : runs_) {
        full_text += run.text;
    }
    return full_text;
}

void HeadingBlock::collect_outline(std::vector<TocOutlineItem>& out) const {
    out.push_back(TocOutlineItem{level_, outline_text()});
}

LayoutResult HeadingBlock::layout(LayoutContext& ctx) {
    // register with TOC collector if present
    if (ctx.toc) {
        TocEntry entry;
        entry.level = level_;
        entry.text = outline_text();
        entry.page_number = ctx.current_page_number;
        ctx.toc->entries.push_back(std::move(entry));
    }

    return TextBlock::layout(ctx);
}

} // namespace pdfmaker
