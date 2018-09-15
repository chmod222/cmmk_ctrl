#ifndef SPEEDSLIDER_HH
#define SPEEDSLIDER_HH

#include <QSlider>

/*
 * Because the firmware likes speed values going backwards from something over 0x40 up to something
 * around 0x10 rather than considering speed faster the higher the number it's given is, we'll
 * write our own slider that works slightly backwards and internally thinks it's going from 0 - 100.
 */
class SpeedSlider : public QSlider
{
    Q_OBJECT

public:
    explicit SpeedSlider(QWidget* parent = nullptr);
    virtual ~SpeedSlider() override;

    virtual int value() const;
    virtual void setValue(int value);

private:
    int const _speedMax;
    int const _speedMin;
};

#endif
