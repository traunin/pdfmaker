#pragma once

#include "pdfmaker/detail/hpdf_guard.hpp"
#include "pdfmaker/font.hpp"
#include "pdfmaker/page.hpp"
#include "pdfmaker/style.hpp"

#include <string>
#include <filesystem>

namespace pdfmaker {

class Document {
public:
    Document();
    ~Document();

    Document(Document&&) noexcept;
    Document& operator=(Document&&) noexcept;

    Document(const Document&) = delete;
    Document& operator=(const Document&) = delete;

    void set_title(const std::string& title);
    void set_author(const std::string& author);
    void set_subject(const std::string& subject);

    Font get_font(const std::string& name, const std::string& encoding = "");
    std::string load_ttf_font(const std::filesystem::path& path, bool embed = true);

    FontFamily make_font_family(const std::string& regular,
                                const std::string& bold = "",
                                const std::string& italic = "",
                                const std::string& bold_italic = "",
                                const std::string& mono = "",
                                const std::string& encoding = "");

    void use_utf8();

    Page add_page();
    Page add_page(const PageStyle& style);
    int page_count() const;

    void save(const std::filesystem::path& path) const;

    HPDF_Doc raw() const noexcept;

private:
    detail::HpdfGuard guard_;
    bool utf8_enabled_ = false;
    int page_count_ = 0;
};

} // namespace pdfmaker
