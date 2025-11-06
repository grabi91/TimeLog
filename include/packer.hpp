#pragma once
#include <string>
#include <filesystem>

#include "cli.hpp"
#include "manifest.hpp"
#include "types.hpp"

namespace fs = std::filesystem;

class Packer {
    public:
        Packer(Cli::ParsedArgs options);
        void scanAndPack();
    
    private:
        void writeHeaderAndManifest();

        std::ofstream::pos_type writeManifestData(const std::string &data);

        // Clear file and saved default header values
        void clearFileAndSaveDefaultHeader();

        void updateHeader();

        // Return file path with removed parent path of m_options.m_dirPath
        // Example:
        // m_options.m_dirPath = "/home/user/test_directory"
        // filePath = "/home/user/test_directory/subdirt/test.txt"
        // retval = "test_directory/subdirt/test.txt"
        fs::path prepareFilePath(const fs::path& filePath);

        const Cli::ParsedArgs m_options;

        Manifest m_manifest;
        Types::Header m_header;
};