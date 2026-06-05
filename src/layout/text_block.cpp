#include "pdfmaker/layout/text_block.hpp"
#include "pdfmaker/document.hpp"

#include <sstream>

namespace pdfmaker {

TextBlock::TextBlock(ParagraphStyle style)
    : para_style_(std::move(style))
{
}

TextBlock::TextBlock(const std::string& text, ParagraphStyle style)
    : para_style_(std::move(style))
{
    add_run(text, para_style_.text);
}

void TextBlock::add_run(const std::string& text, TextStyle style) {
    runs_.push_back({text, std::move(style)});
}

void TextBlock::add_run(TextSpan run) {
    runs_.push_back(std::move(run));
}

std::vector<TextBlock::Line> TextBlock::break_into_lines(float max_width) const {
    std::vector<Line> lines;
    Line current_line;
    current_line.total_width = 0.0f;
    current_line.line_height = 0.0f;

    bool first_line = true;
    float indent = para_style_.first_line_indent;
    float effective_width = max_width - (first_line ? indent : 0.0f);

    for (const auto& run : runs_) {
        Font font = run.style.resolved_font();
        if (!font) continue;

        float line_h = run.style.font_size * para_style_.line_spacing * LINE_HEIGHT_RATIO;
        float run_space_w = font.text_width(" ", run.style.font_size);

        // Split run text into words by spaces
        std::istringstream stream(run.text);
        std::string word_str;

        while (stream >> word_str) {
            float w = font.text_width(word_str, run.style.font_size);

            // Add space width if not the first word on the line
            float space_w = current_line.words.empty() ? 0.0f : run_space_w;

            if (current_line.total_width + space_w + w > effective_width && !current_line.words.empty()) {
                // Line is full, start a new one
                lines.push_back(std::move(current_line));
                current_line = Line{};
                current_line.total_width = 0.0f;
                current_line.line_height = 0.0f;
                first_line = false;
                effective_width = max_width;
                space_w = 0.0f;
            }

            if (!current_line.words.empty()) {
                // Add space before word
                current_line.total_width += space_w;
            }

            current_line.words.push_back({word_str, run.style, w, run_space_w});
            current_line.total_width += w;
            if (line_h > current_line.line_height) {
                current_line.line_height = line_h;
            }
        }
    }

    if (!current_line.words.empty()) {
        lines.push_back(std::move(current_line));
    }

    return lines;
}

void TextBlock::render_line(const Line& line, LayoutContext& ctx, float x, float y, float max_width,
                            bool is_last_line) const {
    if (line.words.empty()) return;

    auto text_scope = ctx.page->begin_text();

    // The last line of a justified paragraph is set flush-left, not stretched.
    if (para_style_.alignment == TextAlignment::Justify && line.words.size() > 1 && !is_last_line) {
        // Justify: distribute extra space between words
        float extra_space = max_width - line.total_width;
        float space_add = extra_space / static_cast<float>(line.words.size() - 1);

        float cx = x;
        for (size_t i = 0; i < line.words.size(); ++i) {
            const auto& word = line.words[i];
            Font font = word.style.resolved_font();
            ctx.page->set_font(font, word.style.font_size);
            ctx.page->set_fill_color(word.style.color);
            ctx.page->text_out(cx, y, word.text);
            cx += word.width;
            if (i + 1 < line.words.size()) {
                cx += word.space_width + space_add;
            }
        }
    } else if (para_style_.alignment == TextAlignment::Center) {
        float offset = (max_width - line.total_width) / 2.0f;
        float cx = x + offset;
        for (const auto& word : line.words) {
            Font font = word.style.resolved_font();
            ctx.page->set_font(font, word.style.font_size);
            ctx.page->set_fill_color(word.style.color);
            ctx.page->text_out(cx, y, word.text);
            cx += word.width + word.space_width;
        }
    } else if (para_style_.alignment == TextAlignment::Right) {
        float offset = max_width - line.total_width;
        float cx = x + offset;
        for (const auto& word : line.words) {
            Font font = word.style.resolved_font();
            ctx.page->set_font(font, word.style.font_size);
            ctx.page->set_fill_color(word.style.color);
            ctx.page->text_out(cx, y, word.text);
            cx += word.width + word.space_width;
        }
    } else {
        // Left align
        float cx = x;
        for (const auto& word : line.words) {
            Font font = word.style.resolved_font();
            ctx.page->set_font(font, word.style.font_size);
            ctx.page->set_fill_color(word.style.color);
            ctx.page->text_out(cx, y, word.text);
            cx += word.width + word.space_width;
        }
    }
}

LayoutResult TextBlock::layout(LayoutContext& ctx) {
    float total_height = 0.0f;
    bool page_break = false;

    auto lines = break_into_lines(ctx.content_width);

    // Space before paragraph
    ctx.cursor_y -= para_style_.space_before;
    total_height += para_style_.space_before;

    bool first_line = true;
    for (size_t i = 0; i < lines.size(); ++i) {
        const auto& line = lines[i];
        bool is_last_line = (i + 1 == lines.size());
        float line_h = line.line_height;

        // Check if we need a new page
        if (ctx.cursor_y - line_h < ctx.page_bottom) {
            ctx.new_page();
            page_break = true;
        }

        ctx.cursor_y -= line_h;

        float x = ctx.content_left;
        if (first_line) {
            x += para_style_.first_line_indent;
        }

        float effective_width = ctx.content_width;
        if (first_line) {
            effective_width -= para_style_.first_line_indent;
        }

        render_line(line, ctx, x, ctx.cursor_y, effective_width, is_last_line);
        total_height += line_h;
        first_line = false;
    }

    // Space after paragraph
    ctx.cursor_y -= para_style_.space_after;
    total_height += para_style_.space_after;

    return {total_height, page_break};
}

float TextBlock::estimate_height(const LayoutContext& ctx) const {
    auto lines = break_into_lines(ctx.content_width);
    float h = para_style_.space_before + para_style_.space_after;
    for (const auto& line : lines) {
        h += line.line_height;
    }
    return h;
}

} // namespace pdfmaker
