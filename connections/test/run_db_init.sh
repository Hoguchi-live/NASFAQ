g++	../common/common.cpp \
 	../sql/db_init.cpp \
	./main_db_init.cpp \
	-L/usr/lib -lgvc -lcgraph  -lfmt -lpqxx -lpq  -o db_init && ./db_init
