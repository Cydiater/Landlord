#include "landlordcore.h"

LandlordCore::LandlordCore(QObject *parent) : QObject(parent), c4l {nullptr} {}

bool LandlordCore::call(PlayerType player) {
	if (c4l == nullptr)
		c4l = new Call4Landlord(player);
	c4l -> call(player);	
	return c4l -> hasCandidate();
}

bool LandlordCore::pass(PlayerType player) {
	if (c4l == nullptr)
		c4l = new Call4Landlord(player);
	c4l -> pass(player);	
	return c4l -> hasCandidate();
}

bool LandlordCore::set(PlayerType player, Call4Landlord::status w) {
	if (c4l == nullptr)
		c4l = new Call4Landlord(player);
	c4l -> set(player, w);	
	return c4l -> hasCandidate();
}

PlayerType LandlordCore::getCandidate() const {
	assert(c4l -> hasCandidate());
	return c4l -> getCandidate();
}

PlayerType LandlordCore::getPlayer() const {
	return player;
}

void LandlordCore::setPlayer(PlayerType _player) {
	player = _player;
}

PlayerType LandlordCore::getDownstreamPlayer() const {
	int index = static_cast<int>(player);
	return static_cast<PlayerType>((index + 1) % 3);
}

PlayerType LandlordCore::getUpstreamPlayer() const {
	int index = static_cast<int>(player);
	return static_cast<PlayerType>((index + 2) % 3);
}

int LandlordCore::comboSize(const ComboMeta &combo) const {
	int size = 0;	
	for (int i = 0; i < 15; i++)
		size += combo[i];
	return size;
}

bool LandlordCore::isRocket(const ComboMeta &combo) const {
	assert(comboSize(combo) == 2);
	return combo[13] == 1 && combo[14] == 1;
}

bool LandlordCore::isPair(const ComboMeta &combo) const {
	assert(comboSize(combo) == 2);
	for (int i = 0; i < 15; i++)
		if (combo[i] == 2)
			return true;
	return false;
}

bool LandlordCore::isThree(const ComboMeta &combo) const {
	assert(comboSize(combo) == 3);
	for (int i = 0; i < 15; i++)
		if (combo[i] == 3)
			return true;
	return false;
}

bool LandlordCore::isBomb(const ComboMeta &combo) const {
	assert(comboSize(combo) == 4);
	for (int i = 0; i < 15; i++)
		if (combo[i] == 4)
			return true;
	return false;
}

bool LandlordCore::isContinous(const ComboMeta &combo, int cnt, int length, int st, int ed) const {
	for (int cl = st; cl < ed; cl++) {
		for (int cr = cl; cr < ed; cr++) {
			if (combo[cr] != cnt)
				break;
			if (cr - cl + 1 >= length)
				return true;
		}
	}
	return false;
}

ComboType LandlordCore::getComboType(const ComboMeta &combo) const {

	int size = comboSize(combo);

	if (size == 0) return ComboType::undefined;

	if (size == 1) return ComboType::single;

	if (size == 2) {
		if (isRocket(combo))
			return ComboType::rocket;

		if (isPair(combo))
			return ComboType::pair;
	}

	if (size == 4) {
		if (isBomb(combo))
			return ComboType::bomb;
	}

	ComboMeta tmp = combo;

	int tmpSize = size;
	for (int i = 0; i < 15; i++) 
		if (1 <= tmp[i] && tmp[i] <= 2) {
			tmpSize -= tmp[i];
			tmp[i] = 0;
			break;
		}

	if (tmpSize == 3 && isThree(tmp))
		return ComboType::threeWith;

	if (isContinous(combo, 1, 5, 0, 12))
		return ComboType::singleContinous;

	if (isContinous(combo, 2, 3, 0, 12))
		return ComboType::doubleContinous;

	for (int i = 0; i < 15; i++) if (combo[i] == 4) {

		if (size == 6) 
			return ComboType::fourWith;

		if (size == 8) {
			bool vaild = true;
			for (int j = 0; j < 15; j++) if (i != j)
				if (combo[j] != 2 && combo[j] > 0)
					vaild = false;
			if (vaild)
				return ComboType::fourWith;
		}

		break;
	}

	if (isContinous(combo, 3, 2, 0, 12))
		return ComboType::aircraft;

	if (size % 4 == 0) {
		int length = size / 4;

		if (length > 2) {
			ComboMeta tmp = combo;
			for (int i = 0; i < 15; i++) {
				if (tmp[i] > 3) 
					tmp[i] = 3;
				else if (combo[i] > 0)
					tmp[i] = 0;
			}

			if (isContinous(tmp, 3, length, 0, 12))
				return ComboType::aircraftWithSingle;

		}
	}

	if (size % 5 == 0) {
		int length = size / 5;
		
		if (length > 2) {
			ComboMeta tmp = combo;
			for (int i = 0; i < 15; i++) if (tmp[i] == 2 || tmp[i] == 4) {
				tmp[i] = 0;
			} 

			if (isContinous(tmp, 3, length, 1, 12))
				return ComboType::aircraftWithPair;
		}
	}

	return ComboType::undefined;
}

int LandlordCore::getSubComboType(const ComboMeta &combo, ComboType type) const {

	int size = comboSize(combo);

	if (type == ComboType::undefined)
		return 0;

	if (type == ComboType::single) 
		return 0;
	
	if (type == ComboType::pair)
		return 0;

	if (type == ComboType::bomb)
		return 0;

	if (type == ComboType::rocket)
		return 0;

	if (type == ComboType::threeWith) 
		return size - 3;

	if (type == ComboType::singleContinous) 
		return size - 5;

	if (type == ComboType::doubleContinous) 
		return size / 2 - 3;

	if (type == ComboType::fourWith) {
		if (size == 6)
			return 0;
		if (size == 8)
			return 1;
		throw std::runtime_error("bad four with");
	}

	if (type == ComboType::aircraft) 
		return size / 3 - 2;

	if (type == ComboType::aircraftWithSingle)
		return size / 4 - 2;

	if (type == ComboType::aircraftWithPair)
		return size / 5 - 2;

	throw std::runtime_error("bad combo");
}

int LandlordCore::getComboValue(const ComboMeta &combo, ComboType type) const {
	if (type == ComboType::undefined)
		return 0;

	int max = -1, min = -1;

	for (int i = 0; i < 15; i++) if (combo[i] > 0)
		max = i;

	for (int i = 14; i >= 0; i--) if (combo[i] > 0)
		min = i;

	assert(max != -1);

	if (type == ComboType::single) 
		return max;

	if (type == ComboType::pair)
		return max;

	if (type == ComboType::bomb)
		return max;

	if (type == ComboType::rocket)
		return 0;

	if (type == ComboType::threeWith) {
		for (int i = 0; i < 15; i++) if (combo[i] == 3)
			return i;
		throw std::runtime_error("bad three with");
	}

	if (type == ComboType::singleContinous)
		return min;

	if (type == ComboType::doubleContinous)
		return min;

	if (type == ComboType::fourWith) {
		for (int i = 14; i >= 0; i--) if (combo[i] == 4)
			return i;
		throw std::runtime_error("bad four with");
	}

	if (type == ComboType::aircraft) 
		return min;

	if (type == ComboType::aircraftWithSingle) {
		for (int i = 14; i >= 0; i--) if (combo[i] >= 3)
			return i;
		throw std::runtime_error("bad aircraftWithSingle");
	}

	if (type == ComboType::aircraftWithPair) {
		for (int i = 14; i >= 0; i--) if (combo[i] == 3)
			return i;
		throw std::runtime_error("bad aircraftWithSingle");
	}

	throw std::runtime_error("bad combo");
}

bool LandlordCore::isBigger(const PokerGroup &lhs, const PokerGroup &rhs) const {
	ComboMeta lhsCombo = lhs.toComboMeta(),
			  rhsCombo = rhs.toComboMeta();

	ComboType lhsType = getComboType(lhsCombo),
			  rhsType = getComboType(rhsCombo);

	if (lhsType == ComboType::undefined)
		return false;

	if (rhsType == ComboType::undefined)
		return true;

	int lhsSubType = getSubComboType(lhsCombo, lhsType),
		rhsSubType = getSubComboType(rhsCombo, rhsType);

	int lhsValue = getComboValue(lhsCombo, lhsType),
		rhsValue = getComboValue(rhsCombo, rhsType);

	if (lhsType == ComboType::rocket)
		return true;

	if (rhsType == ComboType::rocket)
		return false;

	if (lhsType == ComboType::bomb && rhsType == ComboType::bomb)
		return lhsValue > rhsValue;

	if (lhsType == ComboType::bomb)
		return true;

	if (rhsType == ComboType::bomb)
		return false;

	if (lhsType != rhsType || lhsSubType != rhsSubType)
		return false;

	return lhsValue > rhsValue;
}

