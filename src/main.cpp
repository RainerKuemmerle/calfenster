#include <QApplication>
#include <QCalendarWidget>

int main(int argc, char* argv[]) {
  QApplication app(argc, argv);

  QCalendarWidget widget;
  widget.show();

  return QApplication::exec();
}
