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
#ifndef KEYBOARD_WIDGET_H
#define KEYBOARD_WIDGET_H

#include <QFrame>

#include "KeyboardButton.hh"

#include "libcmmk/libcmmk.h"

class KeyboardButton;

enum class KeyboardLayout {
  ISO,
  ANSI
};

enum class KeyboardStyle {
  Normal,
  TKL
};

template <typename T>
	using keyboard = T[CMMK_ROWS_MAX][CMMK_COLS_MAX];


class KeyboardWidget : public QFrame
{
  Q_OBJECT

public:
  KeyboardWidget(KeyboardLayout layout, KeyboardStyle style, QWidget* parent);

  KeyboardButton* getKeyAt(int row, int col) const;

	void setAll(QColor const& color);

	virtual QSize minimumSizeHint() const override;

signals:
  void clicked(KeyboardButton* btn);

protected:
  void resizeEvent(QResizeEvent* event);

  void setup();

private:
  KeyboardLayout _layout;
  KeyboardStyle _style;

  keyboard<KeyboardButton*> _buttons;
};

#endif
