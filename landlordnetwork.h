#ifndef LANDLORDNETWORK_H
#define LANDLORDNETWORK_H

#include <QObject>
#include <QTcpSocket>
#include <QDataStream>

#include "utils.h"

class LandlordNetwork : public QObject {
    Q_OBJECT
public:
    explicit LandlordNetwork(QObject *parent, QTcpSocket *_upstreamSocket, QTcpSocket *_downstreamSocket);

	bool send(QTcpSocket *socket, const PokerGroup &group, NetworkMsg msg, int value = 0);
	bool send2Upstream(const PokerGroup &group, NetworkMsg msg, int value = 0);
	bool send2Downstream(const PokerGroup &group, NetworkMsg msg, int value = 0);

private:
	QTcpSocket *upstreamSocket, *downstreamSocket;
	qint32 upstreamSize, downstreamSize;
	QByteArray upstreamBuffer, downstreamBuffer;

private slots:
	void dataHandle();

signals:
	void receiveFromUpstream(const PokerGroup &group, NetworkMsg msg, int value);	
	void receiveFromDownstream(const PokerGroup &group, NetworkMsg msg, int value);	

};

#endif // LANDLORDNETWORK_H
