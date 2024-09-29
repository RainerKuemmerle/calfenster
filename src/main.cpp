#include <qapplication.h>
#include <qboxlayout.h>
#include <qcalendarwidget.h>
#include <qcommandlineparser.h>
#include <qdebug.h>
#include <qglobal.h>
#include <qlocale.h>
#include <qobject.h>
#include <qtimezone.h>
#include <qwidget.h>

#include <cstdlib>
#include <ctime>
#include <optional>

#include "calfenster/app_server.h"
#include "calfenster/clock_nanny.h"
#include "calfenster/configuration.h"
#include "calfenster/event_filter.h"

int main(int argc, char* argv[]) {
  QApplication app(argc, argv);
  QCoreApplication::setApplicationName("CalFenster");

  // Command line parsing
  QCommandLineParser command_line;
  command_line.addPositionalArgument(
      "command", "prev | next: Show previous or next month");
  command_line.setApplicationDescription("A simple calender widget.");
  command_line.addHelpOption();
  // actual parsing of the command line
  command_line.process(app);

  // check the given commands
  const QStringList commands = command_line.positionalArguments();
  if (commands.size() > 1) {
    qCritical() << "Got more than 1 command: " << commands;
    command_line.showHelp(EXIT_FAILURE);
  }
  const std::optional<calfenster::AppServer::Command> command =
      commands.isEmpty() ? calfenster::AppServer::Command::kShutdown
                         : calfenster::AppServer::CommandFromString(
                               commands[0].trimmed().toLower());

  calfenster::AppServer app_server;
  if (app_server.HasOtherInstance()) {
    if (!command.has_value()) {
      qWarning() << "Wrong command supplied";
      command_line.showHelp(EXIT_FAILURE);
    }
    if (command.value() == calfenster::AppServer::Command::kShutdown) {
      qInfo() << "Other instance found, try to terminate";
    }
    const bool request_status = app_server.SendCommand(command.value());
    return request_status ? EXIT_SUCCESS : EXIT_FAILURE;
  }

  calfenster::Configuration config;
  if (!config.locale.isEmpty()) {
    QLocale locale_override(config.locale);
    QLocale::setDefault(locale_override);
  }

  QWidget main_widget;
  auto* calendar_widget = new QCalendarWidget();
  auto* stack_layout = new QVBoxLayout(&main_widget);
  stack_layout->setSpacing(0);
  stack_layout->setContentsMargins(0, 0, 0, 0);
  stack_layout->addWidget(calendar_widget);

  // Connect signals from AppServer to CalendarWidget
  QObject::connect(&app_server, SIGNAL(ShowPrevMonth()), calendar_widget,
                   SLOT(showPreviousMonth()));
  QObject::connect(&app_server, SIGNAL(ShowNextMonth()), calendar_widget,
                   SLOT(showNextMonth()));

  auto* event_filter = new calfenster::EventFilter(&main_widget);
  event_filter->main_widget = &main_widget;
  event_filter->calendar_widget = calendar_widget;
  main_widget.installEventFilter(event_filter);
  calendar_widget->installEventFilter(event_filter);

  config.ConfigureWindow(main_widget);
  config.ConfigureCalendar(*calendar_widget);

  // Add Clocks to widget
  calfenster::ClockNanny* clock_nanny = nullptr;
  if (!config.clocks.empty()) {
    clock_nanny = new calfenster::ClockNanny(&main_widget, stack_layout);
    for (const auto& clock : config.clocks) {
      QByteArray timezone_specifier = clock.timezone.toLatin1();
      if (!QTimeZone::isTimeZoneIdAvailable(timezone_specifier)) {
        qWarning() << "Timezone specification" << clock.timezone
                   << "is not valid";
        continue;
      }
      QTimeZone timezone(timezone_specifier);
      clock_nanny->AddClock(clock.label, timezone, clock.format);
    }
    clock_nanny->Tick();
  }

  main_widget.show();
  return QApplication::exec();
}
