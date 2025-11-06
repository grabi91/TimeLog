#include <fstream>
#include <iostream>

#include "compressor.hpp"
#include "unpacker.hpp"

Unpacker::Unpacker(Cli::ParsedArgs options):
    m_manifest(options.m_filePath),
    m_options(options)
{
}

void Unpacker::restoreFiles()
{
    if (m_options.m_fileMode == Cli::FileMode::Compressed)
    {
        decompressFile();
    }

    readHeader();

    std::string manifestCompressed = readFromFile(m_header.manifestOffset, m_header.manifestCompressedSize);

    std::string manifestDecompressed = Compressor::decompressData(manifestCompressed, m_header.manifestSize);

    m_manifest.deserialize(manifestDecompressed);
    
    m_manifest.recreateFiles(m_options.m_dirPath);
}

void Unpacker::readHeader()
{
    std::ifstream inFile(m_options.m_filePath, std::ios::binary);
    if (!inFile) {
        throw std::runtime_error("Unpacker::readHeader: Failed to open file");
    }

    inFile.read(reinterpret_cast<char*>(&m_header), sizeof(m_header));
    inFile.close();

    if (m_header.magic != Types::MAGIC_NUMBER) {
        throw std::runtime_error("Unpacker::readHeader: Incorrect Magic File");
    }
}

std::string Unpacker::readFromFile(std::streampos position, std::size_t size)
{
    std::ifstream file(m_options.m_filePath, std::ios::binary);
    if (!file) {
        throw std::runtime_error("Unpacker::readFromFile: Failed to open file");
    }

    file.seekg(position);
    if (!file) {
        throw std::runtime_error("Unpacker::readFromFile: Failed to seek to position");
    }

    std::string buffer(size, '\0');
    file.read(&buffer[0], size);

    if (!file) {
        throw std::runtime_error("Unpacker::readFromFile: Failed to read data");
    }

    return buffer;
}

void Unpacker::decompressFile()
{
    std::string compressedPath =  m_options.m_filePath.string() + ".compressed";
    Compressor::FileInfo fileInfo;
    fileInfo.m_fileSize = Compressor::getFileSize(compressedPath);

    std::cout << "Decompression file: " << compressedPath << std::endl;
    
    Compressor::decompressFile(compressedPath, m_options.m_filePath, fileInfo);
}
