#ifndef LANDLORDCORE_H
#define LANDLORDCORE_H

#include <QObject>

#include "utils.h"
#include "call4landlord.h"

class LandlordCore : public QObject {
    Q_OBJECT

private:

	Call4Landlord *c4l;
	PlayerType player;

public:
    explicit LandlordCore(QObject *parent = nullptr);

	void genFullPokers();
	PokerGroup distributePokers();

	void setPlayer(PlayerType _player);
	PlayerType getPlayer() const;
	PlayerType getDownstreamPlayer() const;
	PlayerType getUpstreamPlayer() const;

	bool call(PlayerType player);
	bool pass(PlayerType player);
	bool set(PlayerType player, Call4Landlord::status w);
	PlayerType getCandidate() const;

	ComboType getComboType(const ComboMeta &combo) const;
	int getSubComboType(const ComboMeta &combo, ComboType type) const;
	int getComboValue(const ComboMeta &combo, ComboType type) const;

	int comboSize(const ComboMeta &combo) const;

	bool isBigger(const PokerGroup &lhs, const PokerGroup &rhs) const;
	bool isRocket(const ComboMeta &combo) const;
	bool isPair(const ComboMeta &combo) const;
	bool isThree(const ComboMeta &combo) const;
	bool isBomb(const ComboMeta &combo) const;
	bool isContinous(const ComboMeta &combo, int cnt, int length, int st, int ed) const;

signals:

};

#endif // LANDLORDCORE_H
