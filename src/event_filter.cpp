#include "calfenster/event_filter.h"

#include <qapplication.h>
#include <qcalendarwidget.h>
#include <qcoreevent.h>
#include <qdatetime.h>
#include <qdebug.h>
#include <qevent.h>
#include <qeventloop.h>
#include <qglobal.h>
#include <qnamespace.h>
#include <qobject.h>
#include <qwidget.h>

#include "calfenster/moc_event_filter.cpp"  // NOLINT

namespace calfenster {
EventFilter::EventFilter(QObject* parent) : QObject(parent) {}

bool EventFilter::eventFilter(QObject* obj, QEvent* event) {
  if (event->type() == QEvent::Close) {
    auto* close_event = dynamic_cast<QCloseEvent*>(event);
    close_event->accept();
    return true;
  }
  if (event->type() == QEvent::KeyPress) {
    auto* key_event = dynamic_cast<QKeyEvent*>(event);
    if (key_event->key() == Qt::Key_Escape ||
        (key_event->modifiers() == Qt::ControlModifier &&
         key_event->key() == Qt::Key_Q)) {
      qDebug() << "Quit.";
      qApp->exit();
      return true;
    }
    if (calendar_widget != nullptr) {
      if (key_event->modifiers() == Qt::ControlModifier &&
          key_event->key() == Qt::Key_G) {
        calendar_widget->setSelectedDate(QDate::currentDate());
        calendar_widget->showSelectedDate();
        key_event->accept();
        return true;
      }
      if (key_event->modifiers() == Qt::ControlModifier &&
          key_event->key() == Qt::Key_C) {
        emit CopyDate(calendar_widget->selectedDate());
        return true;
      }
    }
  }
  if (event->type() == QEvent::MouseMove) {
    main_widget->activateWindow();
  }
  return QObject::eventFilter(obj, event);
}

}  // namespace calfenster
