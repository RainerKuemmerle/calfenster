#include <qapplication.h>
#include <qboxlayout.h>
#include <qcalendarwidget.h>
#include <qobject.h>
#include <qwidget.h>

#include <iostream>

#include "calfenster/app_server.h"

int main(int argc, char* argv[]) {
  QApplication app(argc, argv);

  calfenster::AppServer app_server;
  if (app_server.HasOtherInstance()) {
    std::cout << "Other instance found, try to terminate" << std::endl;
    app_server.SendShutdownRequest();
    return 1;
  }

  QWidget main_widget;
  main_widget.setWindowFlags(Qt::CustomizeWindowHint);

  auto* calendar_widget = new QCalendarWidget(&main_widget);
  auto* stack_layout = new QHBoxLayout(&main_widget);
  stack_layout->addWidget(calendar_widget);

  main_widget.show();

  return QApplication::exec();
}
