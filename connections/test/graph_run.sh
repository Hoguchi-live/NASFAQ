g++	../common/common.cpp \
	../sql/db_init.cpp \
	../sql/db_handle.cpp \
	../../maths/graph.cpp \
	../../maths/maths.cpp \
	graph.cpp \
	-L/usr/lib -lgvc -lcgraph  -lfmt -lpqxx -lpq -o graph && ./graph
