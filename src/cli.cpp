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

ParsedArgs::ParsedArgs():
    m_mode(Mode::Unknown),
    m_fileMode(FileMode::Normall)
{
}

bool ParsedArgs::parse(int argc, char *argv[])
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
                        return false;
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
                        return false;
                    }
                } else {
                    std::cout << "Unknown command.\n";
                    return false;
                }
            } else {
                return false;
            }
        }
    }

    if (m_dirPath.empty()) {
        std::cerr << "dir_path empty.\n";
        return false;
    } else if (m_filePath.empty()) {
        std::cerr << "file empty.\n";
        return false;
    }

    return true;
}

void ParsedArgs::printHelp()
{
    std::cout << "Usage: timelog_cli" << std::endl
              << "[--mode=<Pack/UnPack>]" << std::endl
              << "[--dir_path=<directory_path>] - mode=Pack: compress this directory, mode=UnPack: decompress to this directory" << std::endl
              << "[--file=<output/input_file>]" << std::endl
              << "[--file_mode=Compressed - optional parameter for additional compression of result file, mode=Pack: second file will be produced with extension \".compressed\", mode=UnPack: first try to use file with extension \".compressed\", if found, decompress it and overwrite input_file]" << std::endl
              << std::endl
              << "Example:" << std::endl
              << "Compression: timelog_cli --dir_path=\"test_directory\" --file=\"test_output/file.data\" --mode=Pack" << std::endl
              << "Decompression: timelog_cli --dir_path=\"test_output\" --file=\"test_output/file.data\" --mode=UnPack" << std::endl
              << std::endl
              << "Additional compression:" << std::endl
              << "timelog_cli --dir_path=\"test_directory\" --file=\"test_output/file.data\" --mode=Pack --file_mode=Compressed" << std::endl
              << "timelog_cli --dir_path=\"test_output\" --file=\"test_output/file.data\" --mode=UnPack --file_mode=Compressed" << std::endl;
}

}
