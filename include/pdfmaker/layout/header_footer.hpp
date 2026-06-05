#pragma once

#include "pdfmaker/font.hpp"
#include "pdfmaker/page.hpp"
#include "pdfmaker/style.hpp"

#include <string>
#include <vector>

namespace pdfmaker {

struct HeaderFooterSpec {
    std::string left;
    std::string center;
    std::string right;
    float font_size = 12.0f;

    bool empty() const { return left.empty() && center.empty() && right.empty(); }
};

// replaces {page} / {pages} placeholders in a header or footer template.
std::string substitute_placeholders(const std::string& tmpl, int page_num, int total_pages);

// draws running headers and footers onto pages that have already been laid out
class HeaderFooterRenderer {
public:
    HeaderFooterRenderer(const PageStyle& page_style, Font font);

    // first_page_number is the number shown on the first laid-out page
    void render(std::vector<Page>& pages,
                const HeaderFooterSpec& header,
                const HeaderFooterSpec& footer,
                bool skip_first_page,
                int first_page_number = 1) const;

private:
    void render_spec(Page& page, const HeaderFooterSpec& spec, float y,
                     int page_num, int total_pages) const;

    PageStyle page_style_;
    Font font_;
};

} // namespace pdfmaker
