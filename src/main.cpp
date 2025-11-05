#include <iostream>

#include "cli.hpp"
#include "packer.hpp"
#include "unpacker.hpp"

int main(int argc, char* argv[]) {
    try {
        Cli::ParsedArgs arguments;
        arguments.parse(argc, argv);

        switch (arguments.m_mode) {
            case Cli::Mode::Pack:
                Packer(arguments).scanAndPack();
                break;
            case Cli::Mode::UnPack:
                Unpacker(arguments).restoreFiles();
                break;
            default:
                std::cerr << "Unknown mode.\n";
                break;
        }
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
    }

    return 0;
}
