#include "calfenster/configuration.h"

#include <qnamespace.h>
#include <qwidget.h>

namespace calfenster {

void Configuration::SetWindowFlags(QWidget& widget) const {
  Qt::WindowFlags flags;
  if (bypass_window_manager) {
    // flags |= Qt::X11BypassWindowManagerHint;
    flags |= Qt::BypassWindowManagerHint;
  }
  if (frameless_window) flags |= Qt::FramelessWindowHint;
  if (window_no_shadow) flags |= Qt::NoDropShadowWindowHint;
  if (window_stays_on_top) flags |= Qt::WindowStaysOnTopHint;
  if (window_stays_on_bottom) flags |= Qt::WindowStaysOnBottomHint;
  if (customize_window) flags |= Qt::CustomizeWindowHint;

  widget.setWindowFlags(flags);
}
}  // namespace calfenster
