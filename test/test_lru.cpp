//
// Created by Chenglinli on 2022/4/29.
//

#include "gtest/gtest.h"
#include "../cache/LRU.h"
namespace{
    LRUCache LC(10);
    TEST(LRUCache, get){
        EXPECT_STREQ("", LC.get("0").data());
        int key_val=0;
        int value_val = 100;
        for(int i=0; i < 15; i++){
            LC.put(std::to_string(key_val++), std::to_string(value_val++));
            if(i>=1)
                EXPECT_STREQ("101", LC.get("1").data());
            else{
                EXPECT_STREQ("", LC.get("1").data());
            }
        }
        EXPECT_STREQ("", LC.get("0").data());
        EXPECT_STREQ("101", LC.get("1").data());
        EXPECT_STREQ("114", LC.get("14").data());
        EXPECT_EQ(10, LC.size());
    }
}
int main(int argc, char** argv){
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
    return 0;
}

