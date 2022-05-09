//
// Created by Chenglinli on 2022/5/2.
//

#include "CacheData.h"

CacheData::CacheData(std::shared_ptr<ThreadPool> threadPool,
                     std::shared_ptr<LRUCache> lruCache,
                     std::shared_ptr<LRUCache> lruCacheBackup,
                     std::shared_ptr<ConsistentHash> keyAddrs,
                     int _listenNum,
                     int _epollFdSize,
                     int _readyEventsNum):
                     cacheDataThreadPool(threadPool),
                     listenNum(_listenNum),
                     epollFdSize(_epollFdSize),
                     readyEventsNum(_readyEventsNum){

//    this -> cacheDataThreadPool = threadPool;
    readyEvents = new epoll_event[readyEventsNum];
    serverRwStruct.lruCache = lruCache;
    serverRwStruct.lruCacheBackup = lruCacheBackup;
    serverRwStruct.keyAddrs = keyAddrs;
}

CacheData::~CacheData() {
    delete[] readyEvents;
}

/**
 * 创建并监听套接字
 */
void CacheData::listen() {
    // 创建服务端的监听套接字
    listenedServerSocket = createSocket(AF_INET, SOCK_STREAM, 0);
    // 服务端先断开连接时，端口复用，跳过TIME_WAIT状态
    setReuseAddr(listenedServerSocket);

    // 绑定服务端套接字，并进行监听
    bzero(&serverSocketAddr, sizeof serverSocketAddr);
    serverSocketAddr.sin_family = AF_INET;
    serverSocketAddr.sin_addr.s_addr = inet_addr(CACHESERV_IP);
    serverSocketAddr.sin_port = htons(CACHESERV_PORT);
    bindSocket(listenedServerSocket, (sockaddr*)&serverSocketAddr, sizeof(serverSocketAddr));
    listenSocket(listenedServerSocket, listenNum);

    // 创建epoll I/O复用，并添加监听
    epollFd = epollCreate(epollFdSize);
    epollEvent.events = EPOLLIN | EPOLLET;
    epollEvent.data.fd = listenedServerSocket;
    epollCtl(epollFd, EPOLL_CTL_ADD, listenedServerSocket, &epollEvent);
    serverRwStruct.epollFd = epollFd;
    serverAcceptStruct.epollFd = epollFd;
}

/**
 * epoll wait,等待事件触发
 * @param microsecond
 * @return
 */
int CacheData::wait(int microsecond = 500) {
    return epollWait(epollFd, readyEvents, readyEventsNum, microsecond);
}


/**
 * epoll响应客户端连接套接字是的线程操作函数
 * @param serverAcceptStruct
 */
void acceptTaskThreadFunc(void* serverAcceptStruct){
    pthread_rwlock_rdlock(&shutdown_lock);
    ConnectTask connectTask((ServerAcceptStruct*)serverAcceptStruct);
    connectTask.accept();
    delete (ServerAcceptStruct*)serverAcceptStruct;
    pthread_rwlock_unlock(&shutdown_lock);
}

/**
 * 当epoll触发事件是accept客户端连接时的处理逻辑
 * 将具体的连接处理操作，交给线程池处理
 */
void CacheData::acceptHandler() {
    std::unique_lock<std::mutex> lck(taskMutex);
//    serverAcceptStruct.clientAddr = &clientSocketAddr;
//    serverAcceptStruct.listendFd = listenedServerSocket;
//    serverAcceptStruct.number++;
    ServerAcceptStruct* tmp = new ServerAcceptStruct();
    tmp -> epollFd = epollFd;
    tmp -> number = serverAcceptStruct.number++;
    tmp -> listendFd = listenedServerSocket;
    tmp -> clientAddr = new sockaddr_in();

    Task task(acceptTaskThreadFunc, tmp);
    cacheDataThreadPool ->addTask(task);
    lck.unlock();

    logFile.LOGINFO("add accept socket to epoll tree handler to thread pool.");

}

/**
 * epoll响应客户端读写请求的线程操作函数
 * @param serverRwStruct
 */
void readAndWriteTaskThreadFunc(void* serverRwStruct){
    pthread_rwlock_rdlock(&shutdown_lock);
    PutGetTask putGetTask((ServerRWStruct *)serverRwStruct);
//    while(true){
        ssize_t n = putGetTask.readFromClient();
//        std::cout << n << std::endl;
        if(n == 0){
            putGetTask.closeConnect();
//            break;
        }else if(n > 0){
            auto type = putGetTask.getInfoType();
            if(type == KEY_VALUE_READ) {
                logFile.LOGINFO("receive KEY_VALUE_REQUEST from client");
                putGetTask.kvReadHandler();
            }else if(type == KEY_VALUE_WRITE) {
                std::string info = "receive KEY_VALUE_RESPOND" + std::to_string(type);
                logFile.LOGINFO(info.c_str());
                putGetTask.kvWriteHandler();
            }else if(type == KEY_VALUE_WRITE_BK){
                std::string info = "receive KEY_VALUE_RESPONDBK" + std::to_string(type);
                logFile.LOGINFO(info.c_str());
                putGetTask.kvWriteHandler();
            }else if(type == REFLESH_MASTER){
                pthread_mutex_lock(&reflesh_master_lock);
                master_addr = putGetTask.refleshMaster();
                pthread_mutex_unlock(&reflesh_master_lock);
            }
//            else{
//                break;
//            }
        }
//        else{
//            break;
//        }
//    }

    delete (ServerRWStruct *)serverRwStruct;
    pthread_rwlock_unlock(&shutdown_lock);
}

/**
 * 当epoll触发事件是读写事件时的处理逻辑
 * 将具体的读写处理操作，交给线程池处理
 */
void CacheData::readAndWriteHandler(int index) {
    std::unique_lock<std::mutex> lck(taskMutex);
//    serverRwStruct.buffer = buffer;
//    serverRwStruct.clientSocket = readyEvents[index].data.fd;
    ServerRWStruct* tmp = new ServerRWStruct();
    tmp -> buffer = new char[BUFSIZ];
    tmp -> epollFd = epollFd;
    tmp -> keyAddrs = serverRwStruct.keyAddrs;
    tmp -> lruCache = serverRwStruct.lruCache;
    tmp -> lruCacheBackup = serverRwStruct.lruCacheBackup;
    tmp -> clientSocket = readyEvents[index].data.fd;
    Task task(readAndWriteTaskThreadFunc, tmp);
    cacheDataThreadPool ->addTask(task);
    lck.unlock();

    logFile.LOGINFO("add read and write handler to thread pool.");
}

/**
 * 用于判断是和是连接事件
 * @param socket 文件描述符
 * @return
 */
bool CacheData::isListenedFd(int socket) {
    return readyEvents[socket].data.fd == listenedServerSocket;
}

/**
 * 启动并运行Cache服务
 */
void CacheData::run() {
    listen();
    for(;;){
        int nready = wait(500);
        if(nready <= 0){
            continue;
        }

        pthread_rwlock_rdlock(&shutdown_lock);
        for(int i = 0; i < nready; i++){
            if(isListenedFd(i)){
                logFile.LOGINFO("receive new connect");
                acceptHandler();
            }else{
                logFile.LOGINFO("receive new read or write request");
                readAndWriteHandler(i);
            }

        }
        pthread_rwlock_unlock(&shutdown_lock);
    }
}

