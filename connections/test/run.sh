g++  	../common/common.cpp \
  	../common/formatting.cpp \
	../http/http_connector.cpp \
	../http/users.cpp \
	../ws/ssl_ws.cpp \
	../sql/db_init.cpp \
	../sql/db_handle.cpp \
	../parser/parser_aux.cpp \
	../parser/parser.cpp \
	../ws/http_handshake.cpp \
	./main.cpp \
	-o main -lpqxx -lpq -lfmt -lcurl -lcrypto -lssl -lboost_system -lboost_random -lboost_thread -lpthread && ./main
