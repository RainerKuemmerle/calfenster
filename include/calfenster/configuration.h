#pragma once

#include <qstring.h>

class QWidget;
class QCalendarWidget;

namespace calfenster {
struct Configuration {
  Configuration();
  ~Configuration();

  // Window display options
  bool skip_task_bar = true;
  bool frameless_window = true;
  bool window_no_shadow = false;
  bool window_stays_on_top = true;
  bool window_stays_on_bottom = false;
  bool customize_window = true;

  // Window position
  QString window_position = "mouse";  ///< either mouse or none

  // Calender display options
  bool show_grid = true;

  void ConfigureWindow(QWidget& widget) const;
  void ConfigureCalendar(QCalendarWidget& widget) const;

  bool save_on_exit = false;
};
}  // namespace calfenster
