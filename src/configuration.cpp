#include "calfenster/configuration.h"

#include <qcalendarwidget.h>
#include <qnamespace.h>
#include <qwidget.h>

namespace calfenster {

void Configuration::ConfigureWindow(QWidget& widget) const {
  Qt::WindowFlags flags;
  if (bypass_window_manager) {
    // flags |= Qt::X11BypassWindowManagerHint;
    // flags |= Qt::Dialog;
    flags |= Qt::BypassWindowManagerHint;
  }
  if (frameless_window) flags |= Qt::FramelessWindowHint;
  if (window_no_shadow) flags |= Qt::NoDropShadowWindowHint;
  if (window_stays_on_top) flags |= Qt::WindowStaysOnTopHint;
  if (window_stays_on_bottom) flags |= Qt::WindowStaysOnBottomHint;
  if (customize_window) flags |= Qt::CustomizeWindowHint;

  widget.setWindowFlags(flags);

  if (window_position == "mouse") {
    widget.move(widget.mapFromGlobal(QCursor::pos()));
  }
}

void Configuration::ConfigureCalendar(QCalendarWidget& widget) const {
  widget.setGridVisible(show_grid);
}

}  // namespace calfenster
