#include <iostream>
#include <string>
#include <fstream>
#include <unordered_map>
#include <windows.h>
#include <shellapi.h>
#include <filesystem>

#pragma comment(lib, "Shell32.lib")

#define FA_VERSION "0.0.4"

std::string getExeDir() {
    char path[MAX_PATH];
    GetModuleFileNameA(nullptr,path,MAX_PATH);
    std::string spath(path);
    return spath.substr(0,spath.find_last_of('\\'));
}

void loadMappings(const std::string& filename, std::unordered_map<std::string, std::string>& map) {
    std::ifstream file(filename);

    if (!file.is_open()) {
        std::ofstream createFile(filename);
        if (!createFile.is_open()) {
            std::cerr << "ERROR : Cannot create " << filename << std::endl;
        }
        return;
    }

    std::string line;
    while (std::getline(file, line)) {
        size_t pos = line.find('=');
        if (pos != std::string::npos && pos > 0) { // 简单防止空别名
            map[line.substr(0, pos)] = line.substr(pos + 1);
        }
    }
}


void saveMappings(const std::string& filename, const std::unordered_map<std::string,std::string>& map) {
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
              << "    version                        Show version information\n"
              << "    ls                             List all alias mappings\n"
              << "    help                           Show this help message\n"
              << "    release                        open the github release page\n"
              << "    <alias>                        Open folder by alias name\n"
              << "\n"
              << "ARGUMENTS:\n"
              << "    fa del <alias>                 Remove an alias mapping\n"
              << "    fa add <alias> <path>          Add or update an alias mapping\n"
              << "\n"
              << "EXAMPLES:\n"
              << "    fa version                     Display current version\n"
              << "    fa ls                          Show all configured aliases\n"
              << "    fa example1                    Open folder mapped to 'example1'\n"
              << "    fa del example1                Remove the 'example1' alias\n"
              << "    fa add example1 D:\\example1   Create alias 'example1' for D:\\example1\n"
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
        std::cout << pair.first << " -> " << pair.second << "\n";
    }
    std::cout << "----------------------------------------" << std::endl;
    std::cout << "Total: " << map.size() << " mapping(s)" << std::endl;
}

void openFolder(std::unordered_map<std::string , std::string>& mappings, const std::string& arg1) {
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
            std::cerr << "ERROR : Failed to open folder " << (int)(INT_PTR)hRet << std::endl;
        }else {
            std::cout << "Success." << std::endl;
        }
    }else {
        std::cout << "Warning : Cannot find alias." << std::endl;
    }
}

void openReleasePage() {
    const char* url = "https://github.com/Syrnaxei/Folder-Alias/releases";
    HINSTANCE hRet = ShellExecuteA(
        nullptr,
        "open",
        url,
        nullptr,
        nullptr,
        SW_NORMAL
        );
    if ((INT_PTR)hRet <= 32) {
        std::cerr << "ERROR : Failed to open browser " << (int)(INT_PTR)hRet << std::endl;
    }else {
        std::cout << "Success." << std::endl;
        std::cout << "You need to manually download the release." << std::endl;
    }
}

bool containsEqual(const std::string& str) {
    return str.find('=') != std::string::npos;
}

enum class Command {
    VERSION,
    LS,
    HELP,
    RELEASE,
    OPEN
};

Command parseCommand(const std::string& cmd) {
    if (cmd == "version") return Command::VERSION;
    if (cmd == "ls") return Command::LS;
    if (cmd == "help") return  Command::HELP;
    if (cmd == "release") return Command::RELEASE;
    return Command::OPEN;
}

int main(int argc,char* argv[]) {
    if (argc < 2) {
        getHelp();
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
            case Command::RELEASE:
                openReleasePage();
                break;
            case Command::OPEN:
                openFolder(mappings ,arg1);
                break;
            default:
                std::cout << "Warning : Command not found." << std::endl;
                return 1;
        }
    }

    if (argc == 3) {
        if (arg1 == "del") {
            std::string alias = argv[2];
            std::size_t eraseState = mappings.erase(alias);
            if (eraseState == 1){
                saveMappings(dbFile,mappings);
                std::cout << "Success." <<std::endl;
            }else {
                std::cout << "Warning : Alias not found." << std::endl;
                return 1;
            }
        }else {
            std::cout << "Warning : Command not found." << std::endl;
            return 1;
        }
    }

    if (argc == 4) {
        if (arg1 == "add") {
            if (containsEqual(argv[2])) {
                std::cout << "Warning : Alias cannot contain '='." << std::endl;
                return 1;
            }
            if (containsEqual(argv[3])) {
                std::cout << "Warning : Path cannot contain '='." << std::endl;
                return 1;
            }
            std::string alias = argv[2];
            mappings[alias] = argv[3];
            saveMappings(dbFile,mappings);
            std::cout << "Success." <<std::endl;
        }else {
            std::cout << "Warning : Command not found." << std::endl;
            return 1;
        }
    }

    if (argc > 4) {
        getHelp();
        return 1;
    }

    return 0;
}