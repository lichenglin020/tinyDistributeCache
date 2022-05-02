cmake_minimum_required(VERSION 3.16)
project(tinyDistributeCache)

set(CMAKE_CXX_STANDARD 17)

# 将googletest包添加到项目里后，修改Cmake添加gtest支持
add_subdirectory(./googletest)
set(LIBRARIES gtest pthread) # pthread必须添加，因为gtest需要其支持

add_executable(tinyDistributeCache
        main.cpp
        common/threadpool/Task.cpp
        common/threadpool/Task.h
        common/threadpool/ThreadPool.cpp
        common/threadpool/ThreadPool.h
        common/ConsistentHash.cpp
        common/ConsistentHash.h
        cache/LRU.cpp
        cache/LRU.h
        common/LogFile.cpp
        common/LogFile.h
        common/EpollWrap.h)
add_executable(test_log_file
        common/LogFile.cpp
        common/LogFile.h
        test/test_log_file.cpp)
add_executable(test_lru
        cache/LRU.h
        cache/LRU.cpp
        test/test_lru.cpp)
add_executable(test_thread_pool
        common/threadpool/Task.h
        common/threadpool/Task.cpp
        common/threadpool/ThreadPool.h
        common/threadpool/ThreadPool.cpp
        test/test_thread_pool.cpp)

# 将项目链接到getest上，gtestTest为项目名称，LIBRARIES为之前set的库名称
target_link_libraries(test_lru ${LIBRARIES})
target_link_libraries(test_thread_pool ${LIBRARIES})