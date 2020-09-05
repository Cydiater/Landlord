#include <chrono>
#include <thread>

#include <QDebug>
#include <QString>
#include <QElapsedTimer>

#include "waitforotherplayer.h"
#include "ui_waitforotherplayer.h"

WaitForOtherPlayer::WaitForOtherPlayer(QWidget *parent) :
    QObject(parent),
    pSocket {nullptr},
	connectOK {false} {

}

WaitForOtherPlayer::~WaitForOtherPlayer() {}

QTcpSocket* WaitForOtherPlayer::upstreamSocket() const {
	int index = static_cast<int>(player);
	return pSocket[ (index + 2) % 3 ];
} 

QTcpSocket* WaitForOtherPlayer::downstreamSocket() const {
	int index = static_cast<int>(player);
	return pSocket[ (index + 1) % 3 ];
}

QTcpSocket* WaitForOtherPlayer::playerConnect(quint16 port) {
	QTcpSocket* tmpSocket = new QTcpSocket(nullptr);
	qDebug() << "try connect to " << host.toString() << ":" << port;

	tmpSocket -> connectToHost(host, port);
	while (!tmpSocket -> waitForConnected()) {
		qDebug() << "connect failed";

		QElapsedTimer timer;
		timer.start();
		while (timer.elapsed() < 1000) {
			QEventLoop loop;
			loop.processEvents();
		}

		tmpSocket -> connectToHost(host, port);
	}

	return tmpSocket;
}

void WaitForOtherPlayer::playerFinal() {
	QTcpSocket *socket = static_cast<QTcpSocket*>(sender());
	QByteArray buffer; 
	while (socket -> bytesAvailable() > 0) 
		buffer.append(socket -> readAll());

	QString msg = QString::fromUtf8(buffer);

	qDebug() << msg;

	if (msg == helloA)
		connectOK[0] = true;

	if (msg == helloB) {
		pSocket[1] = socket;
		connectOK[1] = true;
	}

	if (msg == helloC) {
		pSocket[2] = socket;
		connectOK[2] = true;
	}

	if (player == PlayerType::A && connectOK[1] && connectOK[2]) {

		assert(pSocket[1] != nullptr && pSocket[2] != nullptr);
		assert(pSocket[1] != pSocket[2]);

		pSocket[1] -> write(helloA.toUtf8());
		if (!pSocket[1] -> waitForBytesWritten())
			qDebug() << "write failed"; 

		pSocket[2] -> write(helloA.toUtf8());
		if (!pSocket[2] -> waitForBytesWritten())
			qDebug() << "write failed"; 

		connectOK[0] = true; 
	}

	if (connectOK[0]) {
		qDebug() << "start";
		emit gameStart();
	}
}

void WaitForOtherPlayer::playerWait() {
	if (server.hasPendingConnections()) {
		QTcpSocket *tmpSocket = server.nextPendingConnection();
		if (tmpSocket -> parent())
			tmpSocket -> setParent(nullptr);

		connect(tmpSocket, SIGNAL(readyRead()), SLOT(playerFinal()));

		if (player == PlayerType::B) {
			pSocket[2] = tmpSocket;
			if (pSocket[0] != nullptr && !connectOK[1]) {
				connectOK[1] = true;
				connect(pSocket[0], SIGNAL(readyRead()), SLOT(playerFinal()));
				pSocket[0] -> write(helloB.toUtf8());
				if (!pSocket[0] -> waitForBytesWritten())
					qDebug() << "write failed"; 
			}
		}
	}
}

void WaitForOtherPlayer::setPlayerType(PlayerType _player) {
	player = _player;
	switch (player) {

		case PlayerType::A: {

			connect(&server, SIGNAL(newConnection()), SLOT(playerWait()));
			if (!server.listen(host, portA)) {
				qDebug() << "server listen failed";
				return;
			}
			qDebug() << QString("listen to %1:%2").arg(host.toString()).arg(portA);

			break;
		}

		case PlayerType::B: {

			connect(&server, SIGNAL(newConnection()), SLOT(playerWait()));
			if (!server.listen(host, portB)) {
				qDebug() << "server listen failed";
				return;
			}

			pSocket[0] = playerConnect(portA);
			if (pSocket[2] != nullptr && !connectOK[1]) {
				connectOK[1] = true;
				connect(pSocket[0], SIGNAL(readyRead()), SLOT(playerFinal()));
				pSocket[0] -> write(helloB.toUtf8());
				if (!pSocket[0] -> waitForBytesWritten())
					qDebug() << "write failed"; 
			}

			break;
		}

		case PlayerType::C: {

			pSocket[0] = playerConnect(portA);
			pSocket[1] = playerConnect(portB);

			connect(pSocket[0], SIGNAL(readyRead()), SLOT(playerFinal()));
			pSocket[0] -> write(helloC.toUtf8());
			if (!pSocket[0] -> waitForBytesWritten())
				qDebug() << "write failed"; 

			break;
		}

		case PlayerType::undefined: {
			throw std::runtime_error("bad player type");
		}

	}
}
