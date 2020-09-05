#include <chrono>
#include <random>

#include "utils.h"

qint32 ByteArray2Int(QByteArray byteArray) {
	qint32 temp;
    QDataStream data(&byteArray, QIODevice::ReadWrite);
    data >> temp;
    return temp;
}

QByteArray Int2ByteArray(qint32 integer) {
	QByteArray temp;
	QDataStream data(&temp, QIODevice::ReadWrite);
	data << integer;
	return temp;
}

Poker::Poker(int _index, int _color) :
	index {_index},
	color {_color} {

}

QString Poker::toString() const {
	return QString("[%1, %2]").arg(index).arg(color);
}

Poker::Poker(const Poker &rhs) : index {rhs.index},
								 color {rhs.color} {}

QString Poker::getImageName() const {
	int v = index;
	if (v == 0) v = 13;
	else if (v == 13)
        return QString("smalljoker.png");
	else if (v == 14)
        return QString("bigjoker.png");

	return QString("poker_t_%1_v_%2.png").arg(color + 1).arg(v);
}

int Poker::value() const {
	if (index <= 1) return 13 + index;
	if (index >= 13) return 2 + index;
	return index;
}

bool Poker::operator<(const Poker &rhs) const {
	if (value() != rhs.value())
		return value() < rhs.value();
	return color < rhs.color;
}

bool Poker::operator==(const Poker &rhs) const {
	return index == rhs.index && color == rhs.color;
}

PokerGroup* PokerGroup::addPoker(const Poker &poker) {
	group.push_back(poker);
	return this;
}

PokerGroup* PokerGroup::shuffle() {
    std::random_device rd;
    std::mt19937 g(rd());
 
    std::shuffle(group.begin(), group.end(), g);
	return this;
}

PokerGroup PokerGroup::genFullPokers() {
	PokerGroup full;

	for (int i = 0; i < 13; i++)
		for (int j = 0; j < 4; j++)
			full.addPoker(Poker(i, j));

	full.addPoker(Poker(13))
		-> addPoker(Poker(14));

	return full;
}

std::size_t PokerGroup::size() const {
	return group.size();
}

void PokerGroup::clear() {
	group.clear();
}

ComboMeta PokerGroup::toComboMeta() const {
	ComboMeta combo;
	combo.fill(0);
	for (auto poker: group)
		combo[ poker.value() - 2 ]++;
	return combo;
}

QDataStream& operator<<(QDataStream &stream, Poker &poker) {
	stream << poker.index << poker.color;
	return stream;
}

QDataStream& operator>>(QDataStream &stream, Poker &poker) {
	stream >> poker.index >> poker.color;
	return stream;
}

QDataStream& operator<<(QDataStream &stream, const PokerGroup &group) {
	stream << static_cast<int>(group.size());	
	for (auto poker: group.group) 
		stream << poker;
	return stream;
}

QDataStream& operator>>(QDataStream &stream, PokerGroup &group) {
	group.clear();
	int size;
	stream >> size;
	while (size-- > 0) {
		Poker poker;
		stream >> poker;
		group.addPoker(poker);
	}
	return stream;
}
