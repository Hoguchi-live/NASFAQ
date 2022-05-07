gcc 	../src/common.c \
	../src/mute.c \
	../src/performance.c \
	../src/settings.c \
	../src/items.c \
	../src/cJSON/cJSON.c \
	./main.c \
	-Wall -g -o test && ./test
