#pragma once

#include "pdfmaker/layout/text_block.hpp"

namespace pdfmaker {

class HeadingBlock : public TextBlock {
public:
    HeadingBlock(int level, const std::string& text, ParagraphStyle style);

    int level() const { return level_; }

    void collect_outline(std::vector<TocOutlineItem>& out) const override;

    LayoutResult layout(LayoutContext& ctx) override;

private:
    int level_;

    std::string outline_text() const;
};

} // namespace pdfmaker
