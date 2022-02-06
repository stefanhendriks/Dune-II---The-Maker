#pragma once

#include "cMap.h"
#include "structs.h"

#include <functional>

class cRandomMapGenerator {
  public:
	cRandomMapGenerator(cMap& map, s_PreviewMap& preview);

	void generateRandomMap(int startingPoints, std::function<void(float)> onProgress);

  private:
    cMap& m_map;
    s_PreviewMap& m_preview;
};
