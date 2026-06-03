#include "pdfmaker/table.hpp"

namespace pdfmaker {

void TableSpec::add_column(const std::string& header, float width_fraction, TextAlignment align) {
    columns.push_back({header, width_fraction, align});
}

void TableSpec::add_row(std::vector<TableCell> cells) {
    rows.push_back(std::move(cells));
}

void TableSpec::add_row(std::vector<std::string> texts) {
    std::vector<TableCell> cells;
    cells.reserve(texts.size());
    for (auto& t : texts) {
        cells.push_back({std::move(t)});
    }
    rows.push_back(std::move(cells));
}

} // namespace pdfmaker
