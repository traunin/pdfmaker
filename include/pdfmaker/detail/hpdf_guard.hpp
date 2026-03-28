#pragma once

#include <hpdf.h>

namespace pdfmaker::detail {

class HpdfGuard{
public:
    HpdfGuard();
    ~HpdfGuard();

    HpdfGuard(const HpdfGuard&) = delete;
    HpdfGuard& operator=(const HpdfGuard&) = delete;

    HpdfGuard(HpdfGuard&& other) noexcept;
    HpdfGuard& operator=(HpdfGuard&& other) noexcept;

    HPDF_Doc get() const noexcept { return doc_; }

private:
    HPDF_Doc doc_ = nullptr;

    static void error_handler(HPDF_STATUS error_no, HPDF_STATUS detail_no, void* user_data);
};

} // namespace pdfmaker::detail
