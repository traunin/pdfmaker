#pragma once

#include <hpdf.h>

namespace pdfmaker {

class Image {
public:
    Image() = default;
    explicit Image(HPDF_Image handle) : handle_(handle) {}

    float width() const;
    float height() const;

    HPDF_Image raw() const noexcept { return handle_; }
    explicit operator bool() const noexcept { return handle_ != nullptr; }

private:
    HPDF_Image handle_ = nullptr;
};

} // namespace pdfmaker
