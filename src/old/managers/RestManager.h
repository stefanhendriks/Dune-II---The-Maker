#ifndef RESTMANAGER_H_
#define RESTMANAGER_H_

class RestManager {
	public:
		RestManager(int theIdealFps);
		~RestManager();

		void evaluateRestValueWithGivenFPS(int fps);
		void giveCpuSomeSlackIfNeeded();

	protected:
		void decreaseRest(int amount);
		void increaseRest(int amount);
		bool isGivenFpsLessThanIdeal(int fps);

	private:
		int restInMs;
		int idealFps;

};

#endif /* RESTMANAGER_H_ */
