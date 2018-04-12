#include "map.h"
#include <string>
#include <fstream>
#include <iostream>


MapLoader::MapLoader() {}

//	Read in the file
MapLoader::MapLoader(std::string filePath) {

	std::ifstream MapFile(filePath);

	if (MapFile)
		for (int i = 0; i < COLUMNS; i++) {
			for (int j = 0; j < ROWS; j++) {
				MapFile >> map[i][j];
				std::cout << map[i][j];
			}
			std::cout << "\n";
		}

	else std::cout << "Can't load map\n";
}

void MapLoader::getMap(std::vector<glm::vec3> &mp)
{
	for (int i = 0; i < COLUMNS; i++) {
		for (int j = 0; j < ROWS; j++) {
			if (map[i][j] != 0)
				mp.push_back(glm::vec3((float)j*-1, (float)i*-1, 0.0f));
		}
	}
}