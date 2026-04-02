#pragma once

#include "pdfmaker/color.hpp"
#include "pdfmaker/font.hpp"
#include "pdfmaker/style.hpp"

#include <hpdf.h>
#include <string>

namespace pdfmaker {

class TextScope {
public:
    explicit TextScope(HPDF_Page page);
    ~TextScope();

    TextScope(const TextScope&) = delete;
    TextScope& operator=(const TextScope&) = delete;

    TextScope(TextScope&&) noexcept;
    TextScope& operator=(TextScope&&) = delete;

private:
    HPDF_Page page_;
};

// non-owning handle to a page in a Document
// must not outlive the parent Document
class Page {
public:
    Page() = default;
    explicit Page(HPDF_Page handle) : handle_(handle) {}

    float width() const;
    float height() const;
    void set_size(HPDF_PageSizes size, HPDF_PageDirection direction);

    [[nodiscard]] TextScope begin_text();
    void set_font(Font font, float size);
    void text_out(float x, float y, const std::string& text);
    void move_text_pos(float x, float y);
    void show_text(const std::string& text);

    void set_line_width(float width);
    void set_stroke_color(Color c);
    void set_fill_color(Color c);

    void rectangle(float x, float y, float width, float height);
    void line(float x1, float y1, float x2, float y2);
    void move_to(float x, float y);
    void line_to(float x, float y);
    void stroke();
    void fill();
    void fill_stroke();

    void draw_image(HPDF_Image image, float x, float y, float w, float h);

    void set_text_leading(float leading);

    HPDF_Page raw() const noexcept { return handle_; }
    explicit operator bool() const noexcept { return handle_ != nullptr; }

private:
    HPDF_Page handle_ = nullptr;
};

} // namespace pdfmaker
