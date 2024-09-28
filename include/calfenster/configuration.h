#pragma once

#include <qstring.h>

#include <utility>

class QWidget;
class QCalendarWidget;

namespace calfenster {

/**
 * @brief The configuration class of our nice little tool.
 */
struct Configuration {
  /**
   * @brief Information about a clock to be displayed.
   */
  struct Clock {
    Clock(QString label, QString timezone, QString format)
        : label(std::move(label)),
          timezone(std::move(timezone)),
          format(std::move(format)) {}
    QString label;
    QString timezone;
    QString format;
  };

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
  QString locale = "";                ///< Common Locale Data Repository v45

  // Calender display options
  bool show_grid = true;

  std::vector<Clock> clocks;  ///< the clocks configured in the config file

  void ConfigureWindow(QWidget& widget) const;
  void ConfigureCalendar(QCalendarWidget& widget) const;

  bool save_on_exit = false;
};
}  // namespace calfenster
