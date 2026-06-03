#include "pdfmaker/layout/table_block.hpp"
#include "pdfmaker/document.hpp"

namespace pdfmaker {

TableBlock::TableBlock(TableSpec spec, const FontFamily* fonts, float font_size)
    : spec_(std::move(spec))
    , fonts_(fonts)
    , font_size_(font_size)
{
}

std::vector<float> TableBlock::compute_column_widths(float total_width) const {
    size_t n = spec_.columns.size();
    std::vector<float> widths(n, 0.0f);

    float remaining = total_width;
    int auto_count = 0;

    for (size_t i = 0; i < n; ++i) {
        if (spec_.columns[i].width_fraction > 0.0f) {
            widths[i] = total_width * spec_.columns[i].width_fraction;
            remaining -= widths[i];
        } else {
            ++auto_count;
        }
    }

    if (auto_count > 0) {
        float auto_width = remaining / static_cast<float>(auto_count);
        for (size_t i = 0; i < n; ++i) {
            if (spec_.columns[i].width_fraction <= 0.0f) {
                widths[i] = auto_width;
            }
        }
    }

    return widths;
}

float TableBlock::compute_row_height(const std::vector<TableCell>& cells,
                                      const std::vector<float>& col_widths) const {
    Font font = fonts_->regular;
    float max_height = font_size_ + cell_padding_ * 2.0f;

    for (size_t i = 0; i < cells.size() && i < col_widths.size(); ++i) {
        float avail = col_widths[i] - cell_padding_ * 2.0f;
        float text_w = font.text_width(cells[i].text, font_size_);
        int lines = static_cast<int>(text_w / avail) + 1;
        float h = static_cast<float>(lines) * font_size_ * 1.2f + cell_padding_ * 2.0f;
        if (h > max_height) max_height = h;
    }

    return max_height;
}

void TableBlock::render_row(LayoutContext& ctx, const std::vector<TableCell>& cells,
                             const std::vector<float>& col_widths, float x, float y,
                             float row_height, bool is_header) const {
    Font font = is_header ? fonts_->bold : fonts_->regular;
    if (!font) font = fonts_->regular;

    float cx = x;
    for (size_t i = 0; i < cells.size() && i < col_widths.size(); ++i) {
        // Draw cell border
        ctx.page->set_line_width(line_width_);
        ctx.page->set_stroke_color(Color::black());
        ctx.page->rectangle(cx, y - row_height, col_widths[i], row_height);
        ctx.page->stroke();

        // Draw text
        float text_x = cx + cell_padding_;
        float text_y = y - cell_padding_ - font_size_;

        // Alignment within cell
        float avail = col_widths[i] - cell_padding_ * 2.0f;
        float text_w = font.text_width(cells[i].text, font_size_);

        TextAlignment align = cells[i].alignment;
        if (align == TextAlignment::Center) {
            text_x = cx + (col_widths[i] - text_w) / 2.0f;
        } else if (align == TextAlignment::Right) {
            text_x = cx + col_widths[i] - cell_padding_ - text_w;
        }

        {
            auto text_scope = ctx.page->begin_text();
            ctx.page->set_font(font, font_size_);
            ctx.page->set_fill_color(Color::black());
            // Truncate text to fit (simple approach)
            if (text_w <= avail) {
                ctx.page->text_out(text_x, text_y, cells[i].text);
            } else {
                // Simple truncation with ellipsis
                std::string truncated = cells[i].text;
                while (truncated.size() > 3 &&
                       font.text_width(truncated + "...", font_size_) > avail) {
                    truncated.pop_back();
                }
                ctx.page->text_out(text_x, text_y, truncated + "...");
            }
        }

        cx += col_widths[i];
    }
}

LayoutResult TableBlock::layout(LayoutContext& ctx) {
    float total_height = 0.0f;
    bool page_break = false;
    float space_before = 6.0f;
    float space_after = 6.0f;

    ctx.cursor_y -= space_before;
    total_height += space_before;

    auto col_widths = compute_column_widths(ctx.content_width);

    // header row
    std::vector<TableCell> header_cells;
    for (const auto& col : spec_.columns) {
        header_cells.push_back({col.header, col.alignment});
    }
    float header_height = compute_row_height(header_cells, col_widths);

    auto render_header = [&]() {
        render_row(ctx, header_cells, col_widths, ctx.content_left, ctx.cursor_y, header_height, true);
        ctx.cursor_y -= header_height;
        total_height += header_height;
    };

    // check if header fits
    if (ctx.cursor_y - header_height < ctx.page_bottom) {
        ctx.new_page();
        page_break = true;
    }
    render_header();

    // data rows
    for (const auto& row : spec_.rows) {
        float row_h = compute_row_height(row, col_widths);

        if (ctx.cursor_y - row_h < ctx.page_bottom) {
            ctx.new_page();
            page_break = true;
            // re-render header on new page
            render_header();
        }

        render_row(ctx, row, col_widths, ctx.content_left, ctx.cursor_y, row_h, false);
        ctx.cursor_y -= row_h;
        total_height += row_h;
    }

    ctx.cursor_y -= space_after;
    total_height += space_after;

    return {total_height, page_break};
}

float TableBlock::estimate_height(const LayoutContext& ctx) const {
    auto col_widths = compute_column_widths(ctx.content_width);
    float h = 12.0f; // space before + after

    // header
    std::vector<TableCell> header_cells;
    for (const auto& col : spec_.columns) {
        header_cells.push_back({col.header, col.alignment});
    }
    h += compute_row_height(header_cells, col_widths);

    // rows
    for (const auto& row : spec_.rows) {
        h += compute_row_height(row, col_widths);
    }

    return h;
}

} // namespace pdfmaker
