#include "landlordnetwork.h"

LandlordNetwork::LandlordNetwork(QObject *parent, QTcpSocket *_upstreamSocket, QTcpSocket *_downstreamSocket) : 
	QObject(parent),
	upstreamSocket(_upstreamSocket),
	downstreamSocket(_downstreamSocket),
	upstreamSize(0),
	downstreamSize(0) {
	connect(upstreamSocket, SIGNAL(readyRead()), SLOT(dataHandle()));
	connect(downstreamSocket, SIGNAL(readyRead()), SLOT(dataHandle()));
}

bool LandlordNetwork::send(QTcpSocket *socket, const PokerGroup &group, NetworkMsg msg, int value) {
	QByteArray bytes;
	QDataStream stream(&bytes, QIODevice::ReadWrite);

	stream << group << msg << value;

	assert(socket -> state() == QAbstractSocket::ConnectedState);

	socket -> write(Int2ByteArray(bytes.size()));
	socket -> write(bytes);

	return socket -> waitForBytesWritten();
}

void LandlordNetwork::dataHandle() {
	QTcpSocket *socket = static_cast<QTcpSocket*>(sender());
	QByteArray *buffer = socket == upstreamSocket ? &upstreamBuffer : &downstreamBuffer;
	qint32 &size = socket == upstreamSocket ? upstreamSize : downstreamSize;

	while (socket -> bytesAvailable() > 0) {
		buffer -> append(socket -> readAll());
		while ((size == 0 && buffer -> size() >= 4) || (size > 0 && buffer -> size() >= size)) {

			if (size == 0 && buffer -> size() >= 4) {
				size = ByteArray2Int(buffer -> mid(0, 4));
				buffer -> remove(0, 4);
			}

			if (size > 0 && buffer -> size() >= size) {
				QByteArray data = buffer -> mid(0, size);
				buffer -> remove(0, size);
				size = 0;

				PokerGroup group;
				NetworkMsg msg;
				int value;

				QDataStream stream(&data, QIODevice::ReadWrite);
				stream >> group >> msg >> value;

				if (socket == upstreamSocket)		
					emit receiveFromUpstream(group, msg, value);
				else 
					emit receiveFromDownstream(group, msg, value);
			}
		}
	}
}

bool LandlordNetwork::send2Upstream(const PokerGroup &group, NetworkMsg msg, int value) {
	return send(upstreamSocket, group, msg, value);
}

bool LandlordNetwork::send2Downstream(const PokerGroup &group, NetworkMsg msg, int value) {
	return send(downstreamSocket, group, msg, value);
}
