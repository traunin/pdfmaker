#include "pdfmaker/layout/list_block.hpp"
#include "pdfmaker/document.hpp"

namespace pdfmaker {

ListBlock::ListBlock(ListType type, const FontFamily* fonts, float font_size)
    : type_(type)
    , fonts_(fonts)
    , font_size_(font_size)
{
}

void ListBlock::add_item(const std::string& text) {
    item_texts_.push_back(text);

    ParagraphStyle style;
    style.text.font_family = fonts_;
    style.text.font_size = font_size_;
    style.text.color = Color::black();
    style.alignment = TextAlignment::Justify;
    style.line_spacing = 1.5f;
    style.first_line_indent = 0.0f;
    style.space_after = 2.0f;

    items_.push_back(std::make_unique<TextBlock>(text, style));
}

void ListBlock::add_item(std::unique_ptr<ContentBlock> block) {
    item_texts_.push_back("");
    items_.push_back(std::move(block));
}

LayoutResult ListBlock::layout(LayoutContext& ctx) {
    float total_height = 0.0f;
    bool page_break = false;
    float space_before = 4.0f;
    float space_after = 4.0f;

    ctx.cursor_y -= space_before;
    total_height += space_before;

    // save original content area and narrow it for list items
    float original_left = ctx.content_left;
    float original_width = ctx.content_width;
    ctx.content_left += indent_;
    ctx.content_width -= indent_;

    for (size_t i = 0; i < items_.size(); ++i) {
        // render bullet/number
        std::string marker;
        if (type_ == ListType::Unordered) {
            marker = "-";
        } else {
            marker = std::to_string(i + 1) + ".";
        }

        Font font = fonts_->regular;
        float marker_width = font.text_width(marker, font_size_);

        // draw marker to the left of the indent
        {
            auto text_scope = ctx.page->begin_text();
            ctx.page->set_font(font, font_size_);
            ctx.page->set_fill_color(Color::black());
            float marker_x = ctx.content_left - marker_width - 4.0f;
            float marker_y = ctx.cursor_y - font_size_ * 1.5f; // approximate first line position
            ctx.page->text_out(marker_x, marker_y, marker);
        }

        // layout the item content
        auto result = items_[i]->layout(ctx);
        total_height += result.height_used;
        if (result.page_break_occurred) page_break = true;
    }

    // restore content area
    ctx.content_left = original_left;
    ctx.content_width = original_width;

    ctx.cursor_y -= space_after;
    total_height += space_after;

    return {total_height, page_break};
}

float ListBlock::estimate_height(const LayoutContext& ctx) const {
    float h = 8.0f; // space before + after
    LayoutContext modified = ctx;
    modified.content_left += indent_;
    modified.content_width -= indent_;
    for (const auto& item : items_) {
        h += item->estimate_height(modified);
    }
    return h;
}

} // namespace pdfmaker
