#pragma once

#include "pdfmaker/layout/content_block.hpp"
#include "pdfmaker/layout/text_block.hpp"
#include "pdfmaker/style.hpp"

#include <memory>
#include <string>
#include <vector>

namespace pdfmaker {

enum class ListType {
    Unordered,
    Ordered
};

class ListBlock : public ContentBlock {
public:
    ListBlock(ListType type, const FontFamily* fonts, float font_size = 14.0f);

    void add_item(const std::string& text);
    void add_item(std::unique_ptr<ContentBlock> block);

    LayoutResult layout(LayoutContext& ctx) override;
    float estimate_height(const LayoutContext& ctx) const override;

private:
    ListType type_;
    const FontFamily* fonts_;
    float font_size_;
    float indent_ = 20.0f; // indent for list items
    std::vector<std::unique_ptr<ContentBlock>> items_;
    std::vector<std::string> item_texts_; // simple text items
};

} // namespace pdfmaker
