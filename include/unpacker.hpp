#pragma once
#include <string>

#include "cli.hpp"
#include "manifest.hpp"
#include "types.hpp"

class Unpacker {
    public:
        Unpacker(Cli::ParsedArgs options);

        void restoreFiles();

    private:
        void readHeader();

        std::string readFromFile(std::streampos position, std::size_t size);

        // Decompress additional "m_options.m_filePath".compressed and save it to "m_options.m_filePath"
        // If m_options.m_filePath file exist, it will be overwritted
        void decompressFile();

        const Cli::ParsedArgs m_options;

        Manifest m_manifest;
        Types::Header m_header;
};
