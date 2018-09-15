#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <QPushButton>
#include <QSlider>
#include <QCheckBox>
#include <QComboBox>

#include <vector>

#include "widgets/KeyboardWidget.hh"
#include "widgets/ColorButton.hh"

extern "C" {
#include "libcmmk.h"
}

namespace Ui {
  class MainWindow;
}


class MainWindow : public QMainWindow
{
  Q_OBJECT

public:
  explicit MainWindow(struct cmmk* dev, QWidget *parent = nullptr);
  virtual ~MainWindow();

private slots:
  void on_action_Close_triggered();
  void manualOverrideStateChange(int state);

signals:
  // Used by the effect widgets
  void effectDownloaded(enum cmmk_effect_id id, struct cmmk_generic_effect data);
  void updateEffect(enum cmmk_effect_id id, struct cmmk_generic_effect data);

private:
  void switchProfile(int prof = 0);
  void switchEffect(enum cmmk_effect_id eff);

  void pullProfileData();
  void writeProfileData();

  void setupEffectsList();

  void applyColorMatrix(struct cmmk_color_matrix const& colors);
  void restoreEffectPreview();

private:
  QColor _activeColor = QColor{255, 255, 255};

  struct cmmk *_dev;

  Ui::MainWindow *ui;

  QCheckBox* _overrideCheckbox;
  ColorButton* _colorSelector;
  QComboBox* _profileSelector;
  QComboBox* _effectSelector;

  KeyboardWidget* _keyboardWidget;

  bool _manual = false;

  static constexpr size_t effectCount = 10;

  // Max customized effect settings go up to 09, snake
  struct cmmk_generic_effect _effectSettings[effectCount];
  enum cmmk_effect_id _currentEffect;

  std::vector<enum cmmk_effect_id> _enabledEffects;

  struct cmmk_color_matrix _currentCustomizedColors;
  struct cmmk_color_matrix _modifiedCustomColors;
};

#endif // MAINWINDOW_H
