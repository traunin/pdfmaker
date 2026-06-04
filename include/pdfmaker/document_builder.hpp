#pragma once

#include "pdfmaker/document.hpp"
#include "pdfmaker/layout/layout_engine.hpp"
#include "pdfmaker/layout/content_block.hpp"
#include "pdfmaker/layout/text_block.hpp"
#include "pdfmaker/layout/heading_block.hpp"
#include "pdfmaker/layout/code_block.hpp"
#include "pdfmaker/layout/list_block.hpp"
#include "pdfmaker/layout/table_block.hpp"
#include "pdfmaker/layout/image_block.hpp"
#include "pdfmaker/layout/toc_block.hpp"
#include "pdfmaker/layout/page_break.hpp"
#include "pdfmaker/table.hpp"

#include <filesystem>
#include <map>
#include <memory>
#include <string>
#include <vector>

namespace pdfmaker {

class DocumentBuilder {
public:
    DocumentBuilder();

    // document metadata
    DocumentBuilder& title(const std::string& title);
    DocumentBuilder& author(const std::string& author);
    DocumentBuilder& subject(const std::string& subject);

    // page and font setup
    DocumentBuilder& page_style(PageStyle style);
    DocumentBuilder& default_fonts(const std::string& regular, const std::string& bold,
                                    const std::string& italic, const std::string& bold_italic,
                                    const std::string& mono);
    DocumentBuilder& load_ttf_fonts(const std::filesystem::path& regular,
                                     const std::filesystem::path& bold,
                                     const std::filesystem::path& italic,
                                     const std::filesystem::path& bold_italic,
                                     const std::filesystem::path& mono);

    // style configuration
    DocumentBuilder& body_style(ParagraphStyle style);
    DocumentBuilder& heading_style(int level, ParagraphStyle style);

    // headers/footers
    DocumentBuilder& header(const std::string& left, const std::string& center, const std::string& right);
    DocumentBuilder& footer(const std::string& left, const std::string& center, const std::string& right);
    // suppress header/footer on the first page (e.g. a GOST title page).
    DocumentBuilder& skip_first_page_decorations(bool skip = true);

    // content blocks
    DocumentBuilder& add_toc(const std::string& title = "Contents");
    DocumentBuilder& add_heading(int level, const std::string& text);
    DocumentBuilder& add_paragraph(const std::string& text);
    DocumentBuilder& add_paragraph(const std::string& text, ParagraphStyle style);
    DocumentBuilder& add_code(const std::string& code, float font_size = 12.0f);
    DocumentBuilder& add_list(ListType type, const std::vector<std::string>& items);
    DocumentBuilder& add_table(TableSpec spec, float font_size = 12.0f);
    DocumentBuilder& add_image(const std::filesystem::path& path, const std::string& caption = "",
                                float max_width_fraction = 1.0f);
    DocumentBuilder& add_page_break();

    // build and save
    void build(const std::filesystem::path& output_path);

    // access internals for advanced use
    Document& document() { return doc_; }
    const TocCollector& toc() const { return engine_.toc(); }

private:
    Document doc_;
    LayoutEngine engine_;
    FontFamily fonts_{};
    bool fonts_set_ = false;
    std::string title_;
    std::string author_;
    std::string subject_;
    std::vector<std::unique_ptr<ContentBlock>> blocks_;

    ParagraphStyle body_style_{};
    bool body_style_set_ = false;
    std::map<int, ParagraphStyle> heading_styles_;

    ParagraphStyle get_body_style() const;
    ParagraphStyle get_heading_style(int level) const;
    void ensure_fonts();
};

} // namespace pdfmaker
