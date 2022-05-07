#include "graph.h"

//template <typename T>
void make_graph_from_map(std::map<std::string, std::map<std::string, float>> op) {
	#ifndef NO_LAYOUT_OR_RENDERING
	// set up a graphviz context - but only once even for multiple graphs
	GVC_t *gvc = gvContext();
	#endif

	// Create a simple digraph
	Agraph_t *g = agopen("g", Agundirected, 0);

	// Placeholders
	Agnode_t *parent;
	Agnode_t *child;
	char float_buffer[64];

	// Add vertices and edges
	for (auto& userid1_map:op) {
		parent = agnode(g, (char *)(userid1_map.first).c_str(), 1);
		agsafeset(parent, "label", (char *)(userid1_map.first).c_str(), "");
		std::cout << "Parent: " << userid1_map.first << std::endl;
		for (auto& element:userid1_map.second) {
			child = agnode(g, (char *)(element.first).c_str(), 1);
			agsafeset(parent, "label", (char *)(element.first).c_str(), "");

			//std::cout << "    Child: " << element.first << "    Dist: " <<  element.second << std::endl;
			Agedge_t *e = agedge(g, parent, child, 0, 1);

			// Convert float to char*
			snprintf(float_buffer, sizeof(float_buffer), "%f", element.second);
			agsafeset(e, "label", float_buffer, "");
		}
	}

	// Set an attribute - in this case one that affects the visible rendering
	//agsafeset(n, "color", "red", "");

	#ifdef NO_LAYOUT_OR_RENDERING
	// Just write the graph without layout
	agwrite(g, savefile);
	#else
	// Use the directed graph layout engine
	gvLayout(gvc, g, "dot");

	// Output in .dot format
	FILE *fptr = fopen("graph.dot", "w");
	gvRender(gvc, g, "dot", fptr);
	fclose(fptr);

	gvFreeLayout(gvc, g);
	#endif

	agclose(g);
}
