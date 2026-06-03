#include "pdfmaker/detail/hpdf_guard.hpp"
#include "pdfmaker/error.hpp"

#include <utility>

namespace pdfmaker::detail {

HpdfGuard::HpdfGuard()
    : error_(std::make_unique<ErrorState>())
{
    doc_ = HPDF_New(error_handler, error_.get());
    if (!doc_) {
        check();
        throw pdfmaker::PdfError(HPDF_FAILD_TO_ALLOC_MEM, 0);
    }
}

HpdfGuard::~HpdfGuard() {
    if (doc_) {
        HPDF_Free(doc_);
    }
}

HpdfGuard::HpdfGuard(HpdfGuard&& other) noexcept
    : doc_(std::exchange(other.doc_, nullptr))
    , error_(std::move(other.error_))
{
}

HpdfGuard& HpdfGuard::operator=(HpdfGuard&& other) noexcept {
    if (this != &other) {
        if (doc_) {
            HPDF_Free(doc_);
        }
        doc_ = std::exchange(other.doc_, nullptr);
        error_ = std::move(other.error_);
    }
    return *this;
}

void HpdfGuard::check() const {
    if (error_ && error_->has_error) {
        HPDF_STATUS code = error_->code;
        HPDF_STATUS detail = error_->detail;
        error_->has_error = false;
        throw pdfmaker::PdfError(code, detail);
    }
}

// called by libharu from inside its C frames
void HpdfGuard::error_handler(HPDF_STATUS error_no, HPDF_STATUS detail_no, void* user_data) {
    auto* state = static_cast<ErrorState*>(user_data);
    if (state) {
        state->code = error_no;
        state->detail = detail_no;
        state->has_error = true;
    }
}

} // namespace pdfmaker::detail
