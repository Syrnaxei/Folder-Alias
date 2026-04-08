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

//这个函数可以整个多态实现精准查找
void getHelp() {
    std::cout << "Folder-Alias" << "\n"
              << "\n"
              << "USAGE:\n"
              << "    fa <command> [arguments]\n"
              << "\n"
              << "COMMANDS:\n"
              << "    version                    Show version information\n"
              << "    ls                         List all alias mappings\n"
              << "    help                       Show this help message\n"
              << "    <alias>                    Open folder by alias name\n"
              << "\n"
              << "ARGUMENTS:\n"
              << "    fa rm <alias>              Remove an alias mapping\n"
              << "    fa add <alias> <path>      Add or update an alias mapping\n"
              << "\n"
              << "EXAMPLES:\n"
              << "    fa version                 Display current version\n"
              << "    fa ls                      Show all configured aliases\n"
              << "    fa projects                Open folder mapped to 'projects'\n"
              << "    fa add docs D:\\Documents   Create alias 'docs' for D:\\Documents\n"
              << "    fa rm docs                 Remove the 'docs' alias\n"
              << "\n"
              << "CONFIGURATION:\n"
              << "    Alias mappings are stored in: fa_mappings.txt (same directory as executable)\n"
              << "    Format: alias=path (one per line)\n"
              << "\n"
              << "NOTES:\n"
              << "    - Paths can be absolute or relative\n"
              << "    - Use the path copied directly in File Explorer\n";
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
    HELP,
    OPEN
};

Command parseCommand(const std::string& cmd) {
    if (cmd == "version") return Command::VERSION;
    if (cmd == "ls") return Command::LS;
    if (cmd == "help") return  Command::HELP;
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
            case Command::HELP:
                getHelp();
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
                        std::cerr << "ShellExecuteA failed " << (int)(INT_PTR)hRet << std::endl;
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
        if (arg1 == "add") {
            std::string alias = argv[2];
            mappings.erase(alias);
            saveMappings(dbFile,mappings);
            std::cout << "success" <<std::endl;
        }else {
            std::cout << "Command not found" << std::endl;
        }
    }

    if (argc == 4) {
        if (arg1 == "rm") {
            std::string alias = argv[2];
            mappings[alias] = argv[3];
            saveMappings(dbFile,mappings);
            std::cout << "success" <<std::endl;
        }else {
            std::cout << "Command not found" << std::endl;
        }
    }

    return 0;
}