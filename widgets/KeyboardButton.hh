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
#ifndef KEYBOARD_BUTTON_H
#define KEYBOARD_BUTTON_H

#include <QLabel>

class KeyboardButton : public QLabel
{
  Q_OBJECT

public:
  KeyboardButton(QString const& label, int w, int h, QWidget* parent = nullptr);

  Q_PROPERTY(int width MEMBER _w READ keyWidth);
  Q_PROPERTY(int height MEMBER _h READ keyHeight);

  Q_PROPERTY(int row MEMBER _row READ row WRITE setRow);
  Q_PROPERTY(int column MEMBER _column READ column WRITE setColumn);

  Q_PROPERTY(QColor color MEMBER _color READ color WRITE setColor);

  inline int keyWidth() { return _w; }
  inline int keyHeight() { return _h; }

  inline int row() { return _row; }
  inline int column() { return _column; }

  inline void setRow(int row) { _row = row; }
  inline void setColumn(int column) { _column = column; }

	QColor const& color() const { return _color; }
  void setColor(QColor const& col);

signals:
  void clicked(QMouseEvent* event);

protected:
  void mouseReleaseEvent(QMouseEvent* event);

private:
  // Physical button dimensions in terms of the keyboard grid
  int _w;
  int _h;

  int _row;
  int _column;

  QString _fileName;
	QColor _color;
};

#endif

