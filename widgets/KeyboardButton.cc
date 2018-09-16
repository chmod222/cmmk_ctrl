/*
 * This file is part of cmmk_ctrl.
 *
 * cmmk_ctrl is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published
 * by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * cmmk_ctrl is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with cmmk_ctrl.  If not, see <http://www.gnu.org/licenses/>.
 */
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
        _fileName = DATADIR "/cmmk_ctrl/img/key_iso_return.svg";
    } else {
        _fileName = QString(DATADIR "/cmmk_ctrl/img/key_%1x%2.svg").arg(QString::number(w), QString::number(h));
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

