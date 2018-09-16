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

#include "libcmmk/libcmmk.h"

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
