#include "calfenster/clock_nanny.h"

#include <qboxlayout.h>
#include <qdatetime.h>
#include <qlabel.h>
#include <qnamespace.h>
#include <qobject.h>
#include <qtimer.h>
#include <qtimezone.h>
#include <qwidget.h>

#include <iostream>
#include <ostream>

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

  Clock& clock = clocks_.emplace_back(timezone, format);
  clock.time_label = new QLabel(holder_widget);

  clock.clock_label = new QLabel(holder_widget);
  // TODO(Rainer): move to main
  clock.clock_label->setText(
      label.isEmpty() ? QString("Clock ") + QString::number(clocks_.size())
                      : label);

  layout->addWidget(clock.clock_label, 0, Qt::AlignLeft);
  layout->addWidget(clock.time_label, 0, Qt::AlignRight);
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

void ClockNanny::PrintAvailableTimezones() {
  const QList<QByteArray> zones = QTimeZone::availableTimeZoneIds();
  for (const auto& zone : zones) {
    std::cout << zone.toStdString() << '\n';
  }
  std::cout << std::flush;
}

}  // namespace calfenster
