#ifndef UNIT_UTILS
#define UNIT_UTILS
class cUnitUtils {

private:
	static cUnitUtils *instance;

protected:
	cUnitUtils();

public:

	static cUnitUtils *getInstance();

	int findUnit(int type, int iPlayerId); // find the first unit of type belonging to player Id.
	int findUnit(int type, int iPlayerId, int iIgnoreUnitId);

};

#endif