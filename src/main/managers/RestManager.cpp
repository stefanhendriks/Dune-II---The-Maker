#include "allegro.h"

#include "RestManager.h"

RestManager::RestManager(int theIdealFps) {
	restInMs = 0;
	idealFps = theIdealFps;
}

RestManager::~RestManager() {
}

void RestManager::giveCpuSomeSlackIfNeeded() {
	if (restInMs > 0) {
		rest(restInMs);
	}
}

bool RestManager::isGivenFpsLessThanIdeal(int fps) {
	return fps < idealFps;
}

void RestManager::evaluateRestValueWithGivenFPS(int fps) {
	if (isGivenFpsLessThanIdeal(fps)) {
		decreaseRest(6);
	} else {
		increaseRest(2);
	}
}

void RestManager::decreaseRest(int amount) {
	restInMs -= amount;
	if (restInMs < 0) restInMs = 0;
}

void RestManager::increaseRest(int amount) {
	restInMs += amount;
	if (restInMs > 500) restInMs = 500;
}
