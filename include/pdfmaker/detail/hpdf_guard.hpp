#pragma once

#include <hpdf.h>

#include <memory>

namespace pdfmaker::detail {

// Records the last error reported by libharu's C error callback
struct ErrorState {
    HPDF_STATUS code = 0;
    HPDF_STATUS detail = 0;
    bool has_error = false;
};

class HpdfGuard {
public:
    HpdfGuard();
    ~HpdfGuard();

    HpdfGuard(const HpdfGuard&) = delete;
    HpdfGuard& operator=(const HpdfGuard&) = delete;

    HpdfGuard(HpdfGuard&& other) noexcept;
    HpdfGuard& operator=(HpdfGuard&& other) noexcept;

    HPDF_Doc get() const noexcept { return doc_; }

    // throws PdfError if libharu recorded an error, then clears the recorded state
    // safe to call after any libharu call
    void check() const;

private:
    HPDF_Doc doc_ = nullptr;
    std::unique_ptr<ErrorState> error_;

    static void error_handler(HPDF_STATUS error_no, HPDF_STATUS detail_no, void* user_data);
};

} // namespace pdfmaker::detail
