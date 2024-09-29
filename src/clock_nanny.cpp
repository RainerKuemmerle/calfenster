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
    const QDateTime datetime = now.toTimeZone(clock.timezone);
    const QString time_as_string = clock.format.isEmpty()
                                       ? datetime.toString(Qt::RFC2822Date)
                                       : datetime.toString(clock.format);
    clock.time_label->setText(time_as_string);
  }
}

void ClockNanny::Tick() { UpdateClocks(QDateTime::currentDateTime()); }

}  // namespace calfenster
