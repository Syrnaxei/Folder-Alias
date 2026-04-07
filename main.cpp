#include <iostream>
#include <string>
#include <fstream>
#include <unordered_map>
#include <windows.h>
#include <shellapi.h>

#pragma comment(lib, "Shell32.lib")
std::string getExeDir() {
    char path[MAX_PATH];
    GetModuleFileNameA(nullptr,path,MAX_PATH);
    std::string spath(path);
    return spath.substr(0,spath.find_last_of('\\'));
}

void loadMappings(const std::string& filename,std::unordered_map<std::string,std::string>& map) {
    std::ifstream file(filename);

    if (!file.is_open()) {
        std::ofstream createFile(filename);
        if (createFile.is_open()) {
            createFile.close();
        }else {
            std::cout << "文件读取错误" << filename << std::endl;
        }
    }

    std::string line;
    while (std::getline(file,line)) {
        size_t pos = line.find('=');
        if (pos != std::string::npos) {
            map[line.substr(0, pos)] = line.substr(pos + 1);
        }
    }
    file.close();
}

void saveMappings(const std::string& filename,std::unordered_map<std::string,std::string>& map) {
    std::ofstream file(filename);
    for (const auto& pair : map) {
        file << pair.first << "=" << pair.second << '\n';
    }
    file.close();
}

int main(int argc,char* argv[]) {
    if (argc < 2) {
        std::cout << "Enter 'fa help' to get commands used." <<std::endl;
        return 1;
    }

    std::string dbFile = getExeDir() + "\\fa_mappings.txt";
    std::unordered_map<std::string,std::string> mappings;
    loadMappings(dbFile,mappings);
    std::string arg1 = argv[1];

    if (argc == 2) {
        //打开资源管理器
        auto it = mappings.find(arg1);
        if (it != mappings.end()) {
            ShellExecuteA(
                nullptr,
                "explore",
                it->second.c_str(),
                nullptr,
                nullptr,
                SW_SHOWNORMAL
                );
            std::cout << "success" <<std::endl;
        }

        return 0;
    }

    if (argc == 3) {
        std::string alias = argv[2];
        mappings.erase(alias);
        saveMappings(dbFile,mappings);
        std::cout << "success" <<std::endl;
    }

    if (argc == 4) {
        std::string alias = argv[2];
        mappings[alias] = argv[3];
        saveMappings(dbFile,mappings);
        std::cout << "success" <<std::endl;
    }

    return 0;
}