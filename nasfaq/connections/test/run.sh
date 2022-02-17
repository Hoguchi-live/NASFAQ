clang++ ../http/http_connector.cpp \
	../ws/my_ssl.cpp \
	./main.cpp \
	-o main -lcurl -lcrypto -lssl -lboost_system -lboost_random -lboost_thread -lpthread && ./main
