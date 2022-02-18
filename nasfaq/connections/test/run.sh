clang++  ../common/common.cpp \
	../parser/parser.cpp \
	../safe_queue/safe_queue.cpp \
	../http/http_connector.cpp \
	../ws/my_ssl.cpp \
	./main.cpp \
	-o main -lcurl -lcrypto -lssl -lboost_system -lboost_random -lboost_thread -lpthread && ./main
