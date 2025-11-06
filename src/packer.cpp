#include <filesystem>
#include <iostream>
#include <fstream>

#include "packer.hpp"
#include "compressor.hpp"

namespace fs = std::filesystem;

Packer::Packer(Cli::ParsedArgs options):
    m_manifest(options.m_filePath),
    m_options(options)
{
    m_header.magic = Types::MAGIC_NUMBER;
    m_header.version = 0x01U;
    m_header.manifestOffset = sizeof(m_header);
    m_header.manifestSize = 0x0U;
    m_header.manifestCompressedSize = 0x0U;
}

void Packer::scanAndPack()
{
    fs::path parentPath = m_options.m_filePath.parent_path();
    if (!parentPath.empty() && !fs::exists(parentPath))
    {
        throw std::runtime_error("Packer::scanAndPack: file parent path doesn't exist: " + m_options.m_filePath.parent_path().string());
    }

    if (!fs::exists(m_options.m_dirPath))
    {
        throw std::runtime_error("Packer::scanAndPack: path doesn't exist: " + m_options.m_dirPath.string());
    }

    clearHeader();

    fs::path canonicalPath = fs::canonical(m_options.m_dirPath);

    std::cout << canonicalPath << std::endl;

    for (const auto& entry : fs::recursive_directory_iterator(canonicalPath)) {
        if (fs::is_regular_file(entry)) {
            std::string hash = Compressor::sha256HashFile(entry.path());

            m_manifest.addAndWriteFile(hash, prepareFilePath(entry.path()));
        }
    }

    writeHeaderAndManifest();

    if (m_options.m_fileMode == Cli::FileMode::Compressed)
    {
        fs::path compressedFilePath = m_options.m_filePath.string() + ".compressed";
        std::cout << "Additional compression started" << std::endl;
        Compressor::compressFile(m_options.m_filePath, compressedFilePath);
        std::cout << "Compressed file created: " << compressedFilePath << std::endl;
    }
}

void Packer::writeHeaderAndManifest()
{
    std::string serialized = m_manifest.serialize();
    std::string compressed = Compressor::compressData(serialized);

    m_header.manifestSize = serialized.size();
    m_header.manifestCompressedSize = compressed.size();

    m_header.manifestOffset = writeManifestData(compressed);

    writeHeader();
}

std::ofstream::pos_type Packer::writeManifestData(const std::string &data)
{
    std::ofstream outFile(m_options.m_filePath, std::ios::binary | std::ios::app);
    
    std::ofstream::pos_type position = outFile.tellp();

    outFile.write(data.c_str(), data.size());
    outFile.close();

    return position;
}

void Packer::clearHeader()
{
    std::ofstream outFile(m_options.m_filePath, std::ios::binary);
    outFile.write(reinterpret_cast<char*>(&m_header), sizeof(m_header));
    outFile.close();
}

void Packer::writeHeader()
{
    std::ofstream outFile(m_options.m_filePath, std::ios::binary | std::ios::in | std::ios::out);
    outFile.write(reinterpret_cast<char*>(&m_header), sizeof(m_header));
    outFile.close();
}

fs::path Packer::prepareFilePath(const fs::path &filePath)
{
    fs::path parentPath = m_options.m_dirPath.parent_path();
    if (!parentPath.empty() && fs::exists(parentPath)) {
        return fs::relative(filePath, parentPath);
    } else {
        return filePath;
    }
}
