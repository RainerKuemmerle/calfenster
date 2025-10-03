#pragma once

#include <qstring.h>

#include <array>
#include <set>
#include <utility>
#include <vector>

class QWidget;
class QCalendarWidget;

namespace calfenster {

class ClockNanny;
class ClipboardNanny;

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

  struct FontConfig {
    int size = 0;
    QString family = "";
    QString fg = "";
    QString bg = "";
  };

  struct WeekdayConfig {
    WeekdayConfig();
    FontConfig font;
    std::set<int> days;
  };

  struct X11WindowOptions {
    bool skip_task_bar = true;
    bool frameless_window = true;
    bool window_no_shadow = false;
    bool window_stays_on_top = true;
    bool window_stays_on_bottom = false;
    bool customize_window = true;
    // Window position
    QString window_position = "mouse";  ///< either mouse or none
  };

  struct WaylandWindowOptions {
    QString anchors = "AnchorBottom|AnchorRight";
    QString layer = "LayerTop";
    std::array<int, 4> margins = {0, 0, 0, 0};
  };

  struct ClipboardConfig {
    QString format = "dd.MM.yyyy";
  };

  Configuration();
  ~Configuration();

  // Window display options
  X11WindowOptions x11_options;
  WaylandWindowOptions wayland_options;

  // font
  FontConfig calendar_font;
  FontConfig header_font;
  FontConfig clock_font;
  WeekdayConfig weekday_font;

  // clipboard interaction
  ClipboardConfig clipboard_options;

  QString locale = "";  ///< Common Locale Data Repository v45

  // Calender display options
  bool show_grid = true;
  bool show_iso_week = true;
  QString horizontal_header = "short";

  std::vector<Clock> clocks;  ///< the clocks configured in the config file

  void ConfigureWindow(QWidget& widget) const;
  void ConfigureCalendar(QCalendarWidget& widget) const;
  void ConfigureClockNanny(ClockNanny& nanny) const;
  void ConfigureClipboardNanny(ClipboardNanny& nanny) const;

  bool save_on_exit = false;
};
}  // namespace calfenster
