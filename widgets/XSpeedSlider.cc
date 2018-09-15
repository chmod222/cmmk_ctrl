#include "XSpeedSlider.hh"

XSpeedSlider::XSpeedSlider(QWidget* parent)
    : QSlider{Qt::Horizontal, parent},
      _speedMax{0x10},
      _speedMin{0x45}
{
    setMinimum(0);
    setMaximum(10);

    setTickInterval(1);
    setTickPosition(TickPosition::TicksBelow);
}

XSpeedSlider::~XSpeedSlider()
{
}

float lerp(float v0, float v1, float t) {
  return (1 - t) * v0 + t * v1;
}

float ilerp(float t, float v0, float v1) {
  return std::abs((t - v0) / (v1 - v0));
}

int XSpeedSlider::value() const
{
    return static_cast<int>(
        lerp(_speedMin, _speedMax, QSlider::value() / static_cast<float>(maximum())));
}

void XSpeedSlider::setValue(int value)
{
    QSlider::setValue(
        static_cast<int>(ilerp(value, _speedMin, _speedMax) * maximum()));
}
