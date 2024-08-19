#ifndef LISTENER_HPP
#define LISTENER_HPP

#include <QLocalSocket>
#include <QLocalServer>
#include <QString>

#include "backend/runtime/runtime.h"

class Listener : public QObject {
    Q_OBJECT
public:
    explicit Listener(QObject *parent = nullptr);

    void openServer(const class QString& name);

signals:
    void newStateReceived(Orient dir, Cell cur);

private slots:
    void acquireState();
    void newConnection();
private:
    QLocalServer* m_server;
};

#endif // LISTENER_HPP
