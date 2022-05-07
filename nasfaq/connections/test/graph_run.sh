g++	../common/common.cpp \
	../../maths/graph.cpp \
	graph.cpp \
	-L/usr/lib -lgvc -lcgraph  -lfmt -lpqxx -lpq -o graph && ./graph
