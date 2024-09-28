#pragma once

#include <qobject.h>
#include <qwidget.h>

// #include "event_filter.moc"

namespace calfenster {

class EventFilter : public QObject {
  Q_OBJECT
 public:
  explicit EventFilter(QObject* parent = nullptr);

  QWidget* main_widget = nullptr;
  QWidget* calendar_widget = nullptr;

 protected:
  bool eventFilter(QObject* obj, QEvent* event) override;
};

}  // namespace calfenster
