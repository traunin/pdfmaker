#pragma once

#include "pdfmaker/color.hpp"
#include "pdfmaker/font.hpp"

#include <hpdf.h>
#include <string>

namespace pdfmaker {

enum class TextAlignment {
    Left,
    Right,
    Center,
    Justify
};

struct TextStyle {
    const FontFamily* font_family = nullptr;
    float font_size = 14.0f;
    Color color = Color::black();
    bool bold = false;
    bool italic = false;
    bool underline = false;
    bool mono = false;

    Font resolved_font() const;
};

struct ParagraphStyle {
    TextStyle text;
    TextAlignment alignment = TextAlignment::Justify;
    float line_spacing = 1.5f;
    float space_before = 0.0f;
    float space_after = 0.0f;
    float first_line_indent = 0.0f; // in points
};

enum class PageOrientation {
    Portrait,
    Landscape
};

struct PageStyle {
    HPDF_PageSizes page_size = HPDF_PAGE_SIZE_A4;
    PageOrientation orientation = PageOrientation::Portrait;
    float margin_top = 72.0f;    // 1 inch
    float margin_bottom = 72.0f; // 1 inch
    float margin_left = 72.0f;   // 1 inch
    float margin_right = 72.0f;  // 1 inch

    float content_width(float page_width) const;
    float content_height(float page_height) const;
    float content_left() const { return margin_left; }
    float content_top(float page_height) const { return page_height - margin_top; }
    float content_bottom() const { return margin_bottom; }
};

struct TextSpan {
    std::string text;
    TextStyle style;
};

} // namespace pdfmaker
