/*
 * cRandomMapGenerator.h
 *
 *  Created on: 16 nov. 2010
 *      Author: Stefan
 */

#ifndef CRANDOMMAPGENERATOR_H_
#define CRANDOMMAPGENERATOR_H_

class cRandomMapGenerator {
	public:
		cRandomMapGenerator();
		virtual ~cRandomMapGenerator();

		void generateRandomMap();

	private:

    void drawProgress(float progress) const;
};

#endif /* CRANDOMMAPGENERATOR_H_ */
