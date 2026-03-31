#include "pdfmaker/page.hpp"

#include <string>

namespace pdfmaker {

TextScope::TextScope(HPDF_Page page) : page_(page) {
    HPDF_Page_BeginText(page_);
}

TextScope::~TextScope() {
    HPDF_Page_EndText(page_);
}

float Page::width() const {
    return HPDF_Page_GetWidth(page_);
}

float Page::height() const {
    return HPDF_Page_GetHeight(page_);
}

void Page::set_size(HPDF_PageSizes size, HPDF_PageDirection direction) {
    HPDF_Page_SetSize(page_, size, direction);
}

TextScope Page::begin_text() {
    return TextScope(page_);
}

void Page::set_font(Font font, float size) {
    HPDF_Page_SetFontAndSize(page_, font.raw(), size);
}

void Page::text_out(float x, float y, std::string_view text) {
    std::string buf(text);
    HPDF_Page_TextOut(page_, x, y, buf.c_str());
}

void Page::move_text_pos(float x, float y) {
    HPDF_Page_MoveTextPos(page_, x, y);
}

void Page::show_text(std::string_view text) {
    std::string buf(text);
    HPDF_Page_ShowText(page_, buf.c_str());
}

void Page::set_line_width(float width) {
    HPDF_Page_SetLineWidth(page_, width);
}

void Page::set_stroke_color(Color c) {
    HPDF_Page_SetRGBStroke(page_, c.r, c.g, c.b);
}

void Page::set_fill_color(Color c) {
    HPDF_Page_SetRGBFill(page_, c.r, c.g, c.b);
}

void Page::rectangle(float x, float y, float width, float height) {
    HPDF_Page_Rectangle(page_, x, y, width, height);
}

void Page::line(float x1, float y1, float x2, float y2) {
    HPDF_Page_MoveTo(page_, x1, y1);
    HPDF_Page_LineTo(page_, x2, y2);
}

void Page::move_to(float x, float y) {
    HPDF_Page_MoveTo(page_, x, y);
}

void Page::line_to(float x, float y) {
    HPDF_Page_LineTo(page_, x, y);
}

void Page::stroke() {
    HPDF_Page_Stroke(page_);
}

void Page::fill() {
    HPDF_Page_Fill(page_);
}

void Page::fill_stroke() {
    HPDF_Page_FillStroke(page_);
}

void Page::draw_image(HPDF_Image image, float x, float y, float w, float h) {
    HPDF_Page_DrawImage(page_, image, x, y, w, h);
}

void Page::set_text_leading(float leading) {
    HPDF_Page_SetTextLeading(page_, leading);
}

} // namespace pdfmaker
