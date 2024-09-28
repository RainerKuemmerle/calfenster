#include "calfenster/app_server.h"

#include <qapplication.h>
#include <qglobal.h>
#include <qlocalserver.h>
#include <qlocalsocket.h>
#include <qobject.h>
#include <qregion.h>

#include <optional>

#include "calfenster/moc_app_server.cpp"  // NOLINT

namespace calfenster {

static_assert(static_cast<int>(AppServer::Command::kCommandSize) < 255,
              "Command has to fit into a byte");

AppServer::AppServer(QObject* parent) : QObject(parent) {
  server_ = new QLocalServer(this);
  if (!server_->listen("calfenster.lock")) {
    server_->close();
    other_instance_ = true;
    return;
  }

  connect(server_, SIGNAL(newConnection()), this, SLOT(SlotNewConnection()));
}

void AppServer::SlotNewConnection() {
  QLocalSocket* local_socket = server_->nextPendingConnection();
  connect(local_socket, SIGNAL(disconnected()), local_socket,
          SLOT(deleteLater()));
  connect(local_socket, SIGNAL(readyRead()), this, SLOT(SlotReadClient()));
}

void AppServer::SlotReadClient() {
  auto* local_socket = static_cast<QLocalSocket*>(sender());
  QDataStream in(local_socket);
  in.setVersion(QDataStream::Qt_5_3);
  quint8 command_from_client;
  for (;;) {
    if (local_socket->bytesAvailable() < static_cast<int>(sizeof(quint8)))
      break;
    in >> command_from_client;
    auto command = static_cast<Command>(command_from_client);
    switch (command) {
      case Command::kShutdown:
        qApp->exit();
        break;
      case Command::kPrevMonth:
        emit ShowPrevMonth();
        break;
      case Command::kNextMonth:
        emit ShowNextMonth();
        break;
      case Command::kCommandSize:
        break;
    }
  }
}

bool AppServer::SendCommand(Command cmd) {
  QLocalSocket socket(this);
  socket.connectToServer("calfenster.lock");
  if (!socket.waitForConnected(1000)) {
    return false;
  }
  QByteArray array_block;
  QDataStream out(&array_block, QIODevice::WriteOnly);
  out.setVersion(QDataStream::Qt_5_3);
  out << static_cast<quint8>(cmd);
  socket.write(array_block);
  return socket.waitForBytesWritten(1000);
}

std::optional<AppServer::Command> AppServer::CommandFromString(
    const QString& command) {
  if (command == "prev") return Command::kPrevMonth;
  if (command == "next") return Command::kNextMonth;
  return std::nullopt;
}

}  // namespace calfenster
