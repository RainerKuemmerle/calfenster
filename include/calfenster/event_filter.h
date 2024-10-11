#pragma once

#include <qobject.h>

class QWidget;
class QCalendarWidget;

namespace calfenster {

class EventFilter : public QObject {
  Q_OBJECT
 public:
  explicit EventFilter(QObject* parent = nullptr);

  QWidget* main_widget = nullptr;
  QCalendarWidget* calendar_widget = nullptr;

 protected:
  bool eventFilter(QObject* obj, QEvent* event) override;
};

}  // namespace calfenster
