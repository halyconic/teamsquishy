/*
 * graph.cpp
 *
 *  Created on: Dec 9, 2012
 *      Author: swebber
 */

#include "graph.h"
#include "topology.h"

GraphManager::GraphManager(char* filename, bool debug)
{
	/*
	 * Iterate across entries
	 */

	readtopology(filename, debug);
}
