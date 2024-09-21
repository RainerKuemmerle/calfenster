#include "calfenster/event_filter.h"

#include <qapplication.h>
#include <qcoreevent.h>
#include <qevent.h>
#include <qnamespace.h>
#include <qobject.h>
#include <qwidget.h>

#include "calfenster/moc_event_filter.cpp"  // NOLINT

namespace calfenster {
EventFilter::EventFilter(QObject* parent) : QObject(parent) {}

bool EventFilter::eventFilter(QObject* obj, QEvent* event) {
  if (event->type() == QEvent::Close) {
    qApp->exit();
    return true;
  }
  if (event->type() == QEvent::KeyPress) {
    auto* key_event = static_cast<QKeyEvent*>(event);
    if (key_event->key() == Qt::Key_Escape ||
        (key_event->modifiers() == Qt::ControlModifier &&
         key_event->key() == Qt::Key_Q)) {
      qApp->exit();
    }
  }
  return QObject::eventFilter(obj, event);
}

}  // namespace calfenster
