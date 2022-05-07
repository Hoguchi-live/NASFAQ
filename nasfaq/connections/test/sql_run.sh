g++	../common/common.cpp \
 	../sql/db_init.cpp \
 	../sql/db_handle.cpp \
	../../maths/maths.cpp \
	../../maths/graph.cpp \
	sql.cpp \
	-L/usr/lib -lgvc -lcgraph  -lfmt -lpqxx -lpq  -o sql && ./sql
