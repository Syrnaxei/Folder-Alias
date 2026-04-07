#include <iostream>
#include <string>
#include <fstream>
#include <unordered_map>
#include <windows.h>
#include <shellapi.h>

#pragma comment(lib, "Shell32.lib")

#define FA_VERSION "0.0.2"

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
            std::cout << "file read error" << filename << std::endl;
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

void listMappings(const std::unordered_map<std::string,std::string>& map) {
    if (map.empty()) {
        std::cout << "No mappings found." << std::endl;
        return;
    }

    std::cout << "Alias mappings:" << std::endl;
    std::cout << "----------------------------------------" << std::endl;
    for (const auto& pair : map) {
        std::cout << pair.first << " -> " << pair.second << std::endl;
    }
    std::cout << "----------------------------------------" << std::endl;
    std::cout << "Total: " << map.size() << " mapping(s)" << std::endl;
}

enum class Command {
    VERSION,
    LS,
    OPEN
};

Command parseCommand(const std::string& cmd) {
    if (cmd == "version") return Command::VERSION;
    if (cmd == "ls") return Command::LS;
    return Command::OPEN;
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
        switch (parseCommand(arg1)) {
            case Command::VERSION:
                std::cout << "Folder-Alias v" << FA_VERSION << std::endl;
                break;
            case Command::LS:
                listMappings(mappings);
                break;
            case Command::OPEN: {
                auto it = mappings.find(arg1);
                if (it != mappings.end()) {
                    HINSTANCE hRet = ShellExecuteA(
                        nullptr,
                        "explore",
                        it->second.c_str(),
                        nullptr,
                        nullptr,
                        SW_SHOWNORMAL
                        );
                    if ((INT_PTR)hRet <= 32) {
                        std::cerr << "ShellExecuteA failed with code: " << (int)(INT_PTR)hRet << std::endl;
                    }else {
                        std::cout << "success" << std::endl;
                    }
                }else {
                    std::cout << "Cannot find object" << std::endl;
                }
                break;
            }
        }
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