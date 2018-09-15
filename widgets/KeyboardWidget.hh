#ifndef KEYBOARD_WIDGET_H
#define KEYBOARD_WIDGET_H

#include <QFrame>

#include "KeyboardButton.hh"

extern "C" {
#include "libcmmk.h"
}

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
