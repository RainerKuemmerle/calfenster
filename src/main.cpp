#include <qapplication.h>
#include <qboxlayout.h>
#include <qcalendarwidget.h>
#include <qcommandlineparser.h>
#include <qobject.h>
#include <qwidget.h>

#include <cstdlib>
#include <iostream>

#include "calfenster/app_server.h"
#include "calfenster/configuration.h"
#include "calfenster/event_filter.h"

int main(int argc, char* argv[]) {
  QApplication app(argc, argv);
  QCoreApplication::setApplicationName("CalFenster");

  // Command line parsing
  QCommandLineParser command_line;
  command_line.setApplicationDescription("A simple calender widget.");
  command_line.addHelpOption();
  // actual parsing of the command line
  command_line.process(app);

  calfenster::AppServer app_server;
  if (app_server.HasOtherInstance()) {
    std::cout << "Other instance found, try to terminate" << std::endl;
    bool request_status = app_server.SendShutdownRequest();
    return request_status ? EXIT_SUCCESS : EXIT_FAILURE;
  }

  QWidget main_widget;
  auto* calendar_widget = new QCalendarWidget(&main_widget);
  auto* stack_layout = new QHBoxLayout(&main_widget);
  stack_layout->addWidget(calendar_widget);

  calfenster::Configuration config;

  auto* event_filter = new calfenster::EventFilter(&main_widget);
  event_filter->main_widget = &main_widget;
  event_filter->calendar_widget = calendar_widget;
  main_widget.installEventFilter(event_filter);
  calendar_widget->installEventFilter(event_filter);

  config.ConfigureWindow(main_widget);
  config.ConfigureCalendar(*calendar_widget);

  main_widget.show();
  main_widget.activateWindow();
  return QApplication::exec();
}
