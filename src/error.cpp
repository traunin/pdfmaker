#include "pdfmaker/error.hpp"

#include <sstream>

namespace pdfmaker {

PdfError::PdfError(HPDF_STATUS code, HPDF_STATUS detail)
    : std::runtime_error(format_message(code, detail))
    , code_(code)
    , detail_(detail)
{
}

std::string PdfError::format_message(HPDF_STATUS code, HPDF_STATUS detail) {
    std::ostringstream oss;
    oss << "libharu error 0x" << std::hex << code;
    if (detail != 0) {
        oss << " (detail: 0x" << detail << ")";
    }
    return oss.str();
}

namespace detail {
void check_status(HPDF_STATUS status) {
    if (status != HPDF_OK) {
        throw PdfError(status, 0);
    }
}
} // namespace detail

} // namespace pdfmaker
