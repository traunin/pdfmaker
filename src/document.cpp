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
    guard_.check();
}

void Document::set_author(std::string_view author) {
    std::string buf(author);
    HPDF_SetInfoAttr(guard_.get(), HPDF_INFO_AUTHOR, buf.c_str());
    guard_.check();
}

void Document::set_subject(std::string_view subject) {
    std::string buf(subject);
    HPDF_SetInfoAttr(guard_.get(), HPDF_INFO_SUBJECT, buf.c_str());
    guard_.check();
}

Font Document::get_font(std::string_view name, std::string_view encoding) {
    std::string name_buf(name);
    std::string enc_buf(encoding);
    const char* enc_ptr = enc_buf.empty() ? nullptr : enc_buf.c_str();
    HPDF_Font font = HPDF_GetFont(guard_.get(), name_buf.c_str(), enc_ptr);
    guard_.check();
    return Font(font);
}

std::string Document::load_ttf_font(const std::filesystem::path& path, bool embed) {
    std::string path_str = path.string();
    const char* font_name = HPDF_LoadTTFontFromFile(
        guard_.get(), path_str.c_str(), embed ? HPDF_TRUE : HPDF_FALSE);
    guard_.check();
    return std::string(font_name ? font_name : "");
}

FontFamily Document::make_font_family(std::string_view regular,
                                       std::string_view bold,
                                       std::string_view italic,
                                       std::string_view bold_italic,
                                       std::string_view mono,
                                       std::string_view encoding) {
    FontFamily family;
    family.regular = get_font(regular, encoding);
    family.bold = bold.empty() ? family.regular : get_font(bold, encoding);
    family.italic = italic.empty() ? family.regular : get_font(italic, encoding);
    family.bold_italic = bold_italic.empty() ? family.regular : get_font(bold_italic, encoding);
    family.mono = mono.empty() ? family.regular : get_font(mono, encoding);
    return family;
}

void Document::use_utf8_encoding() {
    if (!utf8_enabled_) {
        HPDF_UseUTFEncodings(guard_.get());
        guard_.check();
        utf8_enabled_ = true;
    }
}

Page Document::add_page() {
    HPDF_Page page = HPDF_AddPage(guard_.get());
    guard_.check();
    ++page_count_;
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

Image Document::load_png(const std::filesystem::path& path) {
    std::string path_str = path.string();
    HPDF_Image img = HPDF_LoadPngImageFromFile(guard_.get(), path_str.c_str());
    guard_.check();
    return Image(img);
}

Image Document::load_jpeg(const std::filesystem::path& path) {
    std::string path_str = path.string();
    HPDF_Image img = HPDF_LoadJpegImageFromFile(guard_.get(), path_str.c_str());
    guard_.check();
    return Image(img);
}

void Document::save(const std::filesystem::path& path) {
    std::string path_str = path.string();
    HPDF_SaveToFile(guard_.get(), path_str.c_str());
    guard_.check();
}

void Document::check() const {
    guard_.check();
}

HPDF_Doc Document::raw() const noexcept {
    return guard_.get();
}

} // namespace pdfmaker
