#include <iostream>
#include "probab.h"
#include <fstream>
#include <ctime>
#include <unordered_set>

/*
size_t getMemoryUsage(const std::unordered_map<std::string, uint64_t>& map) {
  auto entrySize = sizeof(std::string) + sizeof(uint64_t) + sizeof(void*);
  auto bucketSize = sizeof(void*);
  auto adminSize = 3 * sizeof(void*) + sizeof(uint64_t);

  auto totalSize = adminSize + map.size() * entrySize + map.bucket_count() * bucketSize;
  return totalSize;
}

void test1()
{
    std::vector<std::string> a = {"T","R", "U","P","E","V", "Z","X","S","G","L","K","E","Q","W","WA","WSA","FGTR","REFDS","GERFDF","grrt"};
    std::vector<std::string> chr;
    
    for(int i = 0;i < 10000;i ++){
        chr.push_back(a[std::rand()%21]);
        chr.push_back("Fa");
        chr.push_back("Ag");
    }
     
     std::ofstream fl("C:\\Projects\\BloomFilter\\build\\out0.txt");
     fl.precision(10);
     fl << "size accurancy\n"; 
     std::unordered_map<std::string, uint64_t> st;
     for(int i = 0;i < 3000;i++){
        st[chr[i]]++;
     }
   
    for(uint32_t j = 1; j < 100;j++){
        cmsketch<std::string, 40, 1> sk(j);

        double acc = 0;     
        for(int i = 0;i < 3000;i++){
            sk.insert(chr[i]);
        }
        for(auto pr : st)
        {   
            acc += static_cast<double>(sk.count(pr.first) - pr.second) / static_cast<double>(sk.count(pr.first));

        }
        acc/=st.size();
        fl << j << " " << 1-acc << "\n";
    }

            

     fl.close();
}*/

int main()
{
   // test1();

}
