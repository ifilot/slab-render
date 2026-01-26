#include "color_picker_dialog.h"
#include "color_wheel_widget.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QSlider>
#include <QLabel>

static QColor idealTextColor(const QColor& bg) {
    // Perceived luminance (WCAG-ish)
    const double luminance =
        0.299 * bg.redF() +
        0.587 * bg.greenF() +
        0.114 * bg.blueF();

    return luminance > 0.5 ? Qt::black : Qt::white;
}

ColorPickerDialog::ColorPickerDialog(const QColor& initial, QWidget* parent)
    : QDialog(parent) {

    setWindowTitle(tr("Select color"));
    setModal(true);
    resize(400, 420);

    auto* main = new QVBoxLayout(this);

    // ---- Wheel + slider ----
    auto* center = new QHBoxLayout();

    wheel_ = new ColorWheelWidget(this);
    center->addWidget(wheel_, 1);
    connect(
        wheel_,
        &ColorWheelWidget::colorChanged,
        this,
        &ColorPickerDialog::updateHexDisplay
    );

    valueSlider_ = new QSlider(Qt::Vertical, this);
    valueSlider_->setRange(0, 100);
    valueSlider_->setFixedWidth(22);
    center->addWidget(valueSlider_);

    main->addLayout(center);

    // ---- Hex display ----
    hexLabel_ = new QLabel(this);
    hexLabel_->setAlignment(Qt::AlignCenter);
    hexLabel_->setMinimumHeight(36);
    hexLabel_->setStyleSheet(
        "QLabel {"
        " border: 1px solid #444;"
        " border-radius: 4px;"
        " font-family: monospace;"
        " font-size: 14px;"
        " font-weight: bold;"
        "}"
    );
    main->addWidget(hexLabel_);

    // ---- Init from color ----
    qreal h, s, v;
    initial.getHsvF(&h, &s, &v);

    wheel_->setColor(initial);
    valueSlider_->setValue(static_cast<int>(v * 100));
    updateHexDisplay(initial);

    // ---- Sync slider → wheel ----
    connect(valueSlider_, &QSlider::valueChanged, this, [this](int val) {
        QColor c = wheel_->color();
        c.setHsvF(c.hueF(), c.saturationF(), val / 100.0);
        wheel_->setColor(c);
        updateHexDisplay(c);
    });

    // ---- Sync wheel changes ----
    connect(wheel_, &QWidget::customContextMenuRequested, this, [this]() {
        updateHexDisplay(wheel_->color());
    });

    // Instead: update on repaint via event filter
    wheel_->installEventFilter(this);

    // ---- Buttons ----
    auto* btns = new QHBoxLayout();
    btns->addStretch();

    auto* btnCancel = new QPushButton(tr("Cancel"));
    auto* btnOk     = new QPushButton(tr("OK"));

    btns->addWidget(btnCancel);
    btns->addWidget(btnOk);
    main->addLayout(btns);

    connect(btnCancel, &QPushButton::clicked, this, &QDialog::reject);
    connect(btnOk,     &QPushButton::clicked, this, &QDialog::accept);
}

QColor ColorPickerDialog::color() const {
    return wheel_->color();
}

void ColorPickerDialog::updateHexDisplay(const QColor& c) {
    const QString hex = c.name(QColor::HexRgb).toUpper();
    const QColor text = idealTextColor(c);

    hexLabel_->setText(hex);
    hexLabel_->setStyleSheet(QString(
        "QLabel {"
        " background-color: %1;"
        " color: %2;"
        " border: 1px solid #444;"
        " border-radius: 4px;"
        " font-family: monospace;"
        " font-size: 14px;"
        " font-weight: bold;"
        "}"
    ).arg(c.name(), text.name()));
}
