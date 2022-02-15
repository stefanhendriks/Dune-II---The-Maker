#pragma once

class cRandomMapGenerator {
	public:
		cRandomMapGenerator();

		void generateRandomMap(int startingPoints);

	private:

    void drawProgress(float progress) const;
};
