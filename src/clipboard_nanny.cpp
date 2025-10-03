#include "calfenster/clipboard_nanny.h"

#include <qclipboard.h>
#include <qdebug.h>
#include <qguiapplication.h>

#include "calfenster/moc_clipboard_nanny.cpp"  // NOLINT

namespace calfenster {
ClipboardNanny::ClipboardNanny(QObject* parent) : QObject(parent) {}

void ClipboardNanny::SetFormat(const QString& format) { format_ = format; }

void ClipboardNanny::SetDateInClipboard(const QDate& date) {
  QClipboard* clipboard = QGuiApplication::clipboard();
  const QString formatted_date = date.toString(format_);
  qDebug() << "Set clipboard to " << formatted_date;
  clipboard->setText(formatted_date);
}

}  // namespace calfenster
