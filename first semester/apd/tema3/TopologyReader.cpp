/*
 * TopologyReader.cpp
 *
 *  Created on: Dec 23, 2015
 *      Author: arotaru
 */

#include "TopologyReader.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <iterator>
#include <unistd.h>
#include <cstring>

using namespace std;

namespace tema3 {

TopologyReader::TopologyReader() { }

TopologyReader::~TopologyReader() { }

Topology* TopologyReader::GetTopologyFromFile(const std::string& path, int rank, int size) {
	std::ifstream in(path.c_str(), std::ifstream::in);
	std::string line;
	for(int i = 0; i < rank; i++) {
		std::getline(in, line);
	}
	std::getline(in, line);
	in.clear();
	in.close();
	Topology *currTopology = new Topology(size);

	std::stringstream ss(line.substr(2));
	int neigh = 0;
	while(!ss.eof()) {
		ss >> neigh;
		currTopology->SetValue(rank, neigh, 1);
	}
	return currTopology;
}

} /* namespace tema3 */
