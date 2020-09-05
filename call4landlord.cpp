#include <QRandomGenerator>

#include "call4landlord.h"

Call4Landlord::Call4Landlord(PlayerType _candidate) : candidate {_candidate} {
	for (int i = 0; i < 3; i++)
		value[i] = status::undefined;
}

void Call4Landlord::call(PlayerType player) {
	int index = static_cast<int>(player);
	assert(value[index] == status::undefined);
	value[index] = status::call;
	candidate = player;
}

void Call4Landlord::set(PlayerType player, status w) {
	if (w == status::call)
		call(player);
	else 
		pass(player);
}

void Call4Landlord::pass(PlayerType player) {
	int index = static_cast<int>(player);
	assert(value[index] == status::undefined);
	value[index] = status::pass;
}

bool Call4Landlord::hasCandidate() const {
	for (int i = 0; i < 3; i++)
		if (value[i] == status::undefined) 
			return false;
	return true;
}

PlayerType Call4Landlord::getCandidate() const {
	return candidate;
}

PlayerType Call4Landlord::random() {
	return static_cast<PlayerType>(QRandomGenerator::global() -> generate() % 3);
}
