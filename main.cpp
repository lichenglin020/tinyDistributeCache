#include <iostream>
//#include "common/ConsistentHash.h"
#include <string>

using namespace std;

int main() {

//    cout << __FILE__ << endl;

    printf("%s\n", __FILE__); // /tmp/tmp.GBvw7qqoCE
    cout << "尝试" << endl;
    string fileName = "../log/testlog6.log";
//    string fileName = "/Users/chenglinli/CLionProjects/tinyDistributeCache/log/testlog1.log";
    auto file = fopen(fileName.c_str(), "a+");
    if(file == nullptr){
        cout << "创建失败";
        return 0;
    }
    string line = "this is a test in this project.";
    if(fwrite((void*)line.c_str(), 1, line.length(), file) != line.size()){
        cout << "写入失败";
    }
    // 刷新输出流(必须要刷新，否则输出可能为空)
    fflush(file);
    fclose(file);


//    std::cout << "ok" << std::endl;
//    ConsistentHash consistentHash(100);
//    std::cout << "TestBalance: " << std::endl;
//    consistentHash.TestBalance(30000);
//    std::cout << std::endl;
//    std::cout << "TestAdd" << std::endl;
//    consistentHash.TestAdd();
//    std::cout << std::endl;
//    std::cout << "TestDelete" << std::endl;
//    consistentHash.TestDelete();
//    std::cout << std::endl;
//    std::cout << "TestBackUp" << std::endl;
//    consistentHash.TestGetNextIndex();
    return 0;
}
