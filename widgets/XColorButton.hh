#ifndef XCOLORBUTTON_HH
#define XCOLORBUTTON_HH

#include <QPushButton>

class XColorButton : public QPushButton
{
    Q_OBJECT

public:
    explicit XColorButton(QWidget* parent = nullptr);
    virtual ~XColorButton() override;

    Q_PROPERTY(QColor selectedColor MEMBER _color READ selectedColor WRITE setSelectedColor);

    QColor const& selectedColor() const;

    void setSelectedColor(QColor const& color);

signals:
    void colorSelected(QColor color);

private:
    QColor _color;
};

#endif
