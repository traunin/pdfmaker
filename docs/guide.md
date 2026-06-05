# pdfmaker Guide

A task-oriented tour of the library. For the exhaustive type reference see
[api.md](api.md).

- [Two ways to build a PDF](#two-ways-to-build-a-pdf)
- [Coordinates and units](#coordinates-and-units)
- [Fonts and UTF-8 / Cyrillic](#fonts-and-utf-8--cyrillic)
- [Styling paragraphs and headings](#styling-paragraphs-and-headings)
- [Lists](#lists)
- [Tables](#tables)
- [Images and captions](#images-and-captions)
- [Table of contents](#table-of-contents)
- [Headers, footers, and page numbers](#headers-footers-and-page-numbers)
- [Title pages and number offsets](#title-pages-and-number-offsets)
- [Error handling](#error-handling)
- [Worked example: a GOST 7.32 document](#worked-example-a-gost-732-document)

---

## Two ways to build a PDF

**Low-level** (`Document` + `Page`) gives you a blank canvas. You add pages and
draw text, lines, rectangles, and images at exact coordinates. Nothing flows or
wraps — you are in full control and fully responsible.

**High-level** (`DocumentBuilder`) is a flowing layout engine. You append
semantic blocks (paragraphs, headings, lists, tables, images, a TOC) and it
paginates, wraps text, draws page numbers, and resolves the contents page for
you.

You can drop down to the low level any time via `DocumentBuilder::document()`.

---

## Coordinates and units

- Units are points (1 pt = 1/72 inch). A4 = 595.28 × 841.89 pt.
- The origin `(0, 0)` is the **bottom-left** corner; **y grows upward**.
- To place text near the top of an A4 page, use a large y such as
  `page.height() - margin_top`.

Handy metric conversions:

| mm | pt | mm | pt |
|----|------|----|------|
| 10 | 28.35 | 20 | 56.7 |
| 12.5 | 35.4 | 30 | 85.0 |
| 15 | 42.5 | 25 | 70.9 |

---

## Fonts and UTF-8 / Cyrillic

libharu's 14 built-in fonts (`"Times-Roman"`, `"Helvetica"`, `"Courier"`, …)
only cover Latin text. For Cyrillic or any UTF-8 content, load TrueType fonts.

**High-level — one call wires everything up:**

```cpp
builder.load_ttf_fonts("times.ttf",   // regular
                       "timesbd.ttf", // bold
                       "timesi.ttf",  // italic
                       "timesbi.ttf", // bold-italic
                       "cour.ttf");   // monospace (code blocks)
```
This loads and embeds all five faces, enables UTF-8, and makes the family the
default. Bold/italic runs and code blocks then resolve to the right face
automatically.

**Low-level — load names, build a family yourself:**

```cpp
doc.use_utf8_encoding();
std::string reg = doc.load_ttf_font("times.ttf");
Font times = doc.get_font(reg, "UTF-8");
```

> On Windows, passing Cyrillic text **as command-line arguments** requires
> reading the wide argv (`CommandLineToArgvW`) and converting to UTF-8 — the
> ANSI code page mangles it otherwise. Text in source files / data files is
> unaffected as long as the file is UTF-8.

---

## Styling paragraphs and headings

The body look is one `ParagraphStyle`:

```cpp
ParagraphStyle body;
body.text.font_size   = 14.0f;
body.alignment        = TextAlignment::Justify;
body.line_spacing     = 1.5f;       // true 1.5 line height (see LINE_HEIGHT_RATIO)
body.first_line_indent = 35.4f;     // 12.5 mm red-line indent
body.space_after      = 0.0f;
builder.body_style(body);
```

Per-paragraph overrides go through the two-argument `add_paragraph`. Per-level
heading looks go through `heading_style(level, ...)`, or pass a style straight to
`add_heading`:

```cpp
ParagraphStyle h;
h.text.font_size = 14.0f;
h.text.bold      = true;
h.line_spacing   = 1.5f;
h.space_after    = 24.0f;           // one empty line after the heading
h.alignment      = TextAlignment::Left;
h.first_line_indent = 35.4f;
builder.add_heading(1, "Анализ предметной области", h);
```

You can leave `text.font_family` null in any style handed to the builder — it
injects the configured family. (At the low level, a style with no font renders
nothing.)

---

## Lists

```cpp
builder.add_list(ListType::Unordered, {"Apples", "Pears", "Plums"});
builder.add_list(ListType::Ordered,   {"Wash", "Peel", "Slice"});
```
Items wrap and paginate like paragraphs; ordered lists are numbered
automatically.

---

## Tables

Build a `TableSpec`, then add it. Columns with `width_fraction == 0` auto-size;
otherwise the value is a fraction of content width. Alignment can be set per
column and overridden per cell.

```cpp
TableSpec spec;
spec.add_column("Метод",        0.0f, TextAlignment::Left);
spec.add_column("Назначение",   0.0f, TextAlignment::Left);
spec.add_row({"add_paragraph", "Добавляет абзац"});
spec.add_row({"add_table",     "Добавляет таблицу"});
builder.add_table(spec, 12.0f);
```

For a GOST-style caption above the table, add it as a body paragraph first so it
inherits the configured fonts:

```cpp
builder.add_paragraph("Таблица 1 - Методы построителя");
builder.add_table(spec);
```

---

## Images and captions

```cpp
builder.add_image("chart.png", "Рисунок 1 - Результаты", 1.0f);
```
The image is scaled to at most `max_width_fraction` of the content width
(preserving aspect ratio) and the caption is centered beneath it. PNG and JPEG
are supported.

At the low level, load and draw explicitly:

```cpp
Image img = doc.load_png("chart.png");
page.draw_image(img.raw(), x, y, w, h);
```

---

## Table of contents

Call `add_toc` where the contents page should appear — usually first, before a
page break:

```cpp
builder.add_toc("СОДЕРЖАНИЕ");
builder.add_page_break();
```
Every `add_heading` registers an outline entry. The engine lays out in two
passes: it reserves the right amount of vertical space for the TOC, lays out the
rest of the document to learn the real page numbers, then fills in the dot
leaders and numbers. Long entries wrap and still align their page number on the
last line.

---

## Headers, footers, and page numbers

```cpp
builder.footer("", "{page}", "");   // centered page number
builder.header("Report", "", "Confidential");
```
The three arguments are the left / center / right cells. `{page}` is replaced by
the page number on each page.

To omit the header/footer on the first page (e.g. a title page laid out as the
first block):

```cpp
builder.skip_first_page_decorations(true);
```

---

## Title pages and number offsets

libharu is write-only — it cannot import pages from an existing PDF. Two
options for a title page:

1. **Lay it out yourself** as the first content (or via the low-level API) and
   call `skip_first_page_decorations(true)`.
2. **Merge externally.** Generate the body with the page numbering already
   offset, then prepend a title-page PDF with an external tool (e.g. PyMuPDF,
   `pdftk`, Ghostscript):

   ```cpp
   builder.first_page_number(2);   // first body page reads "2"
   ```
   The title page becomes physical page 1 (unnumbered) after the merge.

---

## Error handling

Wrap `build()` / `save()`:

```cpp
try {
    builder.build("out.pdf");
} catch (const pdfmaker::PdfError& e) {
    std::cerr << "PDF generation failed: " << e.what()
              << " (code 0x" << std::hex << e.code()
              << ", detail 0x" << e.detail_code() << ")\n";
    return 1;
}
```
At the low level, call `doc.check()` after a batch of operations to surface any
error libharu recorded.

---

## Worked example: a GOST 7.32 document

A complete flowing document with A4/metric margins, embedded Times fonts,
justified 14 pt body at 1.5 spacing, a contents page, centered page numbers, and
each section starting on a new page.

```cpp
#include <pdfmaker/pdfmaker.hpp>
#include <iostream>

using namespace pdfmaker;

int main() {
    try {
        DocumentBuilder builder;

        builder.author("И. И. Иванов")
               .load_ttf_fonts("times.ttf", "timesbd.ttf", "timesi.ttf",
                               "timesbi.ttf", "cour.ttf");

        // A4 with GOST 7.32 margins: 30 / 15 / 20 / 20 mm.
        PageStyle page;
        page.margin_left   = 85.0f;   // 30 mm
        page.margin_right  = 42.5f;   // 15 mm
        page.margin_top    = 56.7f;   // 20 mm
        page.margin_bottom = 56.7f;   // 20 mm
        builder.page_style(page);

        // Centered page number; numbering starts at 2 (external title page = 1).
        builder.footer("", "{page}", "");
        builder.first_page_number(2);

        // Body: 14 pt, justified, 1.5 spacing, 12.5 mm red-line indent.
        ParagraphStyle body;
        body.text.font_size    = 14.0f;
        body.alignment         = TextAlignment::Justify;
        body.line_spacing      = 1.5f;
        body.first_line_indent = 35.4f;
        builder.body_style(body);

        // Contents on its own page.
        builder.add_toc("СОДЕРЖАНИЕ")
               .add_page_break();

        // A reusable heading style: 14 pt bold, one empty line after.
        auto section_heading = [](bool structural) {
            ParagraphStyle h;
            h.text.font_size = 14.0f;
            h.text.bold      = true;
            h.line_spacing   = 1.5f;
            h.space_after    = 24.0f;                 // one empty line
            if (structural) {                          // ВВЕДЕНИЕ, ЗАКЛЮЧЕНИЕ, ...
                h.alignment         = TextAlignment::Center;
                h.first_line_indent = 0.0f;
            } else {                                   // numbered sections
                h.alignment         = TextAlignment::Left;
                h.first_line_indent = 35.4f;
            }
            return h;
        };

        // Introduction (structural, centered, new page).
        builder.add_page_break()
               .add_heading(1, "ВВЕДЕНИЕ", section_heading(true))
               .add_paragraph("Настоящая работа посвящена разработке "
                              "библиотеки для генерации PDF-документов.");

        // Section 1 (numbered, left-aligned, new page).
        builder.add_page_break()
               .add_heading(1, "1 Анализ предметной области", section_heading(false))
               .add_heading(2, "1.1 Формат PDF", section_heading(false))
               .add_paragraph("Формат PDF описывает страницы в системе "
                              "координат с началом в левом нижнем углу.");

        TableSpec spec;
        spec.add_column("Возможность", 0.0f, TextAlignment::Left);
        spec.add_column("Поддержка",   0.0f, TextAlignment::Center);
        spec.add_row({"Текст",        "да"});
        spec.add_row({"Изображения",  "да"});
        builder.add_paragraph("Таблица 1 - Возможности библиотеки")
               .add_table(spec);

        // Conclusion (structural, new page).
        builder.add_page_break()
               .add_heading(1, "ЗАКЛЮЧЕНИЕ", section_heading(true))
               .add_paragraph("Библиотека реализована и протестирована.");

        builder.build("kursovaya.pdf");
        std::cout << "Wrote kursovaya.pdf\n";
    } catch (const PdfError& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }
    return 0;
}
```

Notes:

- **`line_spacing = 1.5f`** yields a real 1.5-line advance (~24 pt at 14 pt)
  because line height is `font_size * line_spacing * LINE_HEIGHT_RATIO` with
  `LINE_HEIGHT_RATIO = 1.15`. Using `font_size * 1.5` directly would be too
  tight.
- **`space_after = 24.0f`** on headings leaves exactly one blank line before the
  body, as GOST 7.32 requires.
- **`add_page_break()` before each level-1 heading** starts every section on a
  fresh page. (Guard against a redundant break right after the TOC's own page
  break.)
- **`first_page_number(2)`** assumes a title page is merged in front afterwards.
  Drop it if pdfmaker produces page 1 itself.
