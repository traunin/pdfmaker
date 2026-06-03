#pragma once

#include "pdfmaker/layout/content_block.hpp"
#include "pdfmaker/style.hpp"

#include <string>

namespace pdfmaker {

class CodeBlock : public ContentBlock {
public:
    CodeBlock(const std::string& code, const FontFamily* fonts, float font_size = 12.0f);

    LayoutResult layout(LayoutContext& ctx) override;
    float estimate_height(const LayoutContext& ctx) const override;

private:
    std::string code_;
    const FontFamily* fonts_;
    float font_size_;
    float padding_ = 6.0f;
    float line_spacing_ = 1.3f;
    Color bg_color_ = Color::gray(0.93f);

    std::vector<std::string> split_lines() const;
};

} // namespace pdfmaker
