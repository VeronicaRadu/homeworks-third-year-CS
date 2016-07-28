/*
 * TopologyReader.h
 *
 *  Created on: Dec 23, 2015
 *      Author: arotaru
 */

#ifndef TOPOLOGYREADER_H_
#define TOPOLOGYREADER_H_

#include "Topology.h"

namespace tema3 {

class TopologyReader {
public:
	TopologyReader();
	virtual ~TopologyReader();
	Topology* GetTopologyFromFile(const std::string&, int, int);
};

} /* namespace tema3 */

#endif /* TOPOLOGYREADER_H_ */
