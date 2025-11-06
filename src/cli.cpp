#include <iostream>
#include <chrono>

#include "cli.hpp"

namespace Cli {

Loading::Loading(std::string message):
    m_running(false)
{
    std::cout << message << std::flush;
    start();
}

Loading::~Loading()
{
    stop();
}

void Loading::start()
{
    if (m_running.load()) return; // Already running

    m_running.store(true);
    m_loadingThread = std::thread([&] {
        while (m_running.load()) {
            std::cout << "." << std::flush;
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        }
    });
}

void Loading::stop()
{
    if (!m_running.load()) return;

    m_running.store(false);
    if (m_loadingThread.joinable()) {
        m_loadingThread.join();
    }

    std::cout << std::endl;
}

void ParsedArgs::parse(int argc, char *argv[])
{
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];

        // Check if argument starts with "--"
        if (arg.rfind("--", 0) == 0) {
            size_t eqPos = arg.find('=');
            if (eqPos != std::string::npos) {
                std::string key = arg.substr(2, eqPos - 2);  // skip "--"
                std::string value = arg.substr(eqPos + 1);
                
                if (key == "mode") {
                    if (value == "Pack") {
                        std::cout << "Mode::Pack" << std::endl;
                        m_mode = Mode::Pack;
                    } else if (value == "UnPack") {
                        std::cout << "Mode::UnPack" << std::endl;
                        m_mode = Mode::UnPack;
                    } else {
                        std::cerr << "Unknown mode.\n";
                    }
                } else if (key == "dir_path") {
                    m_dirPath = value;
                } else if (key == "file") {
                    m_filePath = value;
                } else if (key == "file_mode") {
                    if (value == "Normall") {
                        std::cout << "FileMode::Normall" << std::endl;
                        m_fileMode = FileMode::Normall;
                    } else if (value == "Compressed") {
                        std::cout << "FileMode::Compressed" << std::endl;
                        m_fileMode = FileMode::Compressed;
                    } else {
                        std::cerr << "Unknown file mode.\n";
                    }
                } else {
                    std::cout << "Unknown command.\n";
                }
            }
        }
    }
}

ParsedArgs::ParsedArgs():
    m_mode(Mode::Unknown),
    m_fileMode(FileMode::Normall)
{
}

}