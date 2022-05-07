#ifndef _GRAPH_H_
#define _GRAPH_H_

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>

#include "../connections/common/common.h"

#include <graphviz/gvc.h>
#include <graphviz/cgraph.h>

//template <typename T>
void make_graph_from_map(std::map<std::string, std::map<std::string, float>>);

#endif

