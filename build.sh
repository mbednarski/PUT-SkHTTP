#!/bin/bash

mkdir -p bin

g++ ./src/server/main.cpp ./src/server/Task.cpp ./src/server/ThreadPool.cpp \
./src/server/Settings.cpp ./src/server/HttpException.cpp ./src/server/ContentStream.cpp \
./src/server/HttpResponse.cpp ./src/server/HttpUtil.cpp ./src/server/HttpServerTask.cpp \
./src/server/util.cpp ./src/server/HttpRequest.cpp ./src/server/Logger.cpp ./src/server/MtLoader.cpp \
-Wall -Wextra -lpthread -std=c++11 -g -o ./bin/skhttpserver
