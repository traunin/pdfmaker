#pragma once

#include "pdfmaker/layout/content_block.hpp"
#include "pdfmaker/layout/header_footer.hpp"
#include "pdfmaker/document.hpp"
#include "pdfmaker/style.hpp"

#include <memory>
#include <string>
#include <vector>

namespace pdfmaker {

class LayoutEngine {
public:
    void set_page_style(PageStyle style);
    void set_default_fonts(const FontFamily* family);
    void set_header(const std::string& left, const std::string& center, const std::string& right);
    void set_footer(const std::string& left, const std::string& center, const std::string& right);
    void set_skip_first_page_decorations(bool skip);
    void set_first_page_number(int n);

    void render(Document& doc, std::vector<std::unique_ptr<ContentBlock>>& blocks);

    const TocCollector& toc() const { return toc_; }

private:
    PageStyle page_style_{};
    const FontFamily* fonts_ = nullptr; // owned by the caller (DocumentBuilder)
    HeaderFooterSpec header_;
    HeaderFooterSpec footer_;
    bool skip_first_page_decorations_ = false;
    int first_page_number_ = 1;
    TocCollector toc_;

    std::vector<Page> pages_;

    Page create_page(Document& doc);
};

} // namespace pdfmaker
