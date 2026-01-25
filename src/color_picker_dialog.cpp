#include "color_picker_dialog.h"
#include <QVBoxLayout>
#include <QGridLayout>
#include <QSlider>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>

ColorPickerDialog::ColorPickerDialog(const QColor& initial, QWidget* parent)
    : QDialog(parent), color_(initial) {

    setWindowTitle(tr("Select color"));
    setModal(true);
    setMinimumWidth(300);

    auto* main = new QVBoxLayout(this);

    preview = new QLabel();
    preview->setFixedHeight(40);
    preview->setFrameShape(QFrame::Box);
    main->addWidget(preview);

    auto* grid = new QGridLayout();
    main->addLayout(grid);

    auto make_slider = []() {
        auto* s = new QSlider(Qt::Horizontal);
        s->setRange(0, 255);
        return s;
    };

    slider_r = make_slider();
    slider_g = make_slider();
    slider_b = make_slider();

    grid->addWidget(new QLabel("R"), 0, 0);
    grid->addWidget(slider_r, 0, 1);
    grid->addWidget(new QLabel("G"), 1, 0);
    grid->addWidget(slider_g, 1, 1);
    grid->addWidget(new QLabel("B"), 2, 0);
    grid->addWidget(slider_b, 2, 1);

    edit_hex = new QLineEdit();
    grid->addWidget(new QLabel("Hex"), 3, 0);
    grid->addWidget(edit_hex, 3, 1);

    connect(slider_r, &QSlider::valueChanged,
            this, &ColorPickerDialog::slot_slider_changed);
    connect(slider_g, &QSlider::valueChanged,
            this, &ColorPickerDialog::slot_slider_changed);
    connect(slider_b, &QSlider::valueChanged,
            this, &ColorPickerDialog::slot_slider_changed);

    connect(edit_hex, &QLineEdit::editingFinished,
            this, &ColorPickerDialog::slot_hex_changed);

    auto* buttons = new QHBoxLayout();
    main->addLayout(buttons);
    buttons->addStretch();

    auto* btn_cancel = new QPushButton("Cancel");
    auto* btn_ok = new QPushButton("OK");

    buttons->addWidget(btn_cancel);
    buttons->addWidget(btn_ok);

    connect(btn_ok, &QPushButton::clicked, this, &QDialog::accept);
    connect(btn_cancel, &QPushButton::clicked, this, &QDialog::reject);

    slider_r->setValue(color_.red());
    slider_g->setValue(color_.green());
    slider_b->setValue(color_.blue());

    update_ui();
}

void ColorPickerDialog::slot_slider_changed() {
    color_.setRgb(
        slider_r->value(),
        slider_g->value(),
        slider_b->value()
    );
    update_ui();
}

void ColorPickerDialog::slot_hex_changed() {
    QColor c(edit_hex->text());
    if (c.isValid()) {
        color_ = c;
        slider_r->setValue(c.red());
        slider_g->setValue(c.green());
        slider_b->setValue(c.blue());
        update_ui();
    }
}

void ColorPickerDialog::update_ui() {
    preview->setStyleSheet(
        QString("background-color: %1").arg(color_.name())
    );
    edit_hex->setText(color_.name());
}

QColor ColorPickerDialog::color() const {
    return color_;
}
