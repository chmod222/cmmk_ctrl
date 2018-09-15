#ifndef COLORBUTTON_HH
#define COLORBUTTON_HH

#include <QPushButton>

class ColorButton : public QPushButton
{
    Q_OBJECT

public:
    explicit ColorButton(QWidget* parent = nullptr);
    virtual ~ColorButton() override;

    Q_PROPERTY(QColor selectedColor
      MEMBER _color
      READ selectedColor
      WRITE setSelectedColor);

    QColor const& selectedColor() const;

    void setSelectedColor(QColor const& color);

signals:
    void colorSelected(QColor color);

private:
    QColor _color;
};

#endif
