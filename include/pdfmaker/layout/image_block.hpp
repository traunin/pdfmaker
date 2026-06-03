#pragma once

#include "pdfmaker/layout/content_block.hpp"
#include "pdfmaker/image.hpp"

#include <string>

namespace pdfmaker {

class ImageBlock : public ContentBlock {
public:
    ImageBlock(Image image, float max_width_fraction = 1.0f,
               const std::string& caption = "");

    LayoutResult layout(LayoutContext& ctx) override;
    float estimate_height(const LayoutContext& ctx) const override;

private:
    Image image_;
    float max_width_fraction_;
    std::string caption_;
    float space_before_ = 6.0f;
    float space_after_ = 6.0f;
    float caption_font_size_ = 12.0f;
};

} // namespace pdfmaker
