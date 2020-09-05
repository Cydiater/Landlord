#ifndef WAITFOROTHERPLAYER_H
#define WAITFOROTHERPLAYER_H

#include <QTcpServer>
#include <QTcpSocket>
#include <queue>
#include <QMutex>

#include "utils.h"

const quint16 portA = 63001;
const quint16 portB = 63002;

const QString helloA = "ok from player A";
const QString helloB = "ok from player B";
const QString helloC = "ok from player C";

class WaitForOtherPlayer : public QObject {

	Q_OBJECT

public:
    explicit WaitForOtherPlayer(QWidget *parent = nullptr);
    ~WaitForOtherPlayer();

	void setPlayerType(PlayerType _player);
	QTcpSocket* upstreamSocket() const;
	QTcpSocket* downstreamSocket() const;

private slots:
	QTcpSocket* playerConnect(quint16 port);

	void playerWait();
	void playerFinal();

signals:
	void gameStart();

private:

	QTcpServer server; 
	QTcpSocket *pSocket[3];
	PlayerType player;
	bool connectOK[3];
};

#endif // WAITFOROTHERPLAYER_H
