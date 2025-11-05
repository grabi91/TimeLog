#include <sstream>
#include <string>
#include <iostream>

#include "compressor.hpp"
#include "manifest.hpp"

Manifest::Manifest(std::string filePath):
    m_filePath(filePath)
{
}

std::string Manifest::serialize()
{
    std::ostringstream oss;
    for (const auto& [key, value] : m_manifest) {
        oss << key << ":" << value.toString() << "\n";
    }
    return oss.str();
}

void Manifest::deserialize(const std::string &data)
{
    std::istringstream iss(data);
    std::string line;

    while (std::getline(iss, line)) {
        size_t colonPos = line.find(':');
        if (colonPos == std::string::npos) continue;

        std::string key = line.substr(0, colonPos);
        std::string values = line.substr(colonPos + 1);

        m_manifest[key] = Chunk::fromString(values);
    }
}

void Manifest::addAndWriteFile(std::string hash, const fs::path &filePath)
{
    if (m_manifest.find(hash) != m_manifest.end()) {
        m_manifest[hash].filesPaths.push_back(filePath);
    } else {
        Chunk chunk;
        std::cout << "Compression of unique file with hash: " << hash  << std::endl;
        Compressor::FileInfo fileInfo = Compressor::compressFile(filePath, m_filePath);
        chunk.position = fileInfo.m_position;
        chunk.fileSize = fileInfo.m_fileSize;
        chunk.filesPaths.push_back(filePath);
        m_manifest[hash] = chunk;
    }
}

void Manifest::recreateFiles(std::string dirPath)
{
    for (const auto& [hash, chunk] : m_manifest) {
        Compressor::FileInfo fileInfo;
        fileInfo.m_fileSize = chunk.fileSize;
        fileInfo.m_position = chunk.position;

        std::cout << "Recreation file with hash: " << hash << std::endl;

        for (const auto& filepath : chunk.filesPaths) {
            fs::path outputPath = fs::path(dirPath) / fs::path(filepath);
            fs::create_directories(outputPath.parent_path());

            Compressor::decompressFile(m_filePath, outputPath, fileInfo);
        }
    }
}

std::string Manifest::Chunk::toString() const
{
    std::ostringstream oss;
    oss << position  << "," << fileSize;
    for (size_t i = 0; i < filesPaths.size(); ++i) {
        oss << "," << filesPaths[i];
    }
    return oss.str();
}

Manifest::Chunk Manifest::Chunk::fromString(const std::string& str) {
    std::istringstream iss(str);
    Chunk chunk;

    std::string token;

    // Read position
    if (std::getline(iss, token, ',')) {
        chunk.position = static_cast<std::ofstream::pos_type>(std::stoll(token));
    }

    // Read fileSize
    if (std::getline(iss, token, ',')) {
        chunk.fileSize = static_cast<std::streamsize>(std::stoll(token));
    }

    // Read remaining file paths
    while (std::getline(iss, token, ',')) {
        chunk.filesPaths.push_back(token);
    }

    return chunk;
}

