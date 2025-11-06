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

        void clearHeader();
        void writeHeader();

        fs::path prepareFilePath(const fs::path& filePath);

        const Cli::ParsedArgs m_options;

        Manifest m_manifest;
        Types::Header m_header;
};