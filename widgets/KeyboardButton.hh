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

