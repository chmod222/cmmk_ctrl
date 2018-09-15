#include "KeyboardButton.hh"

#include <QtSvg>

KeyboardButton::KeyboardButton(QString const& label, int w, int h, QWidget* parent)
    : QLabel{parent},
      _w{w},
      _h{h}
{
    (void)label;

    if (w == 6 && h == 8) {
        // Special case: ISO return
        _fileName = "../assets/key_iso_return.svg";
    } else {
        _fileName = QString("../assets/key_%1x%2.svg").arg(QString::number(w), QString::number(h));
    }
}

void KeyboardButton::mouseReleaseEvent(QMouseEvent* event)
{
    emit clicked(event);
}

void KeyboardButton::setColor(QColor const& col)
{
    QSvgRenderer renderer{_fileName};

    QImage img{width(), height(), QImage::Format_ARGB32};
    QPainter p{&img};
    renderer.render(&p);

    for (int i = 0; i < img.width(); ++i) {
        for (int j = 0; j < img.height(); ++j) {
            auto pixel = img.pixelColor(i, j);

            auto h = col.hueF();
            auto s = col.saturationF();

            // The higher the third parameter, the 
            auto l = std::max(0.1, col.valueF() * pixel.valueF());

            pixel.setHsvF(h, s, l, pixel.alphaF());

            img.setPixelColor(i, j, pixel);
        }
    }

    setPixmap(QPixmap::fromImage(img));
}

