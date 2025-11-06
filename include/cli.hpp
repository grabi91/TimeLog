#pragma once
#include <string>
#include <atomic>
#include <thread>
#include <filesystem>

namespace fs = std::filesystem;

namespace Cli {

enum class Mode { Unknown, Pack, UnPack};

enum class FileMode {Normall, Compressed};

class ParsedArgs {
    public:
        ParsedArgs();
        
        bool parse(int argc, char* argv[]);

        void printHelp();

        Mode m_mode;
        FileMode m_fileMode;
        fs::path m_dirPath;
        fs::path m_filePath;

};

class Loading {
    public:
        Loading(std::string message);
        ~Loading();

        void start();
        void stop();

    private:
        std::atomic<bool> m_running;
        std::thread m_loadingThread;

};

}
