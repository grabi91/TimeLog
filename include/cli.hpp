#pragma once
#include <string>
#include <atomic>
#include <thread>

namespace Cli {

enum class Mode { Unknown, Pack, UnPack};

enum class FileMode {Normall, Compressed};

class ParsedArgs {
    public:
        ParsedArgs();
        
        void parse(int argc, char* argv[]);

        Mode m_mode;
        FileMode m_fileMode;
        std::string m_dirPath;
        std::string m_filePath;

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
