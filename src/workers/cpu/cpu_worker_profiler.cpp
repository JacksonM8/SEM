#include "cpu_worker.h"
#include <chrono>
#include <iostream>

#define RUN_COUNT 1000

std::string dblquotewrap(std::string val){
    std::string quote("\"");
    return quote + val + quote;
}
std::string tab(int i = 1){
    return std::string(i, '\t');
}

std::string json_pair(std::string key, std::string val){
    return dblquotewrap(key) + ": " + dblquotewrap(val);
}

int main(){
    Cpu_Worker worker(0, "cpu_worker");

    auto start = std::chrono::steady_clock::now();
    
    int result = worker.MWIP(RUN_COUNT);
    auto end = std::chrono::steady_clock::now();
    auto ms = std::chrono::duration_cast<std::chrono::microseconds>(end - start);

    std::string newline("\n");

    std::string output = "{" + newline + tab() + json_pair("MWIP_microseconds", std::to_string(ms.count()/RUN_COUNT)) + newline + "}";

    std::cout << output << std::endl;

}