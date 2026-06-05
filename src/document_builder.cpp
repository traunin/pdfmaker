#include "pdfmaker/document_builder.hpp"

#include <algorithm>
#include <cctype>
#include <map>
#include <stdexcept>

namespace pdfmaker {

DocumentBuilder::DocumentBuilder() = default;

DocumentBuilder& DocumentBuilder::title(const std::string& title) {
    title_ = title;
    return *this;
}

DocumentBuilder& DocumentBuilder::author(const std::string& author) {
    author_ = author;
    return *this;
}

DocumentBuilder& DocumentBuilder::subject(const std::string& subject) {
    subject_ = subject;
    return *this;
}

DocumentBuilder& DocumentBuilder::page_style(PageStyle style) {
    engine_.set_page_style(style);
    return *this;
}

DocumentBuilder& DocumentBuilder::default_fonts(const std::string& regular, const std::string& bold,
                                                  const std::string& italic, const std::string& bold_italic,
                                                  const std::string& mono) {
    fonts_ = doc_.make_font_family(regular, bold, italic, bold_italic, mono);
    fonts_set_ = true;
    engine_.set_default_fonts(&fonts_);
    return *this;
}

DocumentBuilder& DocumentBuilder::load_ttf_fonts(const std::filesystem::path& regular,
                                                   const std::filesystem::path& bold,
                                                   const std::filesystem::path& italic,
                                                   const std::filesystem::path& bold_italic,
                                                   const std::filesystem::path& mono) {
    doc_.use_utf8_encoding();
    std::string reg_name = doc_.load_ttf_font(regular);
    std::string bold_name = doc_.load_ttf_font(bold);
    std::string ital_name = doc_.load_ttf_font(italic);
    std::string bi_name = doc_.load_ttf_font(bold_italic);
    std::string mono_name = doc_.load_ttf_font(mono);

    fonts_ = doc_.make_font_family(reg_name, bold_name, ital_name, bi_name, mono_name, "UTF-8");
    fonts_set_ = true;
    engine_.set_default_fonts(&fonts_);
    return *this;
}

DocumentBuilder& DocumentBuilder::body_style(ParagraphStyle style) {
    body_style_ = std::move(style);
    body_style_set_ = true;
    return *this;
}

DocumentBuilder& DocumentBuilder::heading_style(int level, ParagraphStyle style) {
    heading_styles_[level] = std::move(style);
    return *this;
}

DocumentBuilder& DocumentBuilder::header(const std::string& left, const std::string& center, const std::string& right) {
    engine_.set_header(left, center, right);
    return *this;
}

DocumentBuilder& DocumentBuilder::footer(const std::string& left, const std::string& center, const std::string& right) {
    engine_.set_footer(left, center, right);
    return *this;
}

DocumentBuilder& DocumentBuilder::skip_first_page_decorations(bool skip) {
    engine_.set_skip_first_page_decorations(skip);
    return *this;
}

DocumentBuilder& DocumentBuilder::first_page_number(int n) {
    engine_.set_first_page_number(n);
    return *this;
}

DocumentBuilder& DocumentBuilder::add_toc(const std::string& title_text) {
    blocks_.push_back(std::make_unique<TocBlock>(title_text));
    return *this;
}

DocumentBuilder& DocumentBuilder::add_heading(int level, const std::string& text) {
    ensure_fonts();
    blocks_.push_back(std::make_unique<HeadingBlock>(level, text, get_heading_style(level)));
    return *this;
}

DocumentBuilder& DocumentBuilder::add_heading(int level, const std::string& text,
                                              ParagraphStyle style) {
    ensure_fonts();
    // supply the loaded font family when the caller's style does not carry one.
    if (!style.text.font_family) {
        style.text.font_family = &fonts_;
    }
    blocks_.push_back(std::make_unique<HeadingBlock>(level, text, std::move(style)));
    return *this;
}

DocumentBuilder& DocumentBuilder::add_paragraph(const std::string& text) {
    blocks_.push_back(std::make_unique<TextBlock>(text, get_body_style()));
    return *this;
}

DocumentBuilder& DocumentBuilder::add_paragraph(const std::string& text, ParagraphStyle style) {
    blocks_.push_back(std::make_unique<TextBlock>(text, std::move(style)));
    return *this;
}

DocumentBuilder& DocumentBuilder::add_code(const std::string& code, float font_size) {
    ensure_fonts();
    blocks_.push_back(std::make_unique<CodeBlock>(code, &fonts_, font_size));
    return *this;
}

DocumentBuilder& DocumentBuilder::add_list(ListType type, const std::vector<std::string>& items) {
    ensure_fonts();
    auto list = std::make_unique<ListBlock>(type, &fonts_);
    for (const auto& item : items) {
        list->add_item(item);
    }
    blocks_.push_back(std::move(list));
    return *this;
}

DocumentBuilder& DocumentBuilder::add_table(TableSpec spec, float font_size) {
    ensure_fonts();
    blocks_.push_back(std::make_unique<TableBlock>(std::move(spec), &fonts_, font_size));
    return *this;
}

DocumentBuilder& DocumentBuilder::add_image(const std::filesystem::path& path,
                                              const std::string& caption,
                                              float max_width_fraction) {
    std::string ext = path.extension().string();
    std::transform(ext.begin(), ext.end(), ext.begin(),
                   [](unsigned char c) { return static_cast<char>(std::tolower(c)); });

    Image img;
    if (ext == ".png") {
        img = doc_.load_png(path);
    } else if (ext == ".jpg" || ext == ".jpeg") {
        img = doc_.load_jpeg(path);
    } else {
        throw std::invalid_argument("add_image: unsupported image extension '" + ext +
                                    "' (expected .png, .jpg, or .jpeg)");
    }
    blocks_.push_back(std::make_unique<ImageBlock>(img, max_width_fraction, caption));
    return *this;
}

DocumentBuilder& DocumentBuilder::add_page_break() {
    blocks_.push_back(std::make_unique<PageBreakBlock>());
    return *this;
}

void DocumentBuilder::build(const std::filesystem::path& output_path) {
    ensure_fonts();

    if (!title_.empty()) doc_.set_title(title_);
    if (!author_.empty()) doc_.set_author(author_);
    if (!subject_.empty()) doc_.set_subject(subject_);

    engine_.render(doc_, blocks_);
    doc_.save(output_path);
}

ParagraphStyle DocumentBuilder::get_body_style() const {
    if (body_style_set_) {
        // a caller may set size/spacing without knowing the loaded fonts; supply
        // the builder's font family when the style does not carry one.
        ParagraphStyle style = body_style_;
        if (!style.text.font_family) {
            style.text.font_family = &fonts_;
        }
        return style;
    }
    // defaults
    ParagraphStyle style;
    style.text.font_family = &fonts_;
    style.text.font_size = 12.0f;
    style.text.color = Color::black();
    style.alignment = TextAlignment::Left;
    style.line_spacing = 1.0f;
    style.space_after = 4.0f;
    return style;
}

ParagraphStyle DocumentBuilder::get_heading_style(int level) const {
    auto it = heading_styles_.find(level);
    if (it != heading_styles_.end()) {
        return it->second;
    }
    // heading defaults
    ParagraphStyle style;
    style.text.font_family = &fonts_;
    style.text.bold = true;
    style.text.color = Color::black();
    style.alignment = TextAlignment::Left;
    style.line_spacing = 1.0f;

    switch (level) {
        case 1:
            style.text.font_size = 18.0f;
            style.space_before = 18.0f;
            style.space_after = 10.0f;
            break;
        case 2:
            style.text.font_size = 16.0f;
            style.space_before = 14.0f;
            style.space_after = 8.0f;
            break;
        case 3:
            style.text.font_size = 14.0f;
            style.space_before = 12.0f;
            style.space_after = 6.0f;
            break;
        default:
            style.text.font_size = 12.0f;
            style.space_before = 10.0f;
            style.space_after = 4.0f;
            break;
    }

    return style;
}

void DocumentBuilder::ensure_fonts() {
    if (!fonts_set_) {
        default_fonts("Times-Roman", "Times-Bold", "Times-Italic", "Times-BoldItalic", "Courier");
    }
}

} // namespace pdfmaker
