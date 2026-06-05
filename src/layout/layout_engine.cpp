#include "pdfmaker/layout/layout_engine.hpp"

namespace pdfmaker {

void LayoutEngine::set_page_style(PageStyle style) {
    page_style_ = style;
}

void LayoutEngine::set_default_fonts(const FontFamily* family) {
    fonts_ = family;
}

void LayoutEngine::set_header(const std::string& left, const std::string& center, const std::string& right) {
    header_.left = left;
    header_.center = center;
    header_.right = right;
}

void LayoutEngine::set_footer(const std::string& left, const std::string& center, const std::string& right) {
    footer_.left = left;
    footer_.center = center;
    footer_.right = right;
}

void LayoutEngine::set_skip_first_page_decorations(bool skip) {
    skip_first_page_decorations_ = skip;
}

void LayoutEngine::set_first_page_number(int n) {
    first_page_number_ = n;
}

Page LayoutEngine::create_page(Document& doc) {
    Page page = doc.add_page(page_style_);
    pages_.push_back(page);
    return page;
}

void LayoutEngine::render(Document& doc, std::vector<std::unique_ptr<ContentBlock>>& blocks) {
    pages_.clear();
    toc_.entries.clear();

    // gather the heading outline up front so the TOC can reserve space
    std::vector<TocOutlineItem> outline;
    for (auto& block : blocks) {
        block->collect_outline(outline);
    }

    // create the first page
    Page current_page = create_page(doc);
    float page_height = current_page.height();
    float page_width = current_page.width();

    LayoutContext ctx{
        doc,
        &current_page,
        page_style_.content_top(page_height), // cursor starts at top of content area
        page_style_.content_left(),
        page_style_.content_width(page_width),
        page_style_.content_bottom(),
        page_style_,
        fonts_,
        nullptr, // new_page callback set below
        &toc_,
        1,
        &outline
    };

    // new_page callback
    ctx.new_page = [&]() {
        current_page = create_page(doc);
        ctx.page = &current_page;
        ctx.current_page_number = static_cast<int>(pages_.size());
        ctx.cursor_y = page_style_.content_top(current_page.height());
    };

    // lay out all blocks
    for (auto& block : blocks) {
        block->layout(ctx);
        doc.check();
    }

    // deferred pass: blocks that reserved space now draw their content
    for (auto& block : blocks) {
        block->finalize(ctx);
        doc.check();
    }

    // render headers and footers on all pages
    if (fonts_) {
        HeaderFooterRenderer renderer(page_style_, fonts_->regular);
        renderer.render(pages_, header_, footer_, skip_first_page_decorations_,
                        first_page_number_);
        doc.check();
    }
}

} // namespace pdfmaker
