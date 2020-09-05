#pragma once

#include <array>

#include <QDebug>
#include <QHostAddress>
#include <QDataStream>

const QHostAddress host = QHostAddress::LocalHost;

enum class PlayerType {
	A = 0,
	B,
	C,
	undefined,
};

enum class NetworkMsg {
	distributeToMe = 0,
	distributeToDownstream,
	distributeToUpstream,
	setBonus,
	playerCallPass,
	playerCallPassSelected,
	playerChoose,
	updateCentralCards,
	eraseCentral,
};

enum class ComboType {
	undefined = 0,
	single,
	pair,
	bomb,
	rocket,
	threeWith,
	singleContinous,
	doubleContinous,
	fourWith,
	aircraft,
	aircraftWithSingle,
	aircraftWithPair
};

const std::vector<QString> ComboLiteral[] = {
	{"undefined"},
	{"single"},
	{"pair"},
	{"bomb"},
	{"rocket"},
	{"three", "three with single", "three with pair"},
	{"Continous of 5", "Continous of 6", "Continous of 7", "Continous of 8", "Continous of 9", "Continous of 10", "Continous of 11", "Continous of 12"},
	{"Double Continous of 3", "Double Continous of 4", "Double Continous of 5", "Double Continous of 6", "Double Continous of 7", "Double Continous of 8", "Double Continous of 9", "Double Continous of 10"},
	{"Four with single", "Four with pair"},
	{"aircraft"},
	{"aircraft with single"},
	{"aircraft with pair"}
};

typedef std::array<int, 15> ComboMeta;

qint32 ByteArray2Int(QByteArray byteArray);
QByteArray Int2ByteArray(qint32 integer);

const QString playerTypeLiteral[] = {
	"Player A",
	"Player B",
	"Player C"
};

struct Poker {
	int index, color;

	Poker(int _index = -1, int _color = 0);
	Poker(const Poker &rhs);

	int value() const;
	QString toString() const;
	QString getImageName() const;

	friend QDataStream& operator<<(QDataStream &stream, Poker &poker);
	friend QDataStream& operator>>(QDataStream &stream, Poker &poker); 
	
	bool operator<(const Poker &rhs) const;
	bool operator==(const Poker &rhs) const;
};

struct PokerGroup {

	std::vector<Poker> group;

	std::size_t size() const;
	void clear();

	PokerGroup* addPoker(const Poker &poker);
	PokerGroup* shuffle();

	ComboMeta toComboMeta() const;

	static PokerGroup genFullPokers();

	friend QDataStream& operator<<(QDataStream &stream, const PokerGroup &group);
	friend QDataStream& operator>>(QDataStream &stream, PokerGroup &group);
};
