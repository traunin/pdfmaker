#include "pdfmaker/layout/header_footer.hpp"
#include "pdfmaker/color.hpp"

namespace pdfmaker {

std::string substitute_placeholders(const std::string& tmpl, int page_num, int total_pages) {
    std::string result = tmpl;
    std::string page_str = std::to_string(page_num);
    std::string total_str = std::to_string(total_pages);

    size_t pos;
    while ((pos = result.find("{page}")) != std::string::npos) {
        result.replace(pos, 6, page_str);
    }
    while ((pos = result.find("{pages}")) != std::string::npos) {
        result.replace(pos, 7, total_str);
    }
    return result;
}

HeaderFooterRenderer::HeaderFooterRenderer(const PageStyle& page_style, Font font)
    : page_style_(page_style)
    , font_(font)
{
}

void HeaderFooterRenderer::render_spec(Page& page, const HeaderFooterSpec& spec, float y,
                                       int page_num, int total_pages) const {
    if (spec.empty()) return;

    float page_w = page.width();

    auto text_scope = page.begin_text();
    page.set_font(font_, spec.font_size);
    page.set_fill_color(Color::black());

    if (!spec.left.empty()) {
        std::string text = substitute_placeholders(spec.left, page_num, total_pages);
        page.text_out(page_style_.margin_left, y, text);
    }
    if (!spec.center.empty()) {
        std::string text = substitute_placeholders(spec.center, page_num, total_pages);
        float w = font_.text_width(text, spec.font_size);
        float x = (page_w - w) / 2.0f;
        page.text_out(x, y, text);
    }
    if (!spec.right.empty()) {
        std::string text = substitute_placeholders(spec.right, page_num, total_pages);
        float w = font_.text_width(text, spec.font_size);
        float x = page_w - page_style_.margin_right - w;
        page.text_out(x, y, text);
    }
}

void HeaderFooterRenderer::render(std::vector<Page>& pages,
                                  const HeaderFooterSpec& header,
                                  const HeaderFooterSpec& footer,
                                  bool skip_first_page,
                                  int first_page_number) const {
    if (!font_) return;

    int total_pages = static_cast<int>(pages.size());

    for (int i = 0; i < total_pages; ++i) {
        if (skip_first_page && i == 0) continue;

        Page& page = pages[i];
        int page_num = first_page_number + i;
        float page_h = page.height();

        float header_y = page_h - page_style_.margin_top / 2.0f;
        render_spec(page, header, header_y, page_num, total_pages);

        float footer_y = page_style_.margin_bottom / 2.0f;
        render_spec(page, footer, footer_y, page_num, total_pages);
    }
}

} // namespace pdfmaker
