#include "calfenster/app_server.h"

#include <qapplication.h>
#include <qglobal.h>
#include <qlocalserver.h>
#include <qlocalsocket.h>
#include <qobject.h>
#include <qregion.h>

#include "calfenster/moc_app_server.cpp" // NOLINT

namespace {
constexpr int kShutdownByte = 1;
}

namespace calfenster {

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
    if (command_from_client == kShutdownByte) {
      qApp->exit();
    }
  }
}

bool AppServer::SendShutdownRequest() {
  QLocalSocket socket(this);
  socket.connectToServer("calfenster.lock");
  if (!socket.waitForConnected(1000)) {
    return false;
  }
  QByteArray array_block;
  QDataStream out(&array_block, QIODevice::WriteOnly);
  out.setVersion(QDataStream::Qt_5_3);
  out << static_cast<quint8>(kShutdownByte);
  socket.write(array_block);
  socket.waitForBytesWritten(1000);
  return true;
}

}  // namespace calfenster
