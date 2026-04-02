#include "pdfmaker/document.hpp"
#include "pdfmaker/error.hpp"

namespace pdfmaker {

Document::Document() = default;
Document::~Document() = default;
Document::Document(Document&&) noexcept = default;
Document& Document::operator=(Document&&) noexcept = default;

void Document::set_title(const std::string& title) {
    HPDF_SetInfoAttr(guard_.get(), HPDF_INFO_TITLE, title.c_str());
}

void Document::set_author(const std::string& author) {
    HPDF_SetInfoAttr(guard_.get(), HPDF_INFO_AUTHOR, author.c_str());
}

void Document::set_subject(const std::string& subject) {
    HPDF_SetInfoAttr(guard_.get(), HPDF_INFO_SUBJECT, subject.c_str());
}

Font Document::get_font(const std::string& name, const std::string& encoding) {
    const char* enc_ptr = encoding.empty() ? nullptr : encoding.c_str();
    HPDF_Font font = HPDF_GetFont(guard_.get(), name.c_str(), enc_ptr);
    return Font(font);
}

std::string Document::load_ttf_font(const std::filesystem::path& path, bool embed) {
    std::string path_str = path.string();
    const char* font_name = HPDF_LoadTTFontFromFile(
        guard_.get(), path_str.c_str(), embed ? HPDF_TRUE : HPDF_FALSE);
    return std::string(font_name);
}

FontFamily Document::make_font_family(const std::string& regular,
                                      const std::string& bold,
                                      const std::string& italic,
                                      const std::string& bold_italic,
                                      const std::string& mono,
                                      const std::string& encoding) {
    FontFamily family;
    family.regular = get_font(regular, encoding);
    family.bold = bold.empty() ? family.regular : get_font(bold, encoding);
    family.italic = italic.empty() ? family.regular : get_font(italic, encoding);
    family.bold_italic = bold_italic.empty() ? family.regular : get_font(bold_italic, encoding);
    family.mono = mono.empty() ? family.regular : get_font(mono, encoding);
    return family;
}

void Document::use_utf8() {
    if (!utf8_enabled_) {
        HPDF_UseUTFEncodings(guard_.get());
        utf8_enabled_ = true;
    }
}

Page Document::add_page() {
    HPDF_Page page = HPDF_AddPage(guard_.get());
    page_count_++;
    return Page(page);
}

Page Document::add_page(const PageStyle& style) {
    Page page = add_page();
    HPDF_PageDirection dir = (style.orientation == PageOrientation::Portrait)
        ? HPDF_PAGE_PORTRAIT : HPDF_PAGE_LANDSCAPE;
    page.set_size(style.page_size, dir);
    return page;
}

int Document::page_count() const {
    return page_count_;
}

void Document::save(const std::filesystem::path& path) const {
    std::string path_str = path.string();
    HPDF_SaveToFile(guard_.get(), path_str.c_str());
}

HPDF_Doc Document::raw() const noexcept {
    return guard_.get();
}

} // namespace pdfmaker