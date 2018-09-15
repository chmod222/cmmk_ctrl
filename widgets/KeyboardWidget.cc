#include "KeyboardWidget.hh"
#include "KeyboardButton.hh"

#include <QMouseEvent>

// left gap, width, height, logical row, logial column
using key_descriptor = std::tuple<int, int, int, int, int>;
using keyboard_layout = keyboard<key_descriptor>;

static keyboard_layout layouts[2] = {
    {
        // ISO layout

        // Row 0
        {{0, 4, 4, 0, 0},
         {4, 4, 4, 0, 1}, {0, 4, 4, 0, 2}, {0, 4, 4, 0, 3}, {0, 4, 4, 0, 4},
         {2, 4, 4, 0, 6}, {0, 4, 4, 0, 7}, {0, 4, 4, 0, 8}, {0, 4, 4, 0, 9},
         {2, 4, 4, 0, 11}, {0, 4, 4, 0, 12}, {0, 4, 4, 0, 13}, {0, 4, 4, 0, 14},

         {1, 4, 4, 0, 15}, {0, 4, 4, 0, 16}, {0, 4, 4, 0, 17},

         {1, 4, 4, 0, 18}, {0, 4, 4, 0, 19}, {0, 4, 4, 0, 20}, {0, 4, 4, 0, 21}},

        // Row 1
        {{0, 4, 4, 1, 0}, {0, 4, 4, 1, 1}, {0, 4, 4, 1, 2}, {0, 4, 4, 1, 3},
         {0, 4, 4, 1, 4}, {0, 4, 4, 1, 5}, {0, 4, 4, 1, 6}, {0, 4, 4, 1, 7},
         {0, 4, 4, 1, 8}, {0, 4, 4, 1, 9}, {0, 4, 4, 1, 10}, {0, 4, 4, 1, 11},
         {0, 4, 4, 1, 12}, {0, 8, 4, 1, 14},

         {1, 4, 4, 1, 15}, {0, 4, 4, 1, 16}, {0, 4, 4, 1, 17},

         {1, 4, 4, 1, 18}, {0, 4, 4, 1, 19}, {0, 4, 4, 1, 20}, {0, 4, 4, 1, 21}},

        // Row 2
        {{0, 6, 4, 2, 0}, {0, 4, 4, 2, 1}, {0, 4, 4, 2, 2}, {0, 4, 4, 2, 3},
         {0, 4, 4, 2, 4}, {0, 4, 4, 2, 5}, {0, 4, 4, 2, 6}, {0, 4, 4, 2, 7},
         {0, 4, 4, 2, 8}, {0, 4, 4, 2, 9}, {0, 4, 4, 2, 10}, {0, 4, 4, 2, 11},
         {0, 4, 4, 2, 12}, {0, 6, 8, 2, 14},

         {1, 4, 4, 2, 15}, {0, 4, 4, 2, 16}, {0, 4, 4, 2, 17},

         {1, 4, 4, 2, 18}, {0, 4, 4, 2, 19}, {0, 4, 4, 2, 20}, {0, 4, 8, 2, 21}},

        // Row 3
        {{0, 7, 4, 3, 0}, {0, 4, 4, 3, 1}, {0, 4, 4, 3, 2}, {0, 4, 4, 3, 3},
         {0, 4, 4, 3, 4}, {0, 4, 4, 3, 5}, {0, 4, 4, 3, 6}, {0, 4, 4, 3, 7},
         {0, 4, 4, 3, 8}, {0, 4, 4, 3, 9}, {0, 4, 4, 3, 10}, {0, 4, 4, 3, 11},
         {0, 4, 4, 3, 12},

         {19, 4, 4, 3, 18}, {0, 4, 4, 3, 19}, {0, 4, 4, 3, 20}},

        // Row 4
        {{0, 5, 4, 4, 0}, {0, 4, 4, 4, 1}, {0, 4, 4, 4, 2}, {0, 4, 4, 4, 3},
         {0, 4, 4, 4, 4}, {0, 4, 4, 4, 5}, {0, 4, 4, 4, 6}, {0, 4, 4, 4, 7},
         {0, 4, 4, 4, 8}, {0, 4, 4, 4, 9}, {0, 4, 4, 4, 10}, {0, 4, 4, 4, 11},
         {0, 11, 4, 4, 14},

         {5, 4, 4, 4, 16},

         {5, 4, 4, 4, 18}, {0, 4, 4, 4, 19}, {0, 4, 4, 4, 20}, {0, 4, 8, 4, 21}},

        // Row 5
        {{0, 5, 4, 5, 0}, {0, 5, 4, 5, 1}, {0, 5, 4, 5, 2}, {0, 25, 4, 5, 6},
         {0, 5, 4, 5, 10}, {0, 5, 4, 5, 11}, {0, 5, 4, 5, 12}, {0, 5, 4, 5, 14},

         {1, 4, 4, 5, 15}, {0, 4, 4, 5, 16}, {0, 4, 4, 5, 17},

         {1, 8, 4, 5, 18}, {0, 4, 4, 5, 20}}
    }, {
        // ANSI layout

        // Row 0
        {{0, 4, 4, 0, 0},
         {4, 4, 4, 0, 1}, {0, 4, 4, 0, 2}, {0, 4, 4, 0, 3}, {0, 4, 4, 0, 4},
         {2, 4, 4, 0, 6}, {0, 4, 4, 0, 7}, {0, 4, 4, 0, 8}, {0, 4, 4, 0, 9},
         {2, 4, 4, 0, 11}, {0, 4, 4, 0, 12}, {0, 4, 4, 0, 13}, {0, 4, 4, 0, 14},

         {1, 4, 4, 0, 15}, {0, 4, 4, 0, 16}, {0, 4, 4, 0, 17},

         {1, 4, 4, 0, 18}, {0, 4, 4, 0, 19}, {0, 4, 4, 0, 20}, {0, 4, 4, 0, 21}},

        // Row 1
        {{0, 4, 4, 1, 0}, {0, 4, 4, 1, 1}, {0, 4, 4, 1, 2}, {0, 4, 4, 1, 3},
         {0, 4, 4, 1, 4}, {0, 4, 4, 1, 5}, {0, 4, 4, 1, 6}, {0, 4, 4, 1, 7},
         {0, 4, 4, 1, 8}, {0, 4, 4, 1, 9}, {0, 4, 4, 1, 10}, {0, 4, 4, 1, 11},
         {0, 4, 4, 1, 12}, {0, 8, 4, 1, 14},

         {1, 4, 4, 1, 15}, {0, 4, 4, 1, 16}, {0, 4, 4, 1, 17},

         {1, 4, 4, 1, 18}, {0, 4, 4, 1, 19}, {0, 4, 4, 1, 20}, {0, 4, 4, 1, 21}},

        // Row 2
        {{0, 6, 4, 2, 0}, {0, 4, 4, 2, 1}, {0, 4, 4, 2, 2}, {0, 4, 4, 2, 3},
         {0, 4, 4, 2, 4}, {0, 4, 4, 2, 5}, {0, 4, 4, 2, 6}, {0, 4, 4, 2, 7},
         {0, 4, 4, 2, 8}, {0, 4, 4, 2, 9}, {0, 4, 4, 2, 10}, {0, 4, 4, 2, 11},
         {0, 4, 4, 2, 12}, {0, 6, 4, 2, 14},

         {1, 4, 4, 2, 15}, {0, 4, 4, 2, 16}, {0, 4, 4, 2, 17},

         {1, 4, 4, 2, 18}, {0, 4, 4, 2, 19}, {0, 4, 4, 2, 20}, {0, 4, 8, 2, 21}},

        // Row 3
        {{0, 7, 4, 3, 0}, {0, 4, 4, 3, 1}, {0, 4, 4, 3, 2}, {0, 4, 4, 3, 3},
         {0, 4, 4, 3, 4}, {0, 4, 4, 3, 5}, {0, 4, 4, 3, 6}, {0, 4, 4, 3, 7},
         {0, 4, 4, 3, 8}, {0, 4, 4, 3, 9}, {0, 4, 4, 3, 10}, {0, 4, 4, 3, 11},
         {0, 9, 4, 3, 14},

         {14, 4, 4, 3, 18}, {0, 4, 4, 3, 19}, {0, 4, 4, 3, 20}},

        // Row 4
        {{0, 9, 4, 4, 0}, {0, 4, 4, 4, 2}, {0, 4, 4, 4, 3},
         {0, 4, 4, 4, 4}, {0, 4, 4, 4, 5}, {0, 4, 4, 4, 6}, {0, 4, 4, 4, 7},
         {0, 4, 4, 4, 8}, {0, 4, 4, 4, 9}, {0, 4, 4, 4, 10}, {0, 4, 4, 4, 11},
         {0, 11, 4, 4, 14},

         {5, 4, 4, 4, 16},

         {5, 4, 4, 4, 18}, {0, 4, 4, 4, 19}, {0, 4, 4, 4, 20}, {0, 4, 8, 4, 21}},

        // Row 5
        {{0, 5, 4, 5, 0}, {0, 5, 4, 5, 1}, {0, 5, 4, 5, 2}, {0, 25, 4, 5, 6},
         {0, 5, 4, 5, 10}, {0, 5, 4, 5, 11}, {0, 5, 4, 5, 12}, {0, 5, 4, 5, 14},

         {1, 4, 4, 5, 15}, {0, 4, 4, 5, 16}, {0, 4, 4, 5, 17},

         {1, 8, 4, 5, 18}, {0, 4, 4, 5, 20}}

    }
};

KeyboardWidget::KeyboardWidget(KeyboardLayout layout, KeyboardStyle style, QWidget* parent)
    : QFrame{parent},
      _layout{layout},
      _style{style}
{
    for (auto& row: _buttons) {
        std::fill(std::begin(row), std::end(row), nullptr);
    }

    setup();
}

void KeyboardWidget::setAll(QColor const& color)
{
    for (auto& row : _buttons) {
        for (auto& col : row) {
            if (col) {
                col->setColor(color);
            }
        }
    }
}

// Size of a keyboard grid column in real pixels. Should be changed dynamically.
constexpr int COLSIZ = 8;

// How many units of columns a row is high. (4 = single button height)
constexpr int ROWSIZ = 4;

QSize KeyboardWidget::minimumSizeHint() const
{
    const int cols = 1 + (_style == KeyboardStyle::Normal ? 22 : 18);

    return QSize{cols * COLSIZ * ROWSIZ, static_cast<int>(6.2 * COLSIZ * ROWSIZ)};
}

void KeyboardWidget::setup()
{
    int const mx = _style == KeyboardStyle::Normal ? 22 : 18;

    float y_offset = 0.0;

    for (int y = 0; y < CMMK_ROWS_MAX; ++y) {
        int x_offset = 0;

        for (int x = 0; x < mx; ++x) {
            auto const& descr = layouts[(int)_layout][y][x];

            auto const ly = std::get<3>(descr);
            auto const lx = std::get<4>(descr);

            auto const gap = std::get<0>(descr);
            auto const keyWidth = std::get<1>(descr);
            auto const keyHeight = std::get<2>(descr);

            if ((keyWidth == 0 && keyHeight == 0) || lx >= mx) {
                continue;
            }

            x_offset += gap;

            auto btn = new KeyboardButton("", keyWidth, keyHeight, this);

            btn->setRow(ly);
            btn->setColumn(lx);

            auto const realHeight = keyHeight * COLSIZ;
            auto const realWidth = keyWidth * COLSIZ;

            auto const px = x_offset * COLSIZ;
            auto const py = y_offset * ROWSIZ * COLSIZ;

            x_offset += keyWidth;

            btn->move(px, py);
            btn->resize(realWidth, realHeight);
            btn->setColor(QColor{0, 0, 0});

            connect(btn, &KeyboardButton::clicked, this, [=] {
                emit clicked(btn);
            });

            _buttons[btn->row()][btn->column()] = btn;
        }

        y_offset += 1.0;

        // Add a bit of space between function keys and first character row
        if (y == 0) {
            y_offset += 0.2;
        }
    }

    setFixedSize(minimumSizeHint());
    updateGeometry();
}

KeyboardButton* KeyboardWidget::getKeyAt(int row, int col) const
{
    return _buttons[row][col];
}

void KeyboardWidget::resizeEvent(QResizeEvent* event)
{
    QFrame::resizeEvent(event);

    // TODO: Figure out how to gracefully resize buttons. Qt seems to hate
    //       having new widgets added without a layout during a resize.
}

