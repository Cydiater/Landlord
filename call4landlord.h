#ifndef CALL4LANDLORD_H
#define CALL4LANDLORD_H

#include "utils.h"

class Call4Landlord {
public:
	enum class status { call, pass, rob, undefined };

    Call4Landlord(PlayerType _candidate);
	
	void call(PlayerType player);
	void pass(PlayerType pass);
	void set(PlayerType, status w);
	bool hasCandidate() const;
	PlayerType getCandidate() const;

	static PlayerType random();

private:
	PlayerType candidate;
	status value[3];
};

#endif // CALL4LANDLORD_H
