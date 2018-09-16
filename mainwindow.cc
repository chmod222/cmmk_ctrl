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
#include "mainwindow.hh"
#include "ui_mainwindow.h"

#include "widgets/KeyboardWidget.hh"
#include "widgets/KeyboardButton.hh"

#include "widgets/SpeedSlider.hh"
#include "widgets/ColorButton.hh"

#include <QComboBox>
#include <QPushButton>
#include <QLabel>
#include <QTabWidget>

#include <QtSvg>

#include <iostream>

#include "libcmmk/libcmmk.h"

static char const* effectNames[256] = { nullptr };

/*
 * Little helpers
 */
namespace {

QColor qcolorFromCmmk(struct rgb const& rgb)
{
    return QColor(rgb.R, rgb.G, rgb.B);
}

struct rgb cmmkFromQcolor(QColor const& qcol)
{
    return {
        static_cast<unsigned char>(qcol.redF() * 255),
        static_cast<unsigned char>(qcol.greenF() * 255),
        static_cast<unsigned char>(qcol.blueF() * 255)
    };
}

}


MainWindow::MainWindow(struct cmmk* dev, QWidget* parent) :
    QMainWindow(parent),
    _dev{dev},
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    effectNames[CMMK_EFFECT_FULLY_LIT] = "Full Bright";
    effectNames[CMMK_EFFECT_BREATHE] = "Color Breathe";
    effectNames[CMMK_EFFECT_CYCLE] = "Color Cycle";
    effectNames[CMMK_EFFECT_SINGLE] = "Single Key";
    effectNames[CMMK_EFFECT_WAVE] = "Color Wave";
    effectNames[CMMK_EFFECT_RIPPLE] = "Ripple";
    effectNames[CMMK_EFFECT_CROSS] = "Cross";
    effectNames[CMMK_EFFECT_RAINDROPS] = "Raindrops";
    effectNames[CMMK_EFFECT_STARS] = "Starfield";
    effectNames[CMMK_EFFECT_SNAKE] = "Snake Game";
    effectNames[CMMK_EFFECT_CUSTOMIZED] = "Customized Colors";
    effectNames[CMMK_EFFECT_MULTILAYER] = "Multilayer/Mosaic";
    effectNames[CMMK_EFFECT_OFF] = "Off";

    _profileSelector = new QComboBox{};

    _profileSelector->addItem("Profile 1", 1);
    _profileSelector->addItem("Profile 2", 2);
    _profileSelector->addItem("Profile 3", 3);
    _profileSelector->addItem("Profile 4", 4);

    connect(
        _profileSelector, QOverload<int>::of(&QComboBox::currentIndexChanged),
        this, [&] {
            switchProfile(_profileSelector->currentData().value<int>());
        });

    _effectSelector = new QComboBox{};

    for (auto& effect : { 
            CMMK_EFFECT_FULLY_LIT, CMMK_EFFECT_BREATHE, CMMK_EFFECT_CYCLE,
            CMMK_EFFECT_SINGLE, CMMK_EFFECT_WAVE, CMMK_EFFECT_RIPPLE,
            CMMK_EFFECT_CROSS, CMMK_EFFECT_RAINDROPS, CMMK_EFFECT_STARS,
            CMMK_EFFECT_SNAKE, CMMK_EFFECT_CUSTOMIZED, CMMK_EFFECT_MULTILAYER,
            CMMK_EFFECT_OFF }) {

        _effectSelector->addItem(effectNames[effect], effect);
    }

    connect(
        _effectSelector, QOverload<int>::of(&QComboBox::currentIndexChanged),
        this, [&] {
            switchEffect(static_cast<cmmk_effect_id>(
                _effectSelector->currentData().value<int>()));
        });

    _colorSelector = new ColorButton{};
    _colorSelector->setMinimumWidth(50);
    _colorSelector->setMaximumHeight(24);

    connect(_colorSelector, &ColorButton::colorSelected, this, [=] (QColor selection) {
        _activeColor = selection;
    });

    _overrideCheckbox = new QCheckBox{"Manual Control"};

    connect(
        _overrideCheckbox, &QCheckBox::stateChanged,
        this, &MainWindow::manualOverrideStateChange);

    ui->mainToolBar->addWidget(_overrideCheckbox);

    ui->mainToolBar->addSeparator();

    ui->mainToolBar->addWidget(new QLabel{"Active Color: "});
    ui->mainToolBar->addWidget(_colorSelector);

    ui->mainToolBar->addSeparator();

    ui->mainToolBar->addWidget(new QLabel{"Active Profile: "});
    ui->mainToolBar->addWidget(_profileSelector);

    ui->mainToolBar->addSeparator();

    ui->mainToolBar->addWidget(new QLabel{"Effect: "});
    ui->mainToolBar->addWidget(_effectSelector);

    auto saveButton = new QPushButton{"Save"};

    ui->mainToolBar->addWidget(saveButton);

    connect(saveButton, &QPushButton::clicked, this, [this] {
        cmmk_set_enabled_effects(_dev, _enabledEffects.data(), _enabledEffects.size());
        cmmk_save_active_profile(_dev);
    });

    _keyboardWidget = new KeyboardWidget{
        _dev->layout_type == CMMK_LAYOUT_TYPE_ISO
            ? KeyboardLayout::ISO
            : KeyboardLayout::ANSI,

        _dev->product_type == CMMK_PRODUCT_MASTERKEYS_PRO_S
            ? KeyboardStyle::TKL
            : KeyboardStyle::Normal,

        ui->centralWidget
    };

    ui->centralLayout->addWidget(_keyboardWidget);

    connect(_keyboardWidget, &KeyboardWidget::clicked, this, [=] (KeyboardButton* btn) {
        btn->setColor(_activeColor);

        struct rgb col{cmmkFromQcolor(_activeColor)};

        if (_manual) {
            cmmk_set_single_key(_dev, btn->row(), btn->column(), &col);
        } else {
            _modifiedCustomColors.data[btn->row()][btn->column()] = col;

            cmmk_set_customized_leds(_dev, &_modifiedCustomColors);
        }
    });

    connect(this, &MainWindow::updateEffect, this, [this] (auto effId, auto eff) {
        cmmk_set_effect(_dev, effId, &eff);
    });

    /*
     * Create the effect configuration treeview
     */
    setupEffectsList();

    /* Set profile dropdown to current profile */
    int prof;

    cmmk_get_active_profile(_dev, &prof);

    _profileSelector->blockSignals(true);
        _profileSelector->setCurrentIndex(
            _profileSelector->findData(prof));
    _profileSelector->blockSignals(false);

    /* And initialize the widgets */
    pullProfileData();
}

void MainWindow::switchProfile(int prof)
{
    cmmk_set_control_mode(_dev, CMMK_PROFILE_CUSTOMIZATION);
    cmmk_set_active_profile(_dev, prof);

    this->setDisabled(true);

    // We have to wait a tiny bit after switching profiles
    QTime dieTime= QTime::currentTime().addMSecs(200);

    while (QTime::currentTime() < dieTime) {
        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
    }

    pullProfileData();

    this->setDisabled(false);

    cmmk_set_control_mode(_dev, CMMK_EFFECT);
}

void MainWindow::switchEffect(enum cmmk_effect_id eff)
{
    cmmk_set_active_effect(_dev, eff);

    _currentEffect = eff;
    restoreEffectPreview();
}

void MainWindow::pullProfileData()
{
    cmmk_set_control_mode(_dev, CMMK_PROFILE_CUSTOMIZATION);

    /*
     * Get current effect
     */
    cmmk_get_active_effect(_dev, &_currentEffect);

    int idx = _effectSelector->findData(static_cast<int>(_currentEffect));

    if (idx != -1) {
        _effectSelector->blockSignals(true);
            _effectSelector->setCurrentIndex(idx);
        _effectSelector->blockSignals(false);
    }

    size_t enabledEffectCount;
    enum cmmk_effect_id enabledEffects[effectCount];

    cmmk_get_enabled_effects(_dev, enabledEffects, effectCount, &enabledEffectCount); 

    _enabledEffects.clear();
    _enabledEffects.insert(
        std::begin(_enabledEffects),
        std::begin(enabledEffects),
        std::begin(enabledEffects) + enabledEffectCount);

    /*
     * Update effect config cache 
     */
    cmmk_get_customized_leds(_dev, &_currentCustomizedColors);

    _modifiedCustomColors = _currentCustomizedColors;

    for (size_t i = 0; i < effectCount; ++i) {
        auto eff = static_cast<enum cmmk_effect_id>(i);

        cmmk_set_active_effect(_dev, eff);
        cmmk_get_effect(_dev, eff, &_effectSettings[i]);

        emit effectDownloaded(eff, _effectSettings[i]);
    }

    cmmk_set_active_effect(_dev, _currentEffect);

    cmmk_set_control_mode(_dev, CMMK_EFFECT);

    restoreEffectPreview();
}

void MainWindow::setupEffectsList()
{
    auto effectsList = new QTreeWidget{};

    effectsList->setColumnCount(2);
    effectsList->setHeaderLabels({"Parameter", "Setting"});

    /*
     * I like lambdas.
     */
    auto addEffect = [&] (enum cmmk_effect_id id, auto extra) {
        auto effect = new QTreeWidgetItem{};

        effect->setText(0, effectNames[id]);
        effect->setData(0, Qt::UserRole, id);

        effectsList->addTopLevelItem(effect);

        extra(effect);
    };

    auto addParam = [&] (auto effect, QString name, QWidget* widget) {
        auto p = new QTreeWidgetItem{};

        p->setText(0, name);

        effect->addChild(p);
        effectsList->setItemWidget(p, 1, widget);
    };

    auto addColor = [&] (auto effect, QString name, struct rgb cmmk_generic_effect::*ptr) {
        auto colorPicker = new ColorButton{};

        colorPicker->setMaximumHeight(22);
        addParam(effect, name, colorPicker);

        connect(
            this, &MainWindow::effectDownloaded,
            this, [=] (auto eff, auto effp) {
                if (effect->data(0, Qt::UserRole).toInt() != eff) {
                    return;
                }

                colorPicker->blockSignals(true);
                colorPicker->setSelectedColor(qcolorFromCmmk(effp.*ptr));
                colorPicker->blockSignals(false);
            });

        connect(
            colorPicker, &ColorButton::colorSelected,
            this, [=] (auto sel) {
                int eid = effect->data(0, Qt::UserRole).toInt();

                _effectSettings[eid].*ptr = cmmkFromQcolor(sel);

                emit updateEffect(static_cast<cmmk_effect_id>(eid), _effectSettings[eid]);
            });
    };

    auto addSpeed = [&] (auto effect, QString name, int cmmk_generic_effect::*ptr) {
        auto slider = new SpeedSlider{};

        addParam(effect, name, slider);

        connect(
            this, &MainWindow::effectDownloaded,
            this, [this, effect, slider, ptr] (auto eff, auto effp) {
                if (effect->data(0, Qt::UserRole).toInt() == eff) {
                    slider->blockSignals(true);
                    slider->setValue(effp.*ptr);
                    slider->blockSignals(false);
                }
            });

        connect(
            slider, &SpeedSlider::valueChanged,
            this, [=] {
                int eid = effect->data(0, Qt::UserRole).toInt();

                _effectSettings[eid].*ptr = slider->value();

                emit updateEffect(
                    static_cast<enum cmmk_effect_id>(eid), _effectSettings[eid]);
            });
    };

    auto addEnabled = [&] (auto effect) {
        auto checkbox = new QCheckBox{};

        addParam(effect, "Enabled", checkbox);

        connect(
            this, &MainWindow::effectDownloaded,
            this, [=] (auto eff, auto effp) {
                (void)effp;

                if (effect->data(0, Qt::UserRole).toInt() == eff) {
                    auto p = std::find(
                        std::begin(_enabledEffects),
                        std::end(_enabledEffects),
                        eff);

                    checkbox->blockSignals(true);

                    if (p != std::end(_enabledEffects)) {
                        checkbox->setCheckState(Qt::Checked);
                    } else {
                        checkbox->setCheckState(Qt::Unchecked);
                    }

                    checkbox->blockSignals(false);
                }
            });

        connect(
            checkbox, &QCheckBox::stateChanged,
            this, [=] {
                auto eid = static_cast<enum cmmk_effect_id>(
                    effect->data(0, Qt::UserRole).toInt());

                if (checkbox->checkState() == Qt::Checked) {
                    _enabledEffects.push_back(eid);
                } else {
                    auto p = std::find(
                        std::begin(_enabledEffects),
                        std::end(_enabledEffects),
                        eid);

                    if (p != std::end(_enabledEffects)) {
                        _enabledEffects.erase(p);
                    }
                }
            });
    };

    addEffect(CMMK_EFFECT_FULLY_LIT, [&] (auto effect) {
        addEnabled(effect);
        addColor(effect, "Color", &cmmk_generic_effect::color1); // c1
    });

    addEffect(CMMK_EFFECT_BREATHE, [&] (auto effect) {
        addEnabled(effect);
        addSpeed(effect, "Speed", &cmmk_generic_effect::p1); // p1
        addColor(effect, "Color", &cmmk_generic_effect::color1); // c1
    });

    addEffect(CMMK_EFFECT_CYCLE, [&] (auto effect) {
        addEnabled(effect);
        addSpeed(effect, "Speed", &cmmk_generic_effect::p1); // p1
    });

    addEffect(CMMK_EFFECT_SINGLE, [&] (auto effect) {
        addEnabled(effect);
        addSpeed(effect, "Speed", &cmmk_generic_effect::p1); // p1

        addColor(effect, "Active", &cmmk_generic_effect::color1); // c1
        addColor(effect, "Rest", &cmmk_generic_effect::color2); // c2
    });

    addEffect(CMMK_EFFECT_WAVE, [&] (auto effect) {
        auto directionCombo = new QComboBox{};

        directionCombo->addItem("Left to right", CMMK_LEFT_TO_RIGHT);
        directionCombo->addItem("Right to left", CMMK_RIGHT_TO_LEFT);
        directionCombo->addItem("Back to front", CMMK_BACK_TO_FRONT);
        directionCombo->addItem("Front to back", CMMK_FRONT_TO_BACK);

        int cmmk_generic_effect::*ptr = &cmmk_generic_effect::p2;

        connect(
            this, &MainWindow::effectDownloaded,
            this, [=] (auto eff, auto effp) {
                if (effect->data(0, Qt::UserRole).toInt() == eff) {
                    int idx = directionCombo->findData(static_cast<int>(effp.*ptr));

                    if (idx != -1) {
                        directionCombo->blockSignals(true);
                        directionCombo->setCurrentIndex(idx);
                        directionCombo->blockSignals(false);
                    }
                }
            });

        connect(
            directionCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, [=] {
                int eid = effect->data(0, Qt::UserRole).toInt();

                _effectSettings[eid].*ptr = directionCombo->currentData().value<int>();

                emit updateEffect(static_cast<cmmk_effect_id>(eid), _effectSettings[eid]);
            });

        addEnabled(effect);
        addSpeed(effect, "Speed", &cmmk_generic_effect::p1);
        addParam(effect, "Direction", directionCombo);

        addColor(effect, "Tint", &cmmk_generic_effect::color1);
    });

    addEffect(CMMK_EFFECT_RIPPLE, [&] (auto effect) {
        auto randomized = new QCheckBox{};

        int cmmk_generic_effect::*ptr = &cmmk_generic_effect::p2;

        connect(
            this, &MainWindow::effectDownloaded,
            this, [=] (auto eff, auto effp) {
                if (effect->data(0, Qt::UserRole).toInt() == eff) {
                    randomized->blockSignals(true);

                    if (effp.*ptr != CMMK_RIPPLE_GIVEN_COLOR) {
                        randomized->setCheckState(Qt::Checked);
                    } else {
                        randomized->setCheckState(Qt::Unchecked);
                    }

                    randomized->blockSignals(false);
                }
            });

        connect(
            randomized, &QCheckBox::stateChanged,
            this, [=] (int newState) {
                int eid = effect->data(0, Qt::UserRole).toInt();

                _effectSettings[eid].*ptr = newState == Qt::Checked
                    ? CMMK_RIPPLE_RANDOM_COLOR
                    : CMMK_RIPPLE_GIVEN_COLOR;

                emit updateEffect(static_cast<cmmk_effect_id>(eid), _effectSettings[eid]);
            });

        addEnabled(effect);
        addSpeed(effect, "Speed", &cmmk_generic_effect::p1);
        addParam(effect, "Random Color", randomized);

        addColor(effect, "Ripple Color", &cmmk_generic_effect::color1);
        addColor(effect, "Rest", &cmmk_generic_effect::color2);
    });

    addEffect(CMMK_EFFECT_CROSS, [&] (auto effect) {
        addEnabled(effect);
        addSpeed(effect, "Speed", &cmmk_generic_effect::p1);

        addColor(effect, "Active", &cmmk_generic_effect::color1);
        addColor(effect, "Rest", &cmmk_generic_effect::color2);
    });

    addEffect(CMMK_EFFECT_RAINDROPS, [&] (auto effect) {
        addEnabled(effect);
        addSpeed(effect, "Speed", &cmmk_generic_effect::p1);
        addSpeed(effect, "Interval", &cmmk_generic_effect::p3);

        addColor(effect, "Active", &cmmk_generic_effect::color1);
        addColor(effect, "Rest", &cmmk_generic_effect::color2);
    });

    addEffect(CMMK_EFFECT_STARS, [&] (auto effect) {
        addEnabled(effect);
        addSpeed(effect, "Speed", &cmmk_generic_effect::p1);
        addSpeed(effect, "Interval", &cmmk_generic_effect::p3);

        addColor(effect, "Active", &cmmk_generic_effect::color1);
        addColor(effect, "Rest", &cmmk_generic_effect::color2);
    });

    addEffect(CMMK_EFFECT_SNAKE, [&] (auto effect) {
        addEnabled(effect);
        addSpeed(effect, "Speed", &cmmk_generic_effect::p1);
        addColor(effect, "Background", &cmmk_generic_effect::color2);
    });

    ui->centralLayout->addWidget(effectsList);
}

void MainWindow::applyColorMatrix(struct cmmk_color_matrix const& colors)
{
    for (int i = 0; i < CMMK_ROWS_MAX; ++i) {
        for (int j = 0; j < CMMK_COLS_MAX; ++j) {
            if (auto k = _keyboardWidget->getKeyAt(i, j)) {
                k->setColor(qcolorFromCmmk(colors.data[i][j]));
            }
        }
    }
}

void MainWindow::restoreEffectPreview()
{
    if (_currentEffect != CMMK_EFFECT_MULTILAYER) {
        applyColorMatrix(_modifiedCustomColors);
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_action_Close_triggered()
{
    this->close();
}

void MainWindow::manualOverrideStateChange(int state)
{
    _manual = state == Qt::Checked;

    _profileSelector->setDisabled(_manual);
    _effectSelector->setDisabled(_manual);

    if (state == Qt::Checked) {
        _keyboardWidget->setAll(QColor{0, 0, 0});

        cmmk_set_control_mode(_dev, CMMK_MANUAL);

    } else {
        cmmk_set_control_mode(_dev, CMMK_EFFECT);
        switchEffect(_currentEffect);
        restoreEffectPreview();
    }
}
