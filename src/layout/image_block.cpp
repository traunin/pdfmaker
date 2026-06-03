#include "pdfmaker/layout/image_block.hpp"
#include "pdfmaker/document.hpp"

namespace pdfmaker {

ImageBlock::ImageBlock(Image image, float max_width_fraction, const std::string& caption)
    : image_(image)
    , max_width_fraction_(max_width_fraction)
    , caption_(caption)
{
}

float ImageBlock::estimate_height(const LayoutContext& ctx) const {
    float max_w = ctx.content_width * max_width_fraction_;
    float img_w = image_.width();
    float img_h = image_.height();

    float scale = (img_w > max_w) ? max_w / img_w : 1.0f;
    float rendered_h = img_h * scale;

    float h = space_before_ + rendered_h + space_after_;
    if (!caption_.empty()) {
        h += caption_font_size_ * 1.5f;
    }
    return h;
}

LayoutResult ImageBlock::layout(LayoutContext& ctx) {
    float total_height = 0.0f;
    bool page_break = false;

    float max_w = ctx.content_width * max_width_fraction_;
    float img_w = image_.width();
    float img_h = image_.height();

    float scale = (img_w > max_w) ? max_w / img_w : 1.0f;
    float rendered_w = img_w * scale;
    float rendered_h = img_h * scale;

    float needed = space_before_ + rendered_h + space_after_;
    if (!caption_.empty()) {
        needed += caption_font_size_ * 1.5f;
    }

    ctx.cursor_y -= space_before_;
    total_height += space_before_;

    if (ctx.cursor_y - rendered_h < ctx.page_bottom) {
        ctx.new_page();
        page_break = true;
    }

    // center the image
    float x = ctx.content_left + (ctx.content_width - rendered_w) / 2.0f;
    float y = ctx.cursor_y - rendered_h;

    ctx.page->draw_image(image_.raw(), x, y, rendered_w, rendered_h);
    ctx.cursor_y -= rendered_h;
    total_height += rendered_h;

    // caption
    if (!caption_.empty() && ctx.default_fonts) {
        float cap_y = ctx.cursor_y - caption_font_size_ * 1.2f;

        Font font = ctx.default_fonts->regular;
        float cap_w = font.text_width(caption_, caption_font_size_);
        float cap_x = ctx.content_left + (ctx.content_width - cap_w) / 2.0f;

        auto text_scope = ctx.page->begin_text();
        ctx.page->set_font(font, caption_font_size_);
        ctx.page->set_fill_color(Color::black());
        ctx.page->text_out(cap_x, cap_y, caption_);

        ctx.cursor_y -= caption_font_size_ * 1.5f;
        total_height += caption_font_size_ * 1.5f;
    }

    ctx.cursor_y -= space_after_;
    total_height += space_after_;

    return {total_height, page_break};
}

} // namespace pdfmaker
