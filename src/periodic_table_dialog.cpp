#include "periodic_table_dialog.h"
#include "atom_settings.h"

#include <QVBoxLayout>
#include <QGridLayout>
#include <QPushButton>
#include <QScrollArea>

enum class ElementFamily {
    Alkali,
    AlkalineEarth,
    Transition,
    PostTransition,
    Metalloid,
    Nonmetal,
    Halogen,
    NobleGas,
    Lanthanide,
    Actinide,
    Unknown
};

static ElementFamily element_family(unsigned int z) {
    if (z == 1) return ElementFamily::Nonmetal;

    if (z >= 3 && z <= 11 && z != 10) return ElementFamily::Alkali;
    if (z == 3 || z == 11 || z == 19 || z == 37 || z == 55 || z == 87)
        return ElementFamily::Alkali;

    if (z == 4 || z == 12 || z == 20 || z == 38 || z == 56 || z == 88)
        return ElementFamily::AlkalineEarth;

    if ((z >= 21 && z <= 30) ||
        (z >= 39 && z <= 48) ||
        (z >= 72 && z <= 80) ||
        (z >= 104 && z <= 112))
        return ElementFamily::Transition;

    if ((z >= 13 && z <= 16) || (z >= 31 && z <= 34) ||
        z == 49 || z == 50 || z == 81 || z == 82 || z == 83)
        return ElementFamily::PostTransition;

    if (z == 5 || z == 14 || z == 32 || z == 33 || z == 51 || z == 52)
        return ElementFamily::Metalloid;

    if (z == 6 || z == 7 || z == 8 || z == 15 || z == 16 || z == 34)
        return ElementFamily::Nonmetal;

    if (z == 9 || z == 17 || z == 35 || z == 53 || z == 85)
        return ElementFamily::Halogen;

    if (z == 2 || z == 10 || z == 18 || z == 36 || z == 54 || z == 86 || z == 118)
        return ElementFamily::NobleGas;

    if (z >= 57 && z <= 71)
        return ElementFamily::Lanthanide;

    if (z >= 89 && z <= 103)
        return ElementFamily::Actinide;

    return ElementFamily::Unknown;
}

static QString family_color(ElementFamily fam) {
    switch (fam) {
        case ElementFamily::Alkali:          return "#f4a261";
        case ElementFamily::AlkalineEarth:   return "#e9c46a";
        case ElementFamily::Transition:      return "#90be6d";
        case ElementFamily::PostTransition:  return "#84a59d";
        case ElementFamily::Metalloid:       return "#8ecae6";
        case ElementFamily::Nonmetal:        return "#a8dadc";
        case ElementFamily::Halogen:         return "#cdb4db";
        case ElementFamily::NobleGas:        return "#bde0fe";
        case ElementFamily::Lanthanide:      return "#74c69d";
        case ElementFamily::Actinide:        return "#52b788";
        default:                             return "#dddddd";
    }
}

struct PTCell {
    int row;
    int col;
    unsigned int elnr;
};

/*
 * Grid rows 0–6  → main periodic table (periods 1–7)
 * Row 7          → spacer
 * Row 8          → lanthanides
 * Row 9          → actinides
 *
 * Columns 0–17 correspond to groups 1–18
 */
static const std::vector<PTCell> periodic_layout = {
    // Period 1
    {0, 0, 1},   {0, 17, 2},

    // Period 2
    {1, 0, 3}, {1, 1, 4},
    {1,12, 5}, {1,13, 6}, {1,14, 7}, {1,15, 8}, {1,16, 9}, {1,17,10},

    // Period 3
    {2, 0,11}, {2, 1,12},
    {2,12,13}, {2,13,14}, {2,14,15}, {2,15,16}, {2,16,17}, {2,17,18},

    // Period 4
    {3, 0,19}, {3, 1,20}, {3, 2,21}, {3, 3,22}, {3, 4,23}, {3, 5,24},
    {3, 6,25}, {3, 7,26}, {3, 8,27}, {3, 9,28}, {3,10,29}, {3,11,30},
    {3,12,31}, {3,13,32}, {3,14,33}, {3,15,34}, {3,16,35}, {3,17,36},

    // Period 5
    {4, 0,37}, {4, 1,38}, {4, 2,39}, {4, 3,40}, {4, 4,41}, {4, 5,42},
    {4, 6,43}, {4, 7,44}, {4, 8,45}, {4, 9,46}, {4,10,47}, {4,11,48},
    {4,12,49}, {4,13,50}, {4,14,51}, {4,15,52}, {4,16,53}, {4,17,54},

    // Period 6 (Cs, Ba, ☐, Hf–Rn)
    {5, 0,55}, {5, 1,56},
    {5, 3,72}, {5, 4,73}, {5, 5,74}, {5, 6,75}, {5, 7,76},
    {5, 8,77}, {5, 9,78}, {5,10,79}, {5,11,80},
    {5,12,81}, {5,13,82}, {5,14,83}, {5,15,84}, {5,16,85}, {5,17,86},

    // Period 7 (Fr, Ra, ☐, Rf–Og)
    {6, 0,87}, {6, 1,88},
    {6, 3,104},{6, 4,105},{6, 5,106},{6, 6,107},{6, 7,108},
    {6, 8,109},{6, 9,110},{6,10,111},{6,11,112},
    {6,12,113},{6,13,114},{6,14,115},{6,15,116},{6,16,117},{6,17,118}
};

PeriodicTableDialog::PeriodicTableDialog(QWidget* parent)
    : QDialog(parent) {

    setWindowTitle(tr("Select element"));
    setModal(true);
    resize(900, 520);

    build_ui();
}

void PeriodicTableDialog::build_ui() {
    auto* main = new QVBoxLayout(this);

    auto* scroll = new QScrollArea();
    scroll->setWidgetResizable(true);
    main->addWidget(scroll);

    auto* container = new QWidget();
    scroll->setWidget(container);

    grid_ = new QGridLayout(container);
    grid_->setSpacing(4);

    // Main periodic table
    for (const auto& cell : periodic_layout) {
        add_element(cell.row, cell.col, cell.elnr);
    }

    // f-block placeholders in main table
    add_placeholder(5, 2, "La–Lu", tr("Lanthanides (57–71)"));
    add_placeholder(6, 2, "Ac–Lr", tr("Actinides (89–103)"));

    // Spacer row
    grid_->setRowMinimumHeight(7, 20);

    // Lanthanides
    for (unsigned int z = 57; z <= 71; ++z) {
        add_element(8, int(z - 57) + 2, z);
    }

    // Actinides
    for (unsigned int z = 89; z <= 103; ++z) {
        add_element(9, int(z - 89) + 2, z);
    }

    auto* btn_cancel = new QPushButton(tr("Cancel"));
    connect(btn_cancel, &QPushButton::clicked, this, &QDialog::reject);
    main->addWidget(btn_cancel, 0, Qt::AlignRight);
}

void PeriodicTableDialog::add_placeholder(
    int row, int col,
    const QString& text,
    const QString& tooltip
) {
    auto* btn = new QPushButton(text);
    btn->setFixedSize(40, 40);
    btn->setEnabled(false);
    btn->setToolTip(tooltip);
    btn->setStyleSheet(
        "QPushButton {"
        " color: #666;"
        " background: #eeeeee;"
        " border: 1px solid #bbbbbb;"
        "}"
    );
    grid_->addWidget(btn, row, col);
}

void PeriodicTableDialog::add_element(int row, int col, unsigned int elnr) {
    const auto& settings = AtomSettings::get();
    const QString symbol =
        QString::fromStdString(settings.get_name_from_elnr(elnr));

    auto* btn = new QPushButton(symbol);
    btn->setFixedSize(40, 40);

    const ElementFamily fam = element_family(elnr);
    const QString bg = family_color(fam);

    btn->setStyleSheet(QString(
        "QPushButton {"
        " background-color: %1;"
        " border: 1px solid #666;"
        " font-weight: bold;"
        "}"
        "QPushButton:hover {"
        " border: 2px solid #000;"
        "}"
    ).arg(bg));

    btn->setToolTip(
        QString("%1\nZ=%2\nRadius=%3 Å")
            .arg(symbol)
            .arg(elnr)
            .arg(settings.get_atom_radius_from_elnr(elnr))
    );

    connect(btn, &QPushButton::clicked, this, [this, symbol]() {
        selected_ = symbol;
        accept();
    });

    grid_->addWidget(btn, row, col);
}

QString PeriodicTableDialog::selectedElement() const {
    return selected_;
}
