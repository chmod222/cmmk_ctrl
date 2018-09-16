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

#include <QApplication>
#include <QtDebug>
#include <QCommandLineOption>
#include <QCommandLineParser>

#include <thread>
#include <chrono>

#include "libcmmk/libcmmk.h"

using namespace std::literals::chrono_literals;

int main(int argc, char *argv[])
{
  QApplication a(argc, argv);

  QCommandLineParser p;

  struct cmmk state;

  QCoreApplication::setApplicationName("cmmk_ctrl");
  QCoreApplication::setApplicationVersion("0.1");

  p.addHelpOption();
  p.addVersionOption();

  p.process(a);

  int foundProduct;

  if (cmmk_find_device(&foundProduct) == 0) {
    qDebug("Found USB PID %04x", foundProduct);
  }

  if (cmmk_attach(&state, foundProduct, -1) != 0) {
    qWarning() <<
      "Failed to attach to MasterKeys control interface.  Please make sure you are either running "
      "as root or have the required permissions";

    return 1;
  }

  char const* layoutNames[] = { "ANSI", "ISO" };
  char const* productNames[] = { "MasterKeys Pro L", "MasterKeys Pro S", "MasterKeys MK750" };

  qDebug("Successfully attached to %s (%s layout)",
    productNames[state.product_type],
    layoutNames[state.layout_type]);

  MainWindow w{&state};
  w.show();

  int r = a.exec();

  cmmk_set_control_mode(&state, CMMK_EFFECT);

  /* Reload current profile to clear any lingering settings */
  int prof;
  cmmk_get_active_profile(&state, &prof);
  cmmk_set_active_profile(&state, prof);

  std::this_thread::sleep_for(200ms);

  cmmk_set_control_mode(&state, CMMK_FIRMWARE);

  cmmk_detach(&state);

  return r;
}
