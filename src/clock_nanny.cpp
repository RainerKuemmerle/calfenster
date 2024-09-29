#include "calfenster/clock_nanny.h"

#include <qboxlayout.h>
#include <qdatetime.h>
#include <qlabel.h>
#include <qobject.h>
#include <qtimer.h>
#include <qwidget.h>

#include "calfenster/moc_clock_nanny.cpp"  // NOLINT

namespace calfenster {
ClockNanny::ClockNanny(QObject* parent, QVBoxLayout* layout)
    : QObject(parent), parent_layout_(layout) {
  timer_ = new QTimer(this);
  connect(timer_, &QTimer::timeout, this, QOverload<>::of(&ClockNanny::Tick));
  timer_->start(1000);
}

void ClockNanny::AddClock(const QString& label, const QTimeZone& timezone,
                          const QString& format) {
  auto* holder_widget = new QWidget();
  parent_layout_->addWidget(holder_widget);

  auto* layout = new QHBoxLayout(holder_widget);
  layout->setContentsMargins(5, 2, 5, 2);

  clocks_.emplace_back(timezone, format);
  clocks_.back().time_label = new QLabel(holder_widget);

  auto* clock_label = new QLabel(holder_widget);
  clock_label->setText(label);

  layout->addWidget(clock_label);
  layout->addWidget(clocks_.back().time_label);
}

void ClockNanny::UpdateClocks(const QDateTime& now) {
  for (const auto& clock : clocks_) {
    QDateTime datetime = now.toTimeZone(clock.timezone);
    clock.time_label->setText(datetime.toString(clock.format));
  }
}

void ClockNanny::Tick() { UpdateClocks(QDateTime::currentDateTime()); }

}  // namespace calfenster
