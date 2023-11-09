CC = gcc
CFLAGS += -Wall -g
INCLUDES = -I ./includes/
LIBS = -lpthread

SRC_DIR := ./src
OBJ_DIR := ./obj
BUILD_DIR := ./build
HEADERS_DIR = ./includes/

TARGETS = farm collector

.DEFAULT_GOAL := all

OBJS-FARM = obj/master.o obj/workerPool.o obj/dirFiles.o obj/farm.o
OBJS-COLLECTOR = obj/master.o obj/workerPool.o obj/collector.o

obj/dirFiles.o:
	$(CC) $(CFLAGS) $(INCLUDES) -c src/dirFiles.c $(LIBS)
	@mv dirFiles.o $(OBJ_DIR)/dirFiles.o

obj/master.o:
	$(CC) $(CFLAGS) $(INCLUDES) -c src/master.c $(LIBS)
	@mv master.o $(OBJ_DIR)/master.o

obj/workerPool.o:
	$(CC) $(CFLAGS) $(INCLUDES) -c src/workerPool.c $(LIBS)
	@mv workerPool.o $(OBJ_DIR)/workerPool.o

obj/collector.o:
	$(CC) $(CFLAGS) $(INCLUDES) -c src/collector.c $(LIBS)
	@mv collector.o $(OBJ_DIR)/collector.o

obj/farm.o:
	$(CC) $(CFLAGS) $(INCLUDES) -c src/farm.c $(LIBS)
	@mv farm.o $(OBJ_DIR)/farm.o

farm: $(OBJS-FARM)
	$(CC) $(CFLAGS) -o $(BUILD_DIR)/farm $(OBJS-FARM) $(LIBS)

collector: $(OBJS-COLLECTOR)
	$(CC) $(CFLAGS) -o $(BUILD_DIR)/collector $(OBJS-COLLECTOR) $(LIBS)

.PHONY: clean cleanall all
all: $(TARGETS)
clean cleanall:
	rm -rf $(BUILD_DIR)/* $(OBJ_DIR)/* *.sk
	@touch $(BUILD_DIR)/.keep
	@touch $(OBJ_DIR)/.keep