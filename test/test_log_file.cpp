//
// Created by Chenglinli on 2022/4/28.
//

#include "../common/LogFile.h"
#include <pthread.h>
//#include <thread>

void* producer_thread_proc(void* arg1)
{
    LogFile* arg = (LogFile*)arg1;
    int index = 0;
    while (true)
    {
        index++;
        if (index == 10) break;

        arg -> LOGINFO("id=%d, gg=%d,hh=%d,ff=%s",index,12,10,"sdfa");
        arg -> LOGWARNING("id=%d, gg=%d,hh=%d,ff=%s",index,11,10,"abcd");
//        sleep(0.1);
//        std::this_thread::sleep_for(100ms);
    }
    return NULL;
}

int main()
{
    LogFile logfile(INFO,500);
    std::string filename = "../log/my.log";
    logfile.openLogFile(filename);
    std::vector<std::thread> vec;
    int threadNum = 3;
    for (int i=0; i<threadNum; i++){
        vec.push_back(std::thread(producer_thread_proc, &logfile));
    }
//    std::thread consumer(LogFile::writeToFile, &logfile);
    logfile.run();
    for (int i=0; i<threadNum; i++){
        vec[i].join();
    }
    logfile.join();
//    consumer.join();
    return 0;
}