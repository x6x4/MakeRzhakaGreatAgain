#include "listener.hpp"

#include <QMessageBox>
#include <QDebug>

Listener::Listener(QObject *parent) : QObject{parent}
{
    m_server = new QLocalServer(this);
}

void
Listener::openServer(const QString& name)
{
    if (!m_server->listen(name)) {
        QMessageBox::critical(nullptr, "Server error",
                                       "Unable to open server: " + m_server->errorString());
        qDebug("Server not run");
        m_server->close();
        return;
    }

    connect(m_server, SIGNAL(newConnection()), this, SLOT(newConnection()));
}

void
Listener::acquireState()
{
    auto* socket = reinterpret_cast<QLocalSocket*>(sender());
    QDataStream in(socket);
    State state = {.cell = std::make_pair(2, 1), .direction = Orient::NORTH };
    while(socket->bytesAvailable() >= sizeof(state)) {
        auto nbytes = in.readRawData(reinterpret_cast<char*>(&state), sizeof(State));
#if 0
        for(size_t i = 0; i < nbytes; i++) {
            qDebug() << "[" << i << "] =" << Qt::hex << static_cast<qint32>(reinterpret_cast<char*>(&state)[i]);
        }
        qDebug() << "----------------------------------";
#endif
        if (nbytes == sizeof(State)) {
            emit newStateReceived(state.direction, state.cell);
        }
    }
}

void
Listener::newConnection() {
    auto* socket = m_server->nextPendingConnection();
    connect(socket, SIGNAL(disconnected()), socket, SLOT(deleteLater()));
    connect(socket, SIGNAL(readyRead()), this, SLOT(acquireState()));
}
