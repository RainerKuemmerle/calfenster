#pragma once

#include <qboxlayout.h>
#include <qdatetime.h>
#include <qlabel.h>
#include <qobject.h>
#include <qobjectdefs.h>
#include <qtimezone.h>

#include <utility>
#include <vector>

namespace calfenster {
class ClockNanny : public QObject {
  Q_OBJECT

 public:
  struct Clock {
    Clock(QTimeZone timezone, QString format)
        : timezone(std::move(timezone)), format(std::move(format)) {}
    QTimeZone timezone;
    QString format;
    QLabel* clock_label = nullptr;
    QLabel* time_label = nullptr;
  };

  ClockNanny(QObject* parent, QVBoxLayout* layout);
  virtual ~ClockNanny() = default;

  void Tick();

  void AddClock(const QString& label, const QTimeZone& timezone,
                const QString& format);

  static void PrintAvailableTimezones();

  std::vector<Clock>& Clocks() { return clocks_; }
  [[nodiscard]] const std::vector<Clock>& Clocks() const { return clocks_; }

 protected:
  QVBoxLayout* parent_layout_ = nullptr;
  QTimer* timer_ = nullptr;
  std::vector<Clock> clocks_;
  void UpdateClocks(const QDateTime& now);
};
}  // namespace calfenster
