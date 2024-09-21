#pragma once

#include <string>

class QWidget;
class QCalendarWidget;

namespace calfenster {
struct Configuration {
  Configuration() = default;
  ~Configuration() = default;

  // Window display options
  bool skip_task_bar = true;
  bool frameless_window = true;
  bool window_no_shadow = false;
  bool window_stays_on_top = true;
  bool window_stays_on_bottom = false;
  bool customize_window = true;

  // Window position
  std::string window_position = "mouse";  ///< either mouse or none

  // Calender display options
  bool show_grid = true;

  void ConfigureWindow(QWidget& widget) const;
  void ConfigureCalendar(QCalendarWidget& widget) const;
};
}  // namespace calfenster
