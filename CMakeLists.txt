cmake_minimum_required(VERSION 3.21)
project(tinyDistributeCache)

set(CMAKE_CXX_STANDARD 17)

# 将googletest包添加到项目里后，修改Cmake添加gtest支持
add_subdirectory(./googletest)
set(LIBRARIES gtest pthread) # pthread必须添加，因为gtest需要其支持

add_executable(tinyDistributeCache main.cpp service.cpp common/ConsistentHash.cpp common/ConsistentHash.h cache/LRU.cpp cache/LRU.h cache/ThreadPool.cpp cache/ThreadPool.h cache/Task.cpp cache/Task.h test/test_thread_pool.cpp)

# 将项目链接到getest上，gtestTest为项目名称，LIBRARIES为之前set的库名称
target_link_libraries(tinyDistributeCache ${LIBRARIES})