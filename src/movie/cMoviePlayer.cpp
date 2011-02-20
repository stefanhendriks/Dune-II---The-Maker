#include "allegroh.h"

#include "cMoviePlayer.h"

cMoviePlayer::cMoviePlayer(DATAFILE *movie) {
	data = movie;
	playing = false;
	frame = 0;
	frameTimer = 0;
}

cMoviePlayer::~cMoviePlayer() {
	data = NULL;
	frame = 0;
	frameTimer = 0;
	playing = false;
}

bool cMoviePlayer::isAtTheEndOfMovie() {
	return data &&
		   frame > -1 &&
		   (data[frame].type == DAT_END ||
			data[frame].type != DAT_BITMAP);
}

bool cMoviePlayer::shouldPlayMovie() {
	return playing && data;
}

void cMoviePlayer::think() {
	if (shouldPlayMovie()) {
		frameTimer++;

		if (frameTimer > 20) {
			frameTimer = 0;
			frame++;

			if (isAtTheEndOfMovie()) {
				frame = 0;
			}
		}
	}
}

BITMAP * cMoviePlayer::getFrameToDraw() {
	if (data == NULL) {
		return NULL;
	}
	return (BITMAP *)data[frame].dat;
}
