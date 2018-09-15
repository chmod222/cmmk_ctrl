#include "XColorButton.hh"

#include <QColorDialog>

XColorButton::XColorButton(QWidget* parent)
    : QPushButton{parent},
      _color{QColor{255, 255, 255}}
{
    connect(this, &QPushButton::clicked, this, [=] {
        auto selected = QColorDialog::getColor(selectedColor(), this);

        setSelectedColor(selected);

        emit colorSelected(selected);
    });

    setSelectedColor(_color);
}

XColorButton::~XColorButton()
{
}


QColor const& XColorButton::selectedColor() const
{
    return _color;
}

void XColorButton::setSelectedColor(QColor const& color)
{
    _color = color;

    setStyleSheet(QString("XColorButton { background-color: %1 }").arg(_color.name()));
    clearFocus();
}
