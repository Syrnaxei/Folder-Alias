#include <iostream>
#include <windows.h>

std::string getExeDir() {
    char path[260];
    GetModuleFileNameA(nullptr,path,260);
    std::string spath(path);
    return spath.substr(0,spath.find_last_of('\\'));
}

int main() {
    std::string s = getExeDir();
    std::cout<< s <<std::endl;
}