#pragma once

#include <qobject.h>

#include <QLocalServer>

#include "app_server.moc"

namespace calfenster {
class AppServer : public QObject {
  Q_OBJECT

 public:
  explicit AppServer(QObject* parent = nullptr);
  virtual ~AppServer() = default;

  [[nodiscard]] bool HasOtherInstance() const { return other_instance_; }
  bool SendShutdownRequest();

 protected:
  QLocalServer* server_ = nullptr;
  bool other_instance_ = false;

 public slots:
  void SlotNewConnection();
  void SlotReadClient();
};
}  // namespace calfenster
