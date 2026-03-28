#include "pdfmaker/detail/hpdf_guard.hpp"
#include "pdfmaker/error.hpp"

#include <utility>

namespace pdfmaker::detail {

HpdfGuard::HpdfGuard() {
    doc_ = HPDF_New(error_handler, nullptr);
    if (!doc_) {
        throw pdfmaker::PdfError(0x100, 0); // failed to allocate
    }
}

HpdfGuard::~HpdfGuard() {
    if (doc_) {
        HPDF_Free(doc_);
    }
}

HpdfGuard::HpdfGuard(HpdfGuard&& other) noexcept
    : doc_(std::exchange(other.doc_, nullptr))
{
}

HpdfGuard& HpdfGuard::operator=(HpdfGuard&& other) noexcept {
    if (this != &other) {
        if (doc_) {
            HPDF_Free(doc_);
        }
        doc_ = std::exchange(other.doc_, nullptr);
    }
    return *this;
}

void HpdfGuard::error_handler(HPDF_STATUS error_no, HPDF_STATUS detail_no, void* user_data) {
    throw pdfmaker::PdfError(error_no, detail_no);
}

} // namespace pdfmaker::detail
