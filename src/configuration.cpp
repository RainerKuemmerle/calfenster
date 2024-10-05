#include "calfenster/configuration.h"

#include <qapplication.h>
#include <qcalendarwidget.h>
#include <qdebug.h>
#include <qfont.h>
#include <qglobal.h>
#include <qnamespace.h>
#include <qsettings.h>
#include <qtextformat.h>
#include <qwidget.h>

#include <cstddef>

#include "calfenster/clock_nanny.h"

namespace {
const QString kSettingsOrg = "calfenster";
const QString kSettingsApp = "calfenster";
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

QFont CreateFont(const calfenster::Configuration::FontConfig& config) {
  const QFont font(
      !config.family.isEmpty() ? config.family : DefaultFont().family(),
      config.size > 0 ? config.size : DefaultFont().pointSize());
  return font;
};

QCalendarWidget::HorizontalHeaderFormat ToHorizontalHeaderFormat(
    QString header) {
  header = header.toLower().trimmed();
  if (header == "single") return QCalendarWidget::SingleLetterDayNames;
  if (header == "short") return QCalendarWidget::ShortDayNames;
  if (header == "long") return QCalendarWidget::LongDayNames;
  return QCalendarWidget::NoHorizontalHeader;
}

}  // namespace

Configuration::FontConfig::FontConfig()
    : family(DefaultFont().family()), size(DefaultFont().pointSize()) {}

Configuration::Configuration() {
  QSettings settings(kSettingsOrg, kSettingsApp);
  save_on_exit = settings.allKeys().isEmpty();

  // Window settings
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

  // Calendar display
  show_grid = settings.value("show_grid", show_grid).toBool();
  show_iso_week = settings.value("show_iso_week", show_iso_week).toBool();
  horizontal_header =
      settings.value("horizontal_header", horizontal_header).toString();

  locale = settings.value("locale", locale).toString();

  auto read_font_config = [&settings](FontConfig& config) {
    config.size = settings.value("font_size", config.size).toInt();
    config.family = settings.value("font_family", config.family).toString();
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
  if (!save_on_exit) return;
  // Create a configuration with default values
  QSettings settings(kSettingsOrg, kSettingsApp);

  // Window settings
  settings.setValue("skip_task_bar", skip_task_bar);
  settings.setValue("frameless_window", frameless_window);
  settings.setValue("window_no_shadow", window_no_shadow);
  settings.setValue("window_stays_on_top", window_stays_on_top);
  settings.setValue("window_stays_on_bottom", window_stays_on_bottom);
  settings.setValue("customize_window", customize_window);
  settings.setValue("window_position", window_position);

  // Calendar display
  settings.setValue("show_grid", show_grid);
  settings.setValue("show_iso_week", show_iso_week);
  settings.setValue("horizontal_header", horizontal_header);

  if (!locale.isEmpty()) settings.setValue("locale", locale);

  auto write_font_config = [&settings](const FontConfig& config) {
    settings.setValue("font_size", config.size);
    settings.setValue("font_family", config.family);
  };

  // Fonts
  settings.beginGroup("Calendar");
  write_font_config(calendar_font);
  settings.endGroup();
  settings.beginGroup("Header");
  write_font_config(header_font);
  settings.endGroup();

  if (!clocks.empty()) {
    settings.beginGroup("Clocks");
    write_font_config(clock_font);
    settings.endGroup();
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

  if (window_position.toLower().trimmed() == "mouse") {
    QPoint cursor_pos = QCursor::pos();
    cursor_pos.setY(cursor_pos.y() - widget.sizeHint().height());
    widget.move(cursor_pos);
  }
}

void Configuration::ConfigureCalendar(QCalendarWidget& widget) const {
  widget.setGridVisible(show_grid);
  const std::vector<Qt::DayOfWeek> weekdays = {
      Qt::Monday, Qt::Tuesday,  Qt::Wednesday, Qt::Thursday,
      Qt::Friday, Qt::Saturday, Qt::Sunday};

  // Fonts
  const QFont qt_calendar_font = CreateFont(calendar_font);
  for (auto day : weekdays) {
    QTextCharFormat format = widget.weekdayTextFormat(day);
    format.setFont(qt_calendar_font);
    widget.setWeekdayTextFormat(day, format);
  }
  QTextCharFormat format = widget.headerTextFormat();
  format.setFont(CreateFont(header_font));
  widget.setHeaderTextFormat(format);

  // HorizontalHeader
  widget.setHorizontalHeaderFormat(ToHorizontalHeaderFormat(horizontal_header));

  // VerticalHeader
  widget.setVerticalHeaderFormat(show_iso_week
                                     ? QCalendarWidget::ISOWeekNumbers
                                     : QCalendarWidget::NoVerticalHeader);
}

void Configuration::ConfigureClockNanny(ClockNanny& nanny) const {
  if (nanny.Clocks().empty()) return;
  const QFont font = CreateFont(clock_font);
  for (auto& clock : nanny.Clocks()) {
    clock.clock_label->setFont(font);
    clock.time_label->setFont(font);
  }
}

}  // namespace calfenster
