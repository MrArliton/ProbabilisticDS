#include <iostream>
#include "probab.h"

int main()
{
    
    skiplist<int> skp({1,2});

    std::cout << "\n" << skp << "\n";
    for(int64_t i = 0;i < 20;i++){
        std::cout << skp.contains(i) << " ";
    }
    std::cout << "\n" << skp << "\n";
}