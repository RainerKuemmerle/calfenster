#pragma once

#include <string>

class QWidget;

namespace calfenster {
struct Configuration {
  Configuration() = default;
  ~Configuration() = default;

  // Window display options
  bool bypass_window_manager = true;
  bool frameless_window = true;
  bool window_no_shadow = false;
  bool window_stays_on_top = true;
  bool window_stays_on_bottom = false;
  bool customize_window = true;

  // Window position
  std::string window_position = "mouse";  ///< either mouse or none

  void SetWindowFlags(QWidget& widget) const;
};
}  // namespace calfenster
