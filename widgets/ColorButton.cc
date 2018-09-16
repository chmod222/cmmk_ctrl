#include "ColorButton.hh"

#include <QColorDialog>

ColorButton::ColorButton(QWidget* parent)
    : QPushButton{parent},
      _color{QColor{255, 255, 255}}
{
    connect(this, &QPushButton::clicked, this, [=] {
        auto selected = QColorDialog::getColor(selectedColor(), this);

        if (selected.isValid()) {
            setSelectedColor(selected);

            emit colorSelected(selected);
        }
    });

    setSelectedColor(_color);
}

ColorButton::~ColorButton()
{
}


QColor const& ColorButton::selectedColor() const
{
    return _color;
}

void ColorButton::setSelectedColor(QColor const& color)
{
    _color = color;

    setStyleSheet(QString("ColorButton { background-color: %1 }").arg(_color.name()));
    clearFocus();
}
