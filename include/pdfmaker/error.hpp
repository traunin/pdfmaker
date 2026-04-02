#pragma once

#include <hpdf.h>
#include <stdexcept>
#include <string>

namespace pdfmaker {

class PdfError : public std::runtime_error {
public:
    PdfError(HPDF_STATUS code, HPDF_STATUS detail);

    HPDF_STATUS code() const noexcept { return code_; }
    HPDF_STATUS detail_code() const noexcept { return detail_; }

private:
    HPDF_STATUS code_;
    HPDF_STATUS detail_;

    static std::string format_message(HPDF_STATUS code, HPDF_STATUS detail);
};

} // namespace pdfmaker
