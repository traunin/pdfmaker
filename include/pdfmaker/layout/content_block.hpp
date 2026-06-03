#pragma once

#include "pdfmaker/style.hpp"
#include "pdfmaker/page.hpp"

#include <functional>
#include <string>
#include <vector>

namespace pdfmaker {

class Document;

struct TocEntry {
    int level;
    std::string text;
    int page_number;
};

struct TocCollector {
    std::vector<TocEntry> entries;
};

// heading metadata gathered before layout, so the TOC can compute how much
// vertical space to reserve
struct TocOutlineItem {
    int level;
    std::string text;
};

struct LayoutContext {
    Document& doc;
    Page* page;
    float cursor_y;
    float content_left;
    float content_width;
    float page_bottom;
    PageStyle page_style;
    const FontFamily* default_fonts;
    std::function<void()> new_page;
    TocCollector* toc = nullptr;
    int current_page_number = 1;
    const std::vector<TocOutlineItem>* toc_outline = nullptr;
};

struct LayoutResult {
    float height_used;
    bool page_break_occurred;
};

class ContentBlock {
public:
    virtual ~ContentBlock() = default;
    virtual LayoutResult layout(LayoutContext& ctx) = 0;
    virtual float estimate_height(const LayoutContext& ctx) const = 0;

    // append this block's heading entries to the document outline
    virtual void collect_outline(std::vector<TocOutlineItem>& /*out*/) const {}

    // draw content that depends on data collected during layout
    virtual void finalize(LayoutContext& /*ctx*/) {}
};

} // namespace pdfmaker
