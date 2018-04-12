#ifndef __MAPH
#define __MAP_H

#include <vector>
#include <string>
#include "glm/glm/glm.hpp"
#include "glm/glm/gtc/matrix_transform.hpp"
#include "glm/glm/gtc/type_ptr.hpp"
#include "const.h"

class MapLoader {
private:
	int map[COLUMNS][ROWS];
public:
	MapLoader();
	MapLoader(std::string filepath);
	void getMap(std::vector<glm::vec3> &mp);

};


#endif
