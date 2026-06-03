#pragma once

#include "pdfmaker/layout/content_block.hpp"
#include "pdfmaker/page.hpp"

#include <string>
#include <vector>

namespace pdfmaker {

class TocBlock : public ContentBlock {
public:
    explicit TocBlock(const std::string& title = "Contents");

    LayoutResult layout(LayoutContext& ctx) override;
    void finalize(LayoutContext& ctx) override;
    float estimate_height(const LayoutContext& ctx) const override;

private:
    // one reserved row, filled in during finalize() once page numbers are known
    struct Slot {
        Page page;
        float y = 0.0f;
    };

    //TOC entry reserved during layout
    struct Entry {
        int level = 1;
        std::vector<std::string> lines; // wrapped text lines
        std::vector<Slot> slots;        // parallel to lines
    };

    std::vector<std::string> wrap_text(Font font, float size,
                                       const std::string& text, float max_width) const;

    std::string title_;
    float title_font_size_ = 16.0f;
    float entry_font_size_ = 14.0f;
    float line_spacing_ = 1.5f;
    float indent_per_level_ = 20.0f;

    std::vector<Entry> entries_;
};

} // namespace pdfmaker
