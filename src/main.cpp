#include <qapplication.h>
#include <qcalendarwidget.h>
#include <qobject.h>

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

  QCalendarWidget widget;
  widget.show();

  return QApplication::exec();
}
