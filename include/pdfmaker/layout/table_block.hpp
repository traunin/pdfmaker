#pragma once

#include "pdfmaker/layout/content_block.hpp"
#include "pdfmaker/table.hpp"

namespace pdfmaker {

class TableBlock : public ContentBlock {
public:
    TableBlock(TableSpec spec, const FontFamily* fonts, float font_size = 12.0f);

    LayoutResult layout(LayoutContext& ctx) override;
    float estimate_height(const LayoutContext& ctx) const override;

private:
    TableSpec spec_;
    const FontFamily* fonts_;
    float font_size_;
    float cell_padding_ = 4.0f;
    float line_width_ = 0.5f;

    std::vector<float> compute_column_widths(float total_width) const;
    float compute_row_height(const std::vector<TableCell>& cells, const std::vector<float>& col_widths) const;

    void render_row(LayoutContext& ctx, const std::vector<TableCell>& cells,
                    const std::vector<float>& col_widths, float x, float y,
                    float row_height, bool is_header) const;
};

} // namespace pdfmaker
