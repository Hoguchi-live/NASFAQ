g++	../../common/common.cpp \
	../../aux/complete_db.cpp \
	../../http/http_connector.cpp \
	../../http/users.cpp \
	../../sql/db_init.cpp \
	../../sql/db_handle.cpp \
	./test_complete_db.cpp \
	-o test_complete_db -lpqxx -lpq -lfmt -lcurl -lcrypto -lssl -lboost_system -lboost_random -lboost_thread -lpthread && ./test_complete_db
