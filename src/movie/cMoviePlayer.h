#ifndef CMOVIEPLAYER_H_
#define CMOVIEPLAYER_H_

class cMoviePlayer {
	public:
		cMoviePlayer(DATAFILE *movie);
		~cMoviePlayer();

		void setPlaying(bool value) { playing = value; }
		bool isPlaying() { return playing; }

		void setFrame(int value) { frame = value; }

		void think();
		BITMAP * getFrameToDraw();

	protected:

	private:
		int frameTimer;
		bool playing;
		int frame;
		DATAFILE *data;
    bool shouldPlayMovie();
    bool isAtTheEndOfMovie();
};

#endif
