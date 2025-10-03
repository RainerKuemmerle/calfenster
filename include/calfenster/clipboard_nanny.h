#pragma once

#include <qdatetime.h>
#include <qobject.h>
#include <qstring.h>

namespace calfenster {
class ClipboardNanny : public QObject {
  Q_OBJECT

 public:
  explicit ClipboardNanny(QObject* parent);
  virtual ~ClipboardNanny() = default;

  void SetFormat(const QString& format);

 protected:
  QString format_ = "dd.MM.yyyy";

 public slots:
  void SetDateInClipboard(const QDate& date);
};
}  // namespace calfenster
