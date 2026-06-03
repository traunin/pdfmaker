#include "pdfmaker/layout/code_block.hpp"
#include "pdfmaker/document.hpp"

#include <sstream>

namespace pdfmaker {

CodeBlock::CodeBlock(const std::string& code, const FontFamily* fonts, float font_size)
    : code_(code)
    , fonts_(fonts)
    , font_size_(font_size)
{
}

std::vector<std::string> CodeBlock::split_lines() const {
    std::vector<std::string> lines;
    std::istringstream stream(code_);
    std::string line;
    while (std::getline(stream, line)) {
        lines.push_back(line);
    }
    if (lines.empty()) {
        lines.push_back("");
    }
    return lines;
}

float CodeBlock::estimate_height(const LayoutContext& /*ctx*/) const {
    auto lines = split_lines();
    float line_h = font_size_ * line_spacing_;
    return static_cast<float>(lines.size()) * line_h + padding_ * 2.0f + 8.0f; // 8pt spacing
}

LayoutResult CodeBlock::layout(LayoutContext& ctx) {
    Font mono = fonts_->resolve(false, false, true);
    if (!mono) {
        mono = fonts_->regular; // fallback
    }

    auto lines = split_lines();
    float line_h = font_size_ * line_spacing_;
    float block_height = static_cast<float>(lines.size()) * line_h + padding_ * 2.0f;

    float space_before = 4.0f;
    float space_after = 4.0f;

    ctx.cursor_y -= space_before;

    // check if entire block fits on current page
    if (ctx.cursor_y - block_height < ctx.page_bottom) {
        ctx.new_page();
    }

    float block_top = ctx.cursor_y;
    float block_left = ctx.content_left;
    float block_width = ctx.content_width;

    // draw background rectangle
    ctx.page->set_fill_color(bg_color_);
    ctx.page->rectangle(block_left, block_top - block_height, block_width, block_height);
    ctx.page->fill();

    // render code lines
    float text_y = block_top - padding_ - font_size_;
    for (const auto& line : lines) {
        if (text_y < ctx.page_bottom) {
            ctx.new_page();
            text_y = ctx.cursor_y - padding_ - font_size_;
        }

        auto text_scope = ctx.page->begin_text();
        ctx.page->set_font(mono, font_size_);
        ctx.page->set_fill_color(Color::black());
        ctx.page->text_out(block_left + padding_, text_y, line);

        text_y -= line_h;
    }

    ctx.cursor_y = block_top - block_height - space_after;

    return {block_height + space_before + space_after, false};
}

} // namespace pdfmaker
