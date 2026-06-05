#include "pdfmaker/layout/toc_block.hpp"
#include "pdfmaker/document.hpp"

#include <algorithm>
#include <sstream>
#include <string>

namespace pdfmaker {

TocBlock::TocBlock(const std::string& title)
    : title_(title)
{
}

// greedy word wrap
std::vector<std::string> TocBlock::wrap_text(Font font, float size,
                                             const std::string& text, float max_width) const {
    std::vector<std::string> lines;
    std::istringstream stream(text);
    std::string word;
    std::string current;
    float current_w = 0.0f;
    float space_w = font.text_width(" ", size);

    while (stream >> word) {
        float word_w = font.text_width(word, size);
        if (current.empty()) {
            current = word;
            current_w = word_w;
        } else if (current_w + space_w + word_w <= max_width) {
            current += ' ';
            current += word;
            current_w += space_w + word_w;
        } else {
            lines.push_back(std::move(current));
            current = word;
            current_w = word_w;
        }
    }
    lines.push_back(std::move(current)); // last (possibly empty) line
    return lines;
}

float TocBlock::estimate_height(const LayoutContext& ctx) const {
    if (!ctx.toc_outline || ctx.toc_outline->empty()) return 0.0f;

    float entry_h = entry_font_size_ * line_spacing_ * LINE_HEIGHT_RATIO;
    // one line per entry is a lower bound; wrapping is resolved during layout.
    float h = title_font_size_ * line_spacing_ * LINE_HEIGHT_RATIO + 8.0f;
    h += static_cast<float>(ctx.toc_outline->size()) * entry_h;
    return h;
}

LayoutResult TocBlock::layout(LayoutContext& ctx) {
    entries_.clear();

    if (!ctx.toc_outline || ctx.toc_outline->empty()) {
        return {0.0f, false};
    }

    float total_height = 0.0f;
    bool page_break = false;

    Font bold_font = ctx.default_fonts->bold;
    Font regular_font = ctx.default_fonts->regular;
    if (!bold_font) bold_font = regular_font;

    // title (static text, drawn now)
    float title_h = title_font_size_ * line_spacing_ * LINE_HEIGHT_RATIO;
    if (ctx.cursor_y - title_h < ctx.page_bottom) {
        ctx.new_page();
        page_break = true;
    }

    ctx.cursor_y -= title_h;
    {
        float title_w = bold_font.text_width(title_, title_font_size_);
        float title_x = ctx.content_left + (ctx.content_width - title_w) / 2.0f;
        auto text_scope = ctx.page->begin_text();
        ctx.page->set_font(bold_font, title_font_size_);
        ctx.page->set_fill_color(Color::black());
        ctx.page->text_out(title_x, ctx.cursor_y, title_);
    }
    total_height += title_h;

    // spacing after title
    ctx.cursor_y -= 8.0f;
    total_height += 8.0f;

    // reserve rows for every (possibly wrapped) entry
    float entry_h = entry_font_size_ * line_spacing_ * LINE_HEIGHT_RATIO;
    entries_.reserve(ctx.toc_outline->size());

    for (const TocOutlineItem& item : *ctx.toc_outline) {
        Entry entry;
        entry.level = item.level;

        float indent = static_cast<float>(item.level - 1) * indent_per_level_;
        float wrap_width = ctx.content_width - indent;
        entry.lines = wrap_text(regular_font, entry_font_size_, item.text, wrap_width);

        for (size_t i = 0; i < entry.lines.size(); ++i) {
            if (ctx.cursor_y - entry_h < ctx.page_bottom) {
                ctx.new_page();
                page_break = true;
            }
            ctx.cursor_y -= entry_h;
            entry.slots.push_back(Slot{*ctx.page, ctx.cursor_y});
            total_height += entry_h;
        }

        entries_.push_back(std::move(entry));
    }

    return {total_height, page_break};
}

void TocBlock::finalize(LayoutContext& ctx) {
    if (!ctx.toc || entries_.empty()) {
        return;
    }

    Font regular_font = ctx.default_fonts->regular;
    if (!regular_font) return;

    const auto& toc_entries = ctx.toc->entries;
    size_t count = std::min(entries_.size(), toc_entries.size());

    float right_edge = ctx.content_left + ctx.content_width;
    float dot_w = regular_font.text_width(".", entry_font_size_);

    for (size_t i = 0; i < count; ++i) {
        const Entry& entry = entries_[i];
        if (entry.lines.empty()) continue;

        float indent = static_cast<float>(entry.level - 1) * indent_per_level_;
        float text_x = ctx.content_left + indent;

        std::string page_str = std::to_string(toc_entries[i].page_number);
        float page_num_w = regular_font.text_width(page_str, entry_font_size_);
        float page_x = right_edge - page_num_w;

        // draw each wrapped text line
        for (size_t j = 0; j < entry.lines.size(); ++j) {
            Page page = entry.slots[j].page;
            float sy = entry.slots[j].y;
            auto text_scope = page.begin_text();
            page.set_font(regular_font, entry_font_size_);
            page.set_fill_color(Color::black());
            page.text_out(text_x, sy, entry.lines[j]);
        }

        // dot leaders and page number sit on the last line of the entry
        Page last_page = entry.slots.back().page;
        float last_y = entry.slots.back().y;
        float last_w = regular_font.text_width(entry.lines.back(), entry_font_size_);

        {
            auto text_scope = last_page.begin_text();
            last_page.set_font(regular_font, entry_font_size_);
            last_page.set_fill_color(Color::black());

            float dots_start = text_x + last_w + dot_w;
            float dots_end = page_x - dot_w;
            float cx = dots_start;
            while (cx < dots_end) {
                last_page.text_out(cx, last_y, ".");
                cx += dot_w * 2.0f;
            }
        }

        {
            auto text_scope = last_page.begin_text();
            last_page.set_font(regular_font, entry_font_size_);
            last_page.set_fill_color(Color::black());
            last_page.text_out(page_x, last_y, page_str);
        }
    }
}

} // namespace pdfmaker
