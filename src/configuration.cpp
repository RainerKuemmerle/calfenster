#include "calfenster/configuration.h"

#include <qcalendarwidget.h>
#include <qdebug.h>
#include <qnamespace.h>
#include <qsettings.h>
#include <qwidget.h>

#include <cstddef>

namespace {
const QString kSettingsOrg = "calfenster";
const QString kSettingsApp = "calfenster";
}  // namespace

namespace calfenster {

Configuration::Configuration() {
  QSettings settings(kSettingsOrg, kSettingsApp);
  save_on_exit = settings.allKeys().isEmpty();
  skip_task_bar = settings.value("skip_task_bar", skip_task_bar).toBool();
  frameless_window =
      settings.value("frameless_window", frameless_window).toBool();
  window_no_shadow =
      settings.value("window_no_shadow", window_no_shadow).toBool();
  window_stays_on_top =
      settings.value("window_stays_on_top", window_stays_on_top).toBool();
  window_stays_on_bottom =
      settings.value("window_stays_on_bottom", window_stays_on_bottom).toBool();
  customize_window =
      settings.value("customize_window", customize_window).toBool();
  window_position =
      settings.value("window_position", window_position).toString();
  locale = settings.value("locale", locale).toString();

  // reading clocks
  int size = settings.beginReadArray("Clocks");
  clocks.reserve(size);
  for (int i = 0; i < size; ++i) {
    settings.setArrayIndex(i);
    clocks.emplace_back(settings.value("label", "").toString(),
                        settings.value("timezone", "").toString(),
                        settings.value("format", "").toString());
  }
  settings.endArray();
}

Configuration::~Configuration() {
  if (!save_on_exit) return;
  // Create a configuration with default values
  QSettings settings(kSettingsOrg, kSettingsApp);
  settings.setValue("skip_task_bar", skip_task_bar);
  settings.setValue("frameless_window", frameless_window);
  settings.setValue("window_no_shadow", window_no_shadow);
  settings.setValue("window_stays_on_top", window_stays_on_top);
  settings.setValue("window_stays_on_bottom", window_stays_on_bottom);
  settings.setValue("customize_window", customize_window);
  if (!window_position.isEmpty())
    settings.setValue("window_position", window_position);
  if (!locale.isEmpty()) settings.setValue("locale", locale);
  if (!clocks.empty()) {
    settings.beginWriteArray("Clocks");
    for (std::size_t i = 0; i < clocks.size(); ++i) {
      settings.setArrayIndex(i);
      settings.setValue("label", clocks[i].label);
      settings.setValue("timezone", clocks[i].timezone);
      settings.setValue("format", clocks[i].format);
    }
    settings.endArray();
  }
}

void Configuration::ConfigureWindow(QWidget& widget) const {
  Qt::WindowFlags flags;
  if (skip_task_bar) flags |= Qt::SplashScreen;
  if (frameless_window) flags |= Qt::FramelessWindowHint;
  if (window_no_shadow) flags |= Qt::NoDropShadowWindowHint;
  if (window_stays_on_top) flags |= Qt::WindowStaysOnTopHint;
  if (window_stays_on_bottom) flags |= Qt::WindowStaysOnBottomHint;
  if (customize_window) flags |= Qt::CustomizeWindowHint;

  widget.setWindowFlags(flags);

  if (window_position == "mouse") {
    QPoint cursor_pos = QCursor::pos();
    cursor_pos.setY(cursor_pos.y() - widget.sizeHint().height());
    widget.move(cursor_pos);
  }
}

void Configuration::ConfigureCalendar(QCalendarWidget& widget) const {
  widget.setGridVisible(show_grid);
}

}  // namespace calfenster
