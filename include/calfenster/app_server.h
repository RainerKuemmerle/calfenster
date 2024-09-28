#pragma once

#include <qglobal.h>
#include <qobject.h>

#include <QLocalServer>
#include <optional>

namespace calfenster {
class AppServer : public QObject {
  Q_OBJECT

 public:
  enum class Command : quint8 {
    kShutdown = 1,
    kPrevMonth,
    kNextMonth,
    kCommandSize  // keep as last for sanity check
  };

  explicit AppServer(QObject* parent = nullptr);
  virtual ~AppServer() = default;

  [[nodiscard]] bool HasOtherInstance() const { return other_instance_; }
  bool SendCommand(Command cmd);

  static std::optional<Command> CommandFromString(const QString& command);

 protected:
  QLocalServer* server_ = nullptr;
  bool other_instance_ = false;

 public slots:
  void SlotNewConnection();
  void SlotReadClient();

 signals:
  void ShowPrevMonth();
  void ShowNextMonth();
};
}  // namespace calfenster
