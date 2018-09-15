#include "mainwindow.h"

#include <QApplication>
#include <QtDebug>
#include <QCommandLineOption>
#include <QCommandLineParser>

#include <thread>
#include <chrono>

extern "C" {
#include "libcmmk.h"
}

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

  // TODO: Autodetect
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
