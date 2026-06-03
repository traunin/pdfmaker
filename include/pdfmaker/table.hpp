#pragma once

#include "pdfmaker/style.hpp"

#include <string>
#include <vector>

namespace pdfmaker {

struct TableCell {
    std::string text;
    TextAlignment alignment = TextAlignment::Left;
};

struct TableColumn {
    std::string header;
    float width_fraction = 0.0f; // 0 = auto, otherwise fraction of content width
    TextAlignment alignment = TextAlignment::Left;
};

struct TableSpec {
    std::vector<TableColumn> columns;
    std::vector<std::vector<TableCell>> rows;

    void add_column(const std::string& header, float width_fraction = 0.0f,
                    TextAlignment align = TextAlignment::Left);
    void add_row(std::vector<TableCell> cells);
    void add_row(std::vector<std::string> texts);
};

} // namespace pdfmaker
