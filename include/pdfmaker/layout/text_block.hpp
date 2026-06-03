#pragma once

#include "pdfmaker/layout/content_block.hpp"
#include "pdfmaker/style.hpp"

#include <string>
#include <vector>

namespace pdfmaker {

class TextBlock : public ContentBlock {
public:
    explicit TextBlock(ParagraphStyle style);
    TextBlock(const std::string& text, ParagraphStyle style);

    void add_run(const std::string& text, TextStyle style);
    void add_run(TextSpan run);

    const std::vector<TextSpan>& runs() const { return runs_; }
    const ParagraphStyle& paragraph_style() const { return para_style_; }

    LayoutResult layout(LayoutContext& ctx) override;
    float estimate_height(const LayoutContext& ctx) const override;

protected:
    struct Word {
        std::string text;
        TextStyle style;
        float width;
        float space_width;
    };

    struct Line {
        std::vector<Word> words;
        float total_width;
        float line_height;
    };

    std::vector<Line> break_into_lines(float max_width) const;
    void render_line(const Line& line, LayoutContext& ctx, float x, float y, float max_width,
                     bool is_last_line) const;

    ParagraphStyle para_style_;
    std::vector<TextSpan> runs_;
};

} // namespace pdfmaker
