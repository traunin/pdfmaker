# pdfmaker API Reference

All types live in namespace `pdfmaker`. Include everything with
`#include <pdfmaker/pdfmaker.hpp>`, or pull in individual headers as needed.

Conventions:

- **Units** are PostScript points (1 pt = 1/72 inch). A4 is 595.28 × 841.89 pt.
- **Coordinate origin** is the bottom-left corner; y increases upward.
- **Handles** (`Page`, `Font`, `Image`) are non-owning. They are valid only as
  long as the parent `Document` is alive. Copying a handle is cheap.

---

## Contents

- [Document](#document)
- [Page](#page) and [TextScope](#textscope)
- [Font](#font) and [FontFamily](#fontfamily)
- [Image](#image)
- [Color](#color)
- [Styles](#styles): `TextStyle`, `ParagraphStyle`, `PageStyle`, `TextAlignment`
- [Table types](#table-types): `TableSpec`, `TableColumn`, `TableCell`
- [DocumentBuilder](#documentbuilder)
- [Error handling](#error-handling): `PdfError`
- [Layout internals](#layout-internals)

---

## Document

`#include <pdfmaker/document.hpp>`

Owns the underlying `HPDF_Doc`. Move-only (copying is deleted). This is the
low-level entry point; pages are drawn on directly.

```cpp
Document();
~Document();
Document(Document&&) noexcept;
Document& operator=(Document&&) noexcept;
```

### Metadata

```cpp
void set_title(std::string_view title);
void set_author(std::string_view author);
void set_subject(std::string_view subject);
```

### Fonts

```cpp
Font get_font(std::string_view name, std::string_view encoding = "");
```
Returns a handle to one of libharu's built-in base-14 fonts (e.g.
`"Times-Roman"`, `"Times-Bold"`, `"Helvetica"`, `"Courier"`). Built-in fonts
cover Latin text only.

```cpp
std::string load_ttf_font(const std::filesystem::path& path, bool embed = true);
```
Loads a TrueType font and returns its internal name (pass that name to
`get_font`). `embed = true` embeds the glyph data in the PDF.

```cpp
FontFamily make_font_family(std::string_view regular,
                            std::string_view bold = "",
                            std::string_view italic = "",
                            std::string_view bold_italic = "",
                            std::string_view mono = "",
                            std::string_view encoding = "");
```
Builds a `FontFamily` from font names already known to the document.

```cpp
void use_utf8_encoding();
```
Switches the document to UTF-8 text encoding. Call before loading TTF fonts you
intend to use for UTF-8 (e.g. Cyrillic) text.

### Pages

```cpp
Page add_page();                       // default PageStyle
Page add_page(const PageStyle& style); // sized + oriented per style
int  page_count() const;
```

### Images

```cpp
Image load_png(const std::filesystem::path& path);
Image load_jpeg(const std::filesystem::path& path);
```

### Output and errors

```cpp
void save(const std::filesystem::path& path);  // writes the PDF; throws PdfError
void check() const;                             // throws PdfError if one is pending
HPDF_Doc raw() const noexcept;                  // escape hatch to the C handle
```

---

## Page

`#include <pdfmaker/page.hpp>`

Non-owning handle to a page. Must not outlive its `Document`. Default-constructs
to a null handle; `explicit operator bool()` reports validity.

### Geometry

```cpp
float width() const;
float height() const;
void  set_size(HPDF_PageSizes size, HPDF_PageDirection direction);
```

### Text

```cpp
[[nodiscard]] TextScope begin_text();           // RAII BeginText/EndText
void set_font(Font font, float size);
void text_out(float x, float y, const std::string& text);   // absolute position
void move_text_pos(float x, float y);
void show_text(const std::string& text);
void set_text_leading(float leading);
```
Text operations must happen inside a `TextScope` (see below). `text_out` places
text at an absolute `(x, y)`; `move_text_pos` + `show_text` are the relative
two-step form.

### Graphics

```cpp
void set_line_width(float width);
void set_stroke_color(Color c);
void set_fill_color(Color c);

void rectangle(float x, float y, float width, float height); // path only
void line(float x1, float y1, float x2, float y2);
void move_to(float x, float y);
void line_to(float x, float y);
void stroke();        // paint the current path's outline
void fill();          // fill the current path
void fill_stroke();   // both
```
`rectangle`/`line`/`move_to`/`line_to` build a path; call `stroke`, `fill`, or
`fill_stroke` to paint it.

### Images

```cpp
void draw_image(HPDF_Image image, float x, float y, float w, float h);
```
Draws an image (from `Image::raw()`) scaled into the given box.

```cpp
HPDF_Page raw() const noexcept;
explicit operator bool() const noexcept;
```

### TextScope

RAII wrapper that emits `BeginText` on construction and `EndText` on
destruction. Non-copyable. Obtain one via `Page::begin_text()` and keep it in
scope while drawing text:

```cpp
{
    auto text = page.begin_text();
    page.set_font(font, 12.0f);
    page.text_out(72.0f, 700.0f, "Hi");
} // EndText emitted here
```

---

## Font

`#include <pdfmaker/font.hpp>`

Non-owning handle to a loaded font.

```cpp
float text_width(std::string_view text, float size) const; // measured width, pt
int   ascent() const;
int   descent() const;
HPDF_Font raw() const noexcept;
explicit operator bool() const noexcept;
```

### FontFamily

Bundles the four style variants plus a monospace face used by the layout engine
to resolve runs.

```cpp
struct FontFamily {
    Font regular, bold, italic, bold_italic, mono;
    Font resolve(bool is_bold, bool is_italic, bool is_mono) const;
};
```

---

## Image

`#include <pdfmaker/image.hpp>`

Non-owning handle to a decoded image. `width()`/`height()` return the intrinsic
pixel dimensions (as floats). Pass `raw()` to `Page::draw_image`.

---

## Color

`#include <pdfmaker/color.hpp>`

Plain RGB triple in the range `[0, 1]`. `constexpr`-friendly.

```cpp
struct Color {
    float r = 0, g = 0, b = 0;
    static constexpr Color rgb(float r, float g, float b);
    static constexpr Color black();
    static constexpr Color white();
    static constexpr Color gray(float v = 0.5f);
    static Color from_hex(std::string_view hex);   // "#1a3c6e" or "1a3c6e"
    bool operator==(const Color&) const;
    bool operator!=(const Color&) const;
};
```

---

## Styles

`#include <pdfmaker/style.hpp>`

### `TextAlignment`

```cpp
enum class TextAlignment { Left, Right, Center, Justify };
```

### `LINE_HEIGHT_RATIO`

```cpp
constexpr float LINE_HEIGHT_RATIO = 1.15f;
```
The ratio of a serif font's natural single-line height to its point size. Line
advance is `font_size * line_spacing * LINE_HEIGHT_RATIO`, so `line_spacing` is
a true multiple of the line height — matching how word processors interpret
"1.5 lines" (≈ 24 pt at 14 pt, not 21 pt).

### `TextStyle`

```cpp
struct TextStyle {
    const FontFamily* font_family = nullptr; // resolved against the family
    float font_size = 14.0f;
    Color color = Color::black();
    bool bold = false, italic = false, underline = false, mono = false;
    Font resolved_font() const;              // picks the variant from the family
};
```

### `ParagraphStyle`

```cpp
struct ParagraphStyle {
    TextStyle text;
    TextAlignment alignment = TextAlignment::Justify;
    float line_spacing = 1.5f;
    float space_before = 0.0f;        // pt added above the paragraph
    float space_after  = 0.0f;        // pt added below
    float first_line_indent = 0.0f;   // red-line indent, pt
};
```

> When supplying a `ParagraphStyle` to `DocumentBuilder` methods, you may leave
> `text.font_family` null — the builder injects its configured family so the
> text is not rendered with a missing font.

### `PageStyle`

```cpp
struct PageStyle {
    HPDF_PageSizes page_size = HPDF_PAGE_SIZE_A4;
    PageOrientation orientation = PageOrientation::Portrait; // Portrait | Landscape
    float margin_top = 72.0f, margin_bottom = 72.0f;         // 1 inch defaults
    float margin_left = 72.0f, margin_right = 72.0f;

    float content_width(float page_width) const;
    float content_height(float page_height) const;
    float content_left() const;
    float content_top(float page_height) const;
    float content_bottom() const;
};
```

Common metric margins (GOST 7.32): left 85 (30 mm), right 42.5 (15 mm),
top/bottom 56.7 (20 mm).

---

## Table types

`#include <pdfmaker/table.hpp>`

```cpp
struct TableCell   { std::string text; TextAlignment alignment = TextAlignment::Left; };
struct TableColumn { std::string header; float width_fraction = 0.0f; TextAlignment alignment = TextAlignment::Left; };

struct TableSpec {
    std::vector<TableColumn> columns;
    std::vector<std::vector<TableCell>> rows;

    void add_column(const std::string& header, float width_fraction = 0.0f,
                    TextAlignment align = TextAlignment::Left);
    void add_row(std::vector<TableCell> cells);
    void add_row(std::vector<std::string> texts);   // inherit column alignment
};
```
`width_fraction == 0` means the column is auto-sized; otherwise it is that
fraction of the content width.

---

## DocumentBuilder

`#include <pdfmaker/document_builder.hpp>`

High-level fluent builder. Configure metadata, fonts, page geometry, and styles;
append content blocks; then `build()`. All setters return `*this` for chaining.

### Metadata

```cpp
DocumentBuilder& title(const std::string& title);
DocumentBuilder& author(const std::string& author);
DocumentBuilder& subject(const std::string& subject);
```

### Page and fonts

```cpp
DocumentBuilder& page_style(PageStyle style);
DocumentBuilder& default_fonts(const std::string& regular, const std::string& bold,
                               const std::string& italic, const std::string& bold_italic,
                               const std::string& mono);              // by font name
DocumentBuilder& load_ttf_fonts(const std::filesystem::path& regular,
                                const std::filesystem::path& bold,
                                const std::filesystem::path& italic,
                                const std::filesystem::path& bold_italic,
                                const std::filesystem::path& mono);   // load + embed TTFs
```
`load_ttf_fonts` is the usual path for UTF-8 / Cyrillic documents: it loads all
five faces, enables UTF-8 encoding, and sets them as the default family.

### Styles

```cpp
DocumentBuilder& body_style(ParagraphStyle style);              // default paragraph look
DocumentBuilder& heading_style(int level, ParagraphStyle style); // per-level heading look
```

### Headers / footers / numbering

```cpp
DocumentBuilder& header(const std::string& left, const std::string& center, const std::string& right);
DocumentBuilder& footer(const std::string& left, const std::string& center, const std::string& right);
DocumentBuilder& skip_first_page_decorations(bool skip = true);  // e.g. a title page
DocumentBuilder& first_page_number(int n);                        // start numbering at n
```
In header/footer text the token `{page}` is replaced with the current page
number. `first_page_number(2)` is used when a separate title page is prepended
externally, so the first laid-out page reads "2".

### Content blocks

```cpp
DocumentBuilder& add_toc(const std::string& title = "Contents");
DocumentBuilder& add_heading(int level, const std::string& text);
DocumentBuilder& add_heading(int level, const std::string& text, ParagraphStyle style);
DocumentBuilder& add_paragraph(const std::string& text);
DocumentBuilder& add_paragraph(const std::string& text, ParagraphStyle style);
DocumentBuilder& add_code(const std::string& code, float font_size = 12.0f);
DocumentBuilder& add_list(ListType type, const std::vector<std::string>& items);
DocumentBuilder& add_table(TableSpec spec, float font_size = 12.0f);
DocumentBuilder& add_image(const std::filesystem::path& path, const std::string& caption = "",
                           float max_width_fraction = 1.0f);
DocumentBuilder& add_page_break();
```
`add_heading` also records a TOC outline entry. `add_toc` reserves space for the
contents and is filled in once all page numbers are known (two-pass layout), so
it works even though it appears before the content it lists. `add_code` renders
monospaced; `add_image` scales the image to at most `max_width_fraction` of the
content width and centers an optional caption beneath it.

### Build

```cpp
void build(const std::filesystem::path& output_path);  // lay out + save; throws PdfError
```

### Advanced access

```cpp
Document& document();              // the underlying Document
const TocCollector& toc() const;   // collected TOC entries after build
```

### `ListType`

```cpp
enum class ListType { Unordered, Ordered };  // from <pdfmaker/layout/list_block.hpp>
```

---

## Error handling

`#include <pdfmaker/error.hpp>`

```cpp
class PdfError : public std::runtime_error {
public:
    HPDF_STATUS code() const noexcept;        // primary libharu error code
    HPDF_STATUS detail_code() const noexcept; // libharu detail code
};
```
Thrown by `Document::save`, `Document::check`, and `DocumentBuilder::build`.
`what()` returns a formatted message including both codes. Because libharu
cannot safely propagate errors across C call frames, pdfmaker records the error
and raises it at these defined checkpoints rather than mid-operation.

---

## Layout internals

`#include <pdfmaker/layout/content_block.hpp>` and siblings.

These types back `DocumentBuilder` and are only needed when writing a custom
block. `ContentBlock` is the abstract interface:

```cpp
class ContentBlock {
    virtual LayoutResult layout(LayoutContext& ctx) = 0;
    virtual float estimate_height(const LayoutContext& ctx) const = 0;
    virtual void collect_outline(std::vector<TocOutlineItem>& out) const {}
    virtual void finalize(LayoutContext& ctx) {}
};
```

The engine runs three phases: `collect_outline` (gather headings so the TOC can
reserve height), `layout` (flow each block, paginating via
`LayoutContext::new_page` when `cursor_y` would drop below `page_bottom`), and
`finalize` (draw content that depends on final page numbers — the TOC's dot
leaders and page numbers). Concrete blocks: `TextBlock`, `HeadingBlock`,
`CodeBlock`, `ListBlock`, `TableBlock`, `ImageBlock`, `TocBlock`, `PageBreak`.
The `LayoutEngine` ties them together and applies `HeaderFooterSpec` decorations
(including `{page}` substitution and `first_page_number` offset).
