#include "pdfmaker/image.hpp"

namespace pdfmaker {

float Image::width() const {
    return static_cast<float>(HPDF_Image_GetWidth(handle_));
}

float Image::height() const {
    return static_cast<float>(HPDF_Image_GetHeight(handle_));
}

} // namespace pdfmaker
