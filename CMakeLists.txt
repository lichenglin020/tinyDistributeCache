cmake_minimum_required(VERSION 3.21)
project(tinyDistributeCache)

set(CMAKE_CXX_STANDARD 17)

add_executable(tinyDistributeCache main.cpp service.cpp common/ConsistentHash.cpp common/ConsistentHash.h cache/LRU.cpp cache/LRU.h cache/ThreadPool.cpp cache/ThreadPool.h cache/Task.cpp cache/Task.h)
