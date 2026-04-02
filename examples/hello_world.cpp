#include <pdfmaker/pdfmaker.hpp>
#include <iostream>

int main() {
    try {
        pdfmaker::Document doc;
        doc.set_title("Hello World");
        doc.set_author("pdf_maker");

        pdfmaker::PageStyle page_style;
        page_style.margin_top = 56.7f;     // 20mm
        page_style.margin_bottom = 56.7f;  // 20mm
        page_style.margin_left = 85.0f;    // 30mm
        page_style.margin_right = 42.5f;   // 15mm

        auto page = doc.add_page(page_style);

        auto font = doc.get_font("Times-Roman");
        auto bold_font = doc.get_font("Times-Bold");

        // Background rectangle behind the title
        auto title_y = page.height() - page_style.margin_top;
        auto content_w = page.width() - page_style.margin_left - page_style.margin_right;
        page.set_fill_color(pdfmaker::Color::from_hex("#dce6f0"));
        page.rectangle(page_style.margin_left,
                       title_y - 24.0f, content_w, 24.0f);
        page.fill();

        // Title (dark blue)
        {
            auto text = page.begin_text();
            page.set_fill_color(pdfmaker::Color::from_hex("#1a3c6e"));
            page.set_font(bold_font, 16.0f);
            page.text_out(page_style.margin_left + 4.0f, title_y - 18.0f,
                          "Hello, World! From pdfmaker.");
        }

        // Line under the title
        page.set_line_width(1.5f);
        page.set_stroke_color(pdfmaker::Color::from_hex("#0000ff"));
        page.line(page_style.margin_left, title_y - 28.0f,
                  page.width() - page_style.margin_right, title_y - 28.0f);
        page.stroke();

        // Body text (dark gray)
        {
            auto text = page.begin_text();
            page.set_fill_color(pdfmaker::Color::rgb(0.25f, 0.25f, 0.25f));
            page.set_font(font, 14.0f);
            page.text_out(page_style.margin_left, title_y - 50.0f,
                          "This is a test document created with the pdfmaker library.");
            page.text_out(page_style.margin_left, title_y - 70.0f,
                          "It wraps libharu with a modern C++17 API.");
        }

        doc.save("hello_world.pdf");
        std::cout << "Created hello_world.pdf\n";
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }

    return 0;
}
