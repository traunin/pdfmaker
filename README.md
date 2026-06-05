# pdfmaker

A modern C++17 library for generating PDF documents. It wraps
[libharu](https://github.com/libharu/libharu) (libHPDF 2.4) with RAII ownership,
typed handles, and exception-based error handling, and adds a higher-level
layout engine for flowing documents (paragraphs, headings, lists, tables,
images, a table of contents, and running headers/footers).

The library exposes two tiers that can be mixed freely:

| Tier | Entry point | Use it for |
|------|-------------|------------|
| **Low-level** | `pdfmaker::Document` + `pdfmaker::Page` | Hand-placing text, shapes, and images at absolute coordinates. Full control, no layout. |
| **High-level** | `pdfmaker::DocumentBuilder` | Multi-page flowing content with automatic pagination, word wrap, page numbers, and a TOC. |

## Features

- **RAII everywhere.** A `Document` owns the underlying `HPDF_Doc`; `Page`,
  `Font`, and `Image` are cheap non-owning handles. No manual `HPDF_Free`.
- **Deferred error checking.** libharu reports errors through a recorded error
  state rather than across C call frames; pdfmaker surfaces them as a
  `PdfError` exception at well-defined points (`Document::check()`,
  `Document::save()`), avoiding undefined behaviour.
- **UTF-8 / Cyrillic support** via embedded TrueType fonts.
- **Layout engine**: paragraphs with justification and red-line indent, level
  styled headings, ordered/unordered lists, auto-sized tables, scaled images
  with captions, a two-pass table of contents, page breaks, and
  `{page}`-substituting headers/footers.
- **GOST 7.32-friendly** geometry helpers (A4, configurable margins, first page
  number offset for an external title page).

## Requirements

- C++17 compiler (MSVC 2019+, GCC 9+, Clang 10+)
- CMake 3.16+
- Internet access on first configure: zlib, libpng, and libharu are pulled in
  via `FetchContent` and built statically.

## Building

```bash
cmake -B build -G "Visual Studio 17 2022"   # or "Ninja", "Unix Makefiles", ...
cmake --build build --config Release
```

CMake options:

| Option | Default | Effect |
|--------|---------|--------|
| `PDFMAKER_BUILD_EXAMPLES` | `ON` | Build the programs under `examples/`. |
| `PDFMAKER_BUILD_TESTS` | `ON` | Build and register the test targets. |

> **Note (Windows/MSVC):** the dependencies are fetched and compiled with the
> chosen generator. If you switch generators, delete `build/` first — a stale
> `CMakeCache.txt` from a different generator causes a configure error.

### Using it from another CMake project

```cmake
add_subdirectory(pdfmaker)        # or FetchContent_Declare the repo
target_link_libraries(my_app PRIVATE pdfmaker)
```

Then include the umbrella header:

```cpp
#include <pdfmaker/pdfmaker.hpp>
```

## Quick start — low-level (`Document` / `Page`)

Draw directly at absolute coordinates. The origin is the **bottom-left** corner;
y grows upward (PDF convention).

```cpp
#include <pdfmaker/pdfmaker.hpp>

int main() {
    pdfmaker::Document doc;
    doc.set_title("Hello");

    pdfmaker::PageStyle style;        // A4, default margins
    pdfmaker::Page page = doc.add_page(style);

    pdfmaker::Font font = doc.get_font("Times-Roman");

    {
        auto text = page.begin_text();         // RAII BeginText/EndText
        page.set_font(font, 14.0f);
        page.set_fill_color(pdfmaker::Color::black());
        page.text_out(85.0f, page.height() - 60.0f, "Hello, World!");
    }

    doc.save("hello.pdf");            // throws pdfmaker::PdfError on failure
}
```

See [`examples/hello_world.cpp`](examples/hello_world.cpp) for a fuller example
with rectangles, lines, and colours.

## Quick start — high-level (`DocumentBuilder`)

The builder lays out flowing content across as many pages as needed. Methods
are chainable.

```cpp
#include <pdfmaker/pdfmaker.hpp>
using namespace pdfmaker;

int main() {
    DocumentBuilder builder;

    builder.title("Report")
           .author("Jane Doe")
           // Embed TrueType fonts so Cyrillic / UTF-8 text renders.
           .load_ttf_fonts("times.ttf", "timesbd.ttf", "timesi.ttf",
                           "timesbi.ttf", "cour.ttf")
           .footer("", "{page}", "");          // centered page number

    builder.add_toc("Contents")
           .add_page_break();

    builder.add_heading(1, "Introduction")
           .add_paragraph("This document was produced with pdfmaker. "
                          "Long paragraphs wrap and paginate automatically.")
           .add_list(ListType::Unordered, {"First point", "Second point"})
           .add_image("chart.png", "Figure 1 - Quarterly results");

    builder.build("report.pdf");
}
```

## Documentation

- **[docs/api.md](docs/api.md)** — full reference for every public type.
- **[docs/guide.md](docs/guide.md)** — task-oriented guide: fonts & UTF-8,
  styling, tables, images, TOC, headers/footers, and a complete GOST 7.32
  document walkthrough.

## Error handling

Almost every operation can fail inside libharu. pdfmaker collects those errors
and raises `pdfmaker::PdfError` (derived from `std::runtime_error`) at
`build()` / `save()` / `check()`:

```cpp
try {
    builder.build("out.pdf");
} catch (const pdfmaker::PdfError& e) {
    std::cerr << e.what()                       // human-readable message
              << " (code 0x" << std::hex << e.code() << ")\n";
}
```

## License

See the repository for license terms. Bundled dependencies (zlib, libpng,
libharu) retain their respective licenses.
