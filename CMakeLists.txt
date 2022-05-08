cmake_minimum_required(VERSION 3.16)

# aliyun的g++9编译环境，如果想使用本地编译环境或者docker编译环境，需要将下面的一段注释掉
# 注意这段命令必须放在project之前（否则会出现死循环或者不生效的问题）
# 注意再切换编译环境后，需要清空cmake的缓存，否则可能出错
#SET(CMAKE_CXX_COMPILER "/opt/rh/devtoolset-9/root/usr/bin/g++") # 使用SCL

project(tinyDistributeCache)

set(CMAKE_CXX_STANDARD 17)

# 将googletest包添加到项目里后，修改Cmake添加gtest支持
add_subdirectory(./googletest)
set(LIBRARIES gtest pthread) # pthread必须添加，因为gtest需要其支持

# 在linux在需要使用c++11多线程会出现问题，所需要添加的支持
# 否则会出现Enable multithreading to use std::thread: Operation not permitted错误
SET(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} "-std=c++17 -pthread")

# 添加一些系统设置信息
message(STATUS "CMAKE_SYSTEM_NAME          = ${CMAKE_SYSTEM_NAME}"         )
message(STATUS "CMAKE_CXX_COMPILER_ID      = ${CMAKE_CXX_COMPILER_ID}"     )
message(STATUS "CMAKE_CXX_COMPILER_VERSION = ${CMAKE_CXX_COMPILER_VERSION}")
message(STATUS "CMAKE_COMPILER_IS_GNUCXX   = ${CMAKE_COMPILER_IS_GNUCXX}"  )

add_executable(tinyDistributeCache
        common/threadpool/Task.cpp
        common/threadpool/Task.h
        common/threadpool/ThreadPool.cpp
        common/threadpool/ThreadPool.h
        common/ConsistentHash.cpp
        common/ConsistentHash.h
        common/LogFile.cpp
        common/LogFile.h
        common/EpollWrap.h
        cache/LRU.cpp
        cache/LRU.h
        cache/ConnectTask.cpp
        cache/ConnectTask.h
        cache/PutGetTask.cpp
        cache/PutGetTask.h
        cache/CacheServer.cpp
        cache/CacheServer.h
        cache/CacheData.cpp
        cache/CacheData.h
        common/EpollWrap.cpp
        common/Tools.cpp
        common/Tools.h)
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
add_executable(test main.cpp)
add_executable(test_consistentHash
        common/ConsistentHash.cpp
        common/ConsistentHash.h
        test/test_consistentHash.cpp)

# 将项目链接到getest上，gtestTest为项目名称，LIBRARIES为之前set的库名称
target_link_libraries(test_lru ${LIBRARIES})
target_link_libraries(test_thread_pool ${LIBRARIES})
