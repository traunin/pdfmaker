#pragma once

#include "pdfmaker/detail/hpdf_guard.hpp"
#include "pdfmaker/font.hpp"
#include "pdfmaker/image.hpp"
#include "pdfmaker/page.hpp"
#include "pdfmaker/style.hpp"

#include <filesystem>
#include <string>
#include <string_view>
#include <vector>

namespace pdfmaker {

class Document {
public:
    Document();
    ~Document();

    Document(Document&&) noexcept;
    Document& operator=(Document&&) noexcept;

    Document(const Document&) = delete;
    Document& operator=(const Document&) = delete;

    // Metadata
    void set_title(std::string_view title);
    void set_author(std::string_view author);
    void set_subject(std::string_view subject);

    Font get_font(std::string_view name, std::string_view encoding = "");
    std::string load_ttf_font(const std::filesystem::path& path, bool embed = true);

    FontFamily make_font_family(std::string_view regular,
                                std::string_view bold = "",
                                std::string_view italic = "",
                                std::string_view bold_italic = "",
                                std::string_view mono = "",
                                std::string_view encoding = "");

    void use_utf8_encoding();

    Page add_page();
    Page add_page(const PageStyle& style);
    int page_count() const;

    Image load_png(const std::filesystem::path& path);
    Image load_jpeg(const std::filesystem::path& path);

    void save(const std::filesystem::path& path);

    // throws PdfError if libharu has recorded an error
    void check() const;

    HPDF_Doc raw() const noexcept;

private:
    detail::HpdfGuard guard_;
    bool utf8_enabled_ = false;
    int page_count_ = 0;
};

} // namespace pdfmaker
