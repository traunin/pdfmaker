#include "pdfmaker/document.hpp"
#include "pdfmaker/error.hpp"

#include <string>

namespace pdfmaker {

Document::Document() = default;
Document::~Document() = default;
Document::Document(Document&&) noexcept = default;
Document& Document::operator=(Document&&) noexcept = default;

void Document::set_title(std::string_view title) {
    std::string buf(title);
    HPDF_SetInfoAttr(guard_.get(), HPDF_INFO_TITLE, buf.c_str());
}

void Document::set_author(std::string_view author) {
    std::string buf(author);
    HPDF_SetInfoAttr(guard_.get(), HPDF_INFO_AUTHOR, buf.c_str());
}

void Document::set_subject(std::string_view subject) {
    std::string buf(subject);
    HPDF_SetInfoAttr(guard_.get(), HPDF_INFO_SUBJECT, buf.c_str());
}

Font Document::get_font(std::string_view name, std::string_view encoding) {
    std::string name_buf(name);
    std::string enc_buf(encoding);
    const char* enc_ptr = enc_buf.empty() ? nullptr : enc_buf.c_str();
    HPDF_Font font = HPDF_GetFont(guard_.get(), name_buf.c_str(), enc_ptr);
    return Font(font);
}

std::string Document::load_ttf_font(const std::filesystem::path& path, bool embed) {
    std::string path_str = path.string();
    const char* font_name = HPDF_LoadTTFontFromFile(
        guard_.get(), path_str.c_str(), embed ? HPDF_TRUE : HPDF_FALSE);
    return std::string(font_name);
}

FontFamily Document::make_font_family(std::string_view regular,
                                      std::string_view bold,
                                      std::string_view italic,
                                      std::string_view bold_italic,
                                      std::string_view mono,
                                      std::string_view encoding) {
    FontFamily family;
    family.regular = get_font(regular, encoding);
    family.bold = get_font(bold, encoding);
    family.italic = get_font(italic, encoding);
    family.bold_italic = get_font(bold_italic, encoding);
    family.mono = get_font(mono, encoding);
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

void Document::save(const std::filesystem::path& path) const {
    std::string path_str = path.string();
    HPDF_SaveToFile(guard_.get(), path_str.c_str());
}

HPDF_Doc Document::raw() const noexcept {
    return guard_.get();
}

} // namespace pdfmaker