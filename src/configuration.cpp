#include "calfenster/configuration.h"

#include <qapplication.h>
#include <qbrush.h>
#include <qcalendarwidget.h>
#include <qcolor.h>
#include <qdatetime.h>
#include <qdebug.h>
#include <qfont.h>
#include <qglobal.h>
#include <qlist.h>
#include <qlogging.h>
#include <qmargins.h>
#include <qnamespace.h>
#include <qobjectdefs.h>
#include <qsettings.h>
#include <qtextformat.h>
#include <qwidget.h>
#include <qwindow.h>
#include <qwindowdefs.h>

#include <QMetaEnum>
#include <array>
#include <cstddef>
#include <regex>
#include <set>
#include <vector>

#include "calfenster/clock_nanny.h"
#include "calfenster/config.h"

#ifdef HAVE_LAYER_SHELL
#include <LayerShellQt/window.h>
#endif

namespace {
const QString kSettingsOrg = "calfenster";
const QString kSettingsApp = "calfenster";

constexpr std::array<Qt::DayOfWeek, 7> kWeekdays = {
    Qt::Monday, Qt::Tuesday,  Qt::Wednesday, Qt::Thursday,
    Qt::Friday, Qt::Saturday, Qt::Sunday};
}  // namespace

namespace calfenster {

namespace {
/**
 * @brief Obtain a singleton to Qt's default font.
 *
 * Wrapped into a function to delay initialization of the singleton to the first
 * call.
 *
 * @return const QFont& Qt's default font.
 */
const QFont& DefaultFont() {
  static const QFont kDefaultFont = qApp->font();
  return kDefaultFont;
}

//! Converts #RRGGBB to a QColor
QColor ToColor(const std::string& color_str) {
  const std::regex pattern("#([0-9a-fA-F]{6})");
  std::smatch match;
  if (!std::regex_match(color_str, match, pattern)) {
    return {};
  }
  unsigned int r, g, b;                                   // NOLINT
  sscanf(match.str(1).c_str(), "%2x%2x%2x", &r, &g, &b);  // NOLINT
  return {static_cast<int>(r), static_cast<int>(g), static_cast<int>(b)};
}

QStringList ToStringList(const std::set<int>& ints) {
  QStringList result;
  for (auto i : ints) {
    result.append(QString::number(i));
  }
  return result;
}

QStringList ToStringList(const std::array<int, 4>& ints) {
  QStringList result;
  for (auto i : ints) {
    result.append(QString::number(i));
  }
  return result;
}

std::set<int> ToIntSet(const QStringList& list) {
  std::set<int> result;
  for (const auto& s : list) {
    bool ok = true;
    int i = s.toInt(&ok);
    if (ok) {
      result.insert(i);
    }
  }
  return result;
}

template <int N>
std::array<int, N> ToIntArray(const QStringList& list) {
  std::array<int, N> result{};
  int max_idx = std::min(static_cast<int>(list.size()), N);
  for (int i = 0; i < max_idx; ++i) {
    bool ok = true;
    int number = list[i].toInt(&ok);
    if (ok) {
      result[i] = number;  // NOLINT
    }
  }
  return result;
}

QFont CreateFont(const Configuration::FontConfig& config) {
  const QFont font(
      !config.family.isEmpty() ? config.family : DefaultFont().family(),
      config.size > 0 ? config.size : DefaultFont().pointSize());
  return font;
};

QTextCharFormat UpdateFormat(const QTextCharFormat& format,
                             const Configuration::FontConfig& config) {
  QTextCharFormat result = format;
  result.setFont(CreateFont(config));
  if (!config.fg.isEmpty()) {
    QColor color = ToColor(config.fg.toStdString());
    if (color.isValid()) {
      result.setForeground(QBrush(color));
    }
  }
  if (!config.bg.isEmpty()) {
    QColor color = ToColor(config.bg.toStdString());
    if (color.isValid()) {
      result.setBackground(QBrush(color));
    }
  }
  return result;
}

QCalendarWidget::HorizontalHeaderFormat ToHorizontalHeaderFormat(
    const QString& s) {
  const QString header = s.toLower().trimmed();
  if (header == "single") {
    return QCalendarWidget::SingleLetterDayNames;
  }
  if (header == "short") {
    return QCalendarWidget::ShortDayNames;
  }
  if (header == "long") {
    return QCalendarWidget::LongDayNames;
  }
  return QCalendarWidget::NoHorizontalHeader;
}

#ifdef HAVE_LAYER_SHELL
template <typename T>
T StringToEnum(QMetaEnum meta_enum, const QString& value) {
  bool ok = true;
  T result = T(meta_enum.keyToValue(qPrintable(value), &ok));
  if (!ok) {
    qWarning() << "Unknown enum value " << value;
    return T{};
  }
  return result;
}

template <typename T>
T FlaggedStringToEnum(QMetaEnum meta_enum, const QString& str) {
  T ret = {};
  const auto splitted = str.split(QLatin1Char('|'), Qt::SkipEmptyParts);
  for (const auto& value : splitted) {
    ret |= StringToEnum<T>(meta_enum, value);
  }
  return ret;
}

const auto kAnchorMetaEnum =
    QMetaEnum::fromType<LayerShellQt::Window::Anchor>();
const auto kLayerMetaEnum = QMetaEnum::fromType<LayerShellQt::Window::Layer>();
#endif

}  // namespace

Configuration::WeekdayConfig::WeekdayConfig() {
  font.fg = "#ff0000";
  days = {Qt::Saturday, Qt::Sunday};
}

Configuration::Configuration() {
  QSettings settings(kSettingsOrg, kSettingsApp);
  save_on_exit = settings.allKeys().isEmpty();

  // Calendar display
  show_grid = settings.value("show_grid", show_grid).toBool();
  show_iso_week = settings.value("show_iso_week", show_iso_week).toBool();
  horizontal_header =
      settings.value("horizontal_header", horizontal_header).toString();

  locale = settings.value("locale", locale).toString();

  // Window settings
  settings.beginGroup("X11");
  x11_options.skip_task_bar =
      settings.value("skip_task_bar", x11_options.skip_task_bar).toBool();
  x11_options.frameless_window =
      settings.value("frameless_window", x11_options.frameless_window).toBool();
  x11_options.window_no_shadow =
      settings.value("window_no_shadow", x11_options.window_no_shadow).toBool();
  x11_options.window_stays_on_top =
      settings.value("window_stays_on_top", x11_options.window_stays_on_top)
          .toBool();
  x11_options.window_stays_on_bottom =
      settings
          .value("window_stays_on_bottom", x11_options.window_stays_on_bottom)
          .toBool();
  x11_options.customize_window =
      settings.value("customize_window", x11_options.customize_window).toBool();
  x11_options.window_position =
      settings.value("window_position", x11_options.window_position).toString();
  settings.endGroup();

  settings.beginGroup("Wayland");
  wayland_options.anchors =
      settings.value("anchors", wayland_options.anchors).toString();
  wayland_options.layer =
      settings.value("layer", wayland_options.layer).toString();
  wayland_options.margins = ToIntArray<4>(
      settings.value("margins", ToStringList(wayland_options.margins))
          .toStringList());
  settings.endGroup();

  auto read_font_config = [&settings](FontConfig& config) {
    config.size = settings.value("font_size", config.size).toInt();
    config.family = settings.value("font_family", config.family).toString();
    config.fg = settings.value("fg", config.fg).toString();
    config.bg = settings.value("bg", config.bg).toString();
  };

  // Fonts
  settings.beginGroup("Calendar");
  read_font_config(calendar_font);
  settings.endGroup();
  settings.beginGroup("Header");
  read_font_config(header_font);
  settings.endGroup();
  settings.beginGroup("Clocks");
  read_font_config(clock_font);
  settings.endGroup();

  // weekday by default uses calender family and size
  weekday_font.font.family = calendar_font.family;
  weekday_font.font.size = calendar_font.size;

  // Weekday Fonts
  settings.beginGroup("Weekdays");
  weekday_font.days = ToIntSet(
      settings.value("days", ToStringList(weekday_font.days)).toStringList());
  read_font_config(weekday_font.font);
  settings.endGroup();

  // reading clocks
  int size = settings.beginReadArray("Clocks");
  clocks.reserve(size);
  for (int i = 0; i < size; ++i) {
    settings.setArrayIndex(i);
    clocks.emplace_back(
        settings.value("label", "").toString(),
        settings.value("tz", "").toString(),
        settings.value("format", "yyyy-MM-dd hh:mm").toString());
  }
  settings.endArray();
}

Configuration::~Configuration() {
  if (!save_on_exit) {
    return;
  }
  // Create a configuration with default values
  QSettings settings(kSettingsOrg, kSettingsApp);

  // Calendar display
  settings.setValue("show_grid", show_grid);
  settings.setValue("show_iso_week", show_iso_week);
  settings.setValue("horizontal_header", horizontal_header);

  if (!locale.isEmpty()) {
    settings.setValue("locale", locale);
  }

  // Window settings
  settings.beginGroup("X11");
  settings.setValue("customize_window", x11_options.customize_window);
  settings.setValue("frameless_window", x11_options.frameless_window);
  settings.setValue("skip_task_bar", x11_options.skip_task_bar);
  settings.setValue("window_no_shadow", x11_options.window_no_shadow);
  settings.setValue("window_stays_on_top", x11_options.window_stays_on_top);
  settings.setValue("window_stays_on_bottom",
                    x11_options.window_stays_on_bottom);
  settings.setValue("window_position", x11_options.window_position);
  settings.endGroup();

  settings.beginGroup("Wayland");
  settings.setValue("anchors", wayland_options.anchors);
  settings.setValue("layer", wayland_options.layer);
  settings.setValue("margins", ToStringList(wayland_options.margins));
  settings.endGroup();

  auto write_font_config = [&settings](const FontConfig& config) {
    if (config.size > 0) {
      settings.setValue("font_size", config.size);
    } else {
      settings.setValue("font_size", "");
    }
    settings.setValue("font_family", config.family);
    settings.setValue("fg", config.fg);
    settings.setValue("bg", config.bg);
  };

  // Fonts
  settings.beginGroup("Calendar");
  write_font_config(calendar_font);
  settings.endGroup();
  settings.beginGroup("Header");
  write_font_config(header_font);
  settings.endGroup();

  // Weekday Fonts
  settings.beginGroup("Weekdays");
  settings.setValue("days", ToStringList(weekday_font.days));
  write_font_config(weekday_font.font);
  settings.endGroup();

  if (!clocks.empty()) {
    settings.beginGroup("Clocks");
    write_font_config(clock_font);
    settings.endGroup();
    settings.beginWriteArray("Clocks");
    for (std::size_t i = 0; i < clocks.size(); ++i) {
      settings.setArrayIndex(static_cast<int>(i));
      settings.setValue("label", clocks[i].label);
      settings.setValue("timezone", clocks[i].timezone);
      settings.setValue("format", clocks[i].format);
    }
    settings.endArray();
  }
}

void Configuration::ConfigureWindow(QWidget& widget) const {
  Qt::WindowFlags flags;
  if (x11_options.skip_task_bar) {
    flags |= Qt::SplashScreen;
  }
  if (x11_options.frameless_window) {
    flags |= Qt::FramelessWindowHint;
  }
  if (x11_options.window_no_shadow) {
    flags |= Qt::NoDropShadowWindowHint;
  }
  if (x11_options.window_stays_on_top) {
    flags |= Qt::WindowStaysOnTopHint;
  }
  if (x11_options.window_stays_on_bottom) {
    flags |= Qt::WindowStaysOnBottomHint;
  }
  if (x11_options.customize_window) {
    flags |= Qt::CustomizeWindowHint;
  }

  widget.setWindowFlags(flags);

  if (x11_options.window_position.toLower().trimmed() == "mouse") {
    QPoint cursor_pos = QCursor::pos();
    cursor_pos.setY(cursor_pos.y() - widget.sizeHint().height());
    widget.move(cursor_pos);
  }

  if (QGuiApplication::platformName() == "wayland") {
#ifdef HAVE_LAYER_SHELL
    widget.createWinId();
    QWindow* window = widget.windowHandle();
    LayerShellQt::Window* layer_shell = LayerShellQt::Window::get(window);
    const auto layer = StringToEnum<LayerShellQt::Window::Layer>(
        kLayerMetaEnum, wayland_options.layer);
    const auto anchors = FlaggedStringToEnum<LayerShellQt::Window::Anchors>(
        kAnchorMetaEnum, wayland_options.anchors);
    layer_shell->setAnchors(anchors);
    layer_shell->setLayer(layer);
    layer_shell->setMargins(
        QMargins(wayland_options.margins[0], wayland_options.margins[1],
                 wayland_options.margins[2], wayland_options.margins[3]));
    layer_shell->setScreenConfiguration(
        LayerShellQt::Window::ScreenFromCompositor);
#else
    qInfo() << "No Layer Shell support compiled in.";
#endif
  }
}

void Configuration::ConfigureCalendar(QCalendarWidget& widget) const {
  widget.setGridVisible(show_grid);

  // Undo widget highlighting Saturday / Sunday
  widget.setWeekdayTextFormat(Qt::Saturday,
                              widget.weekdayTextFormat(Qt::Monday));
  widget.setWeekdayTextFormat(Qt::Sunday, widget.weekdayTextFormat(Qt::Monday));

  // Fonts, including weekday fonts
  widget.setHeaderTextFormat(
      UpdateFormat(widget.headerTextFormat(), header_font));
  for (const auto& day : kWeekdays) {
    widget.setWeekdayTextFormat(
        day, UpdateFormat(widget.weekdayTextFormat(day),
                          weekday_font.days.count(day) > 0 ? weekday_font.font
                                                           : calendar_font));
  }

  // Today in bold
  const QDate today = QDate::currentDate();
  const auto weekday = static_cast<Qt::DayOfWeek>(today.dayOfWeek());
  QTextCharFormat today_format = widget.weekdayTextFormat(weekday);
  today_format.setFontWeight(QFont::Bold);
  widget.setDateTextFormat(today, today_format);

  // HorizontalHeader
  widget.setHorizontalHeaderFormat(ToHorizontalHeaderFormat(horizontal_header));

  // VerticalHeader
  widget.setVerticalHeaderFormat(show_iso_week
                                     ? QCalendarWidget::ISOWeekNumbers
                                     : QCalendarWidget::NoVerticalHeader);

  // Default font of the widget
  widget.setFont(CreateFont(calendar_font));
}

void Configuration::ConfigureClockNanny(ClockNanny& nanny) const {
  if (nanny.Clocks().empty()) {
    return;
  }
  const QFont font = CreateFont(clock_font);
  for (auto& clock : nanny.Clocks()) {
    clock.clock_label->setFont(font);
    clock.time_label->setFont(font);
  }
}

}  // namespace calfenster
