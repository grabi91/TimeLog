#include <zlib.h>
#include <fstream>
#include <openssl/sha.h>

#include "cli.hpp"
#include "compressor.hpp"

std::string Compressor::compressData(const std::string &data)
{
    uLong srcLen = data.size();
    uLong destLen = compressBound(srcLen);
    std::string compressed(destLen, '\0');

    if (compress(reinterpret_cast<Bytef*>(&compressed[0]), &destLen,
                 reinterpret_cast<const Bytef*>(data.c_str()), srcLen) != Z_OK) {
        throw std::runtime_error("Compressor::compressData: Compression failed");
    }

    compressed.resize(destLen);
    return compressed;
}

std::string Compressor::decompressData(const std::string &compressedData, std::uint64_t originalSize)
{
    std::string decompressed(originalSize, '\0');

    if (uncompress(reinterpret_cast<Bytef*>(&decompressed[0]), &originalSize,
                   reinterpret_cast<const Bytef*>(compressedData.c_str()), compressedData.size()) != Z_OK) {
        throw std::runtime_error("Compressor::decompressData: Decompression failed");
    }

    decompressed.resize(originalSize);
    return decompressed;
}

Compressor::FileInfo Compressor::compressFile(const fs::path& inputPath, const fs::path& outputPath)
{
    char inBuffer[CHUNK_BUFFER_SIZE];
    char outBuffer[CHUNK_BUFFER_SIZE];

    std::ifstream inFile(inputPath, std::ios::binary);
    std::ofstream outFile(outputPath, std::ios::binary | std::ios::app | std::ios::ate);

    if (!inFile || !outFile) {
        throw std::runtime_error("Compressor::compressFile: Failed to open files");
    }

    FileInfo fileInfo;

    fileInfo.m_position = outFile.tellp();

    Cli::Loading loading("Proccessing");

    z_stream strm = {};
    deflateInit(&strm, Z_BEST_COMPRESSION);

    int flush;
    do {
        inFile.read(inBuffer, CHUNK_BUFFER_SIZE);
        strm.avail_in = inFile.gcount();
        flush = inFile.eof() ? Z_FINISH : Z_NO_FLUSH;
        strm.next_in = reinterpret_cast<Bytef*>(inBuffer);

        do {
            strm.avail_out = CHUNK_BUFFER_SIZE;
            strm.next_out = reinterpret_cast<Bytef*>(outBuffer);
            deflate(&strm, flush);
            size_t have = CHUNK_BUFFER_SIZE - strm.avail_out;
            outFile.write(outBuffer, have);
        } while (strm.avail_out == 0);
    } while (flush != Z_FINISH);

    deflateEnd(&strm);

    fileInfo.m_fileSize = outFile.tellp() - fileInfo.m_position;

    outFile.close();

    return fileInfo;
}

void Compressor::decompressFile(const fs::path& inputPath, const fs::path& outputPath, const FileInfo& fileInfo)
{
    char inBuffer[CHUNK_BUFFER_SIZE];
    char outBuffer[CHUNK_BUFFER_SIZE];

    std::ifstream inFile(inputPath, std::ios::binary);
    std::ofstream outFile(outputPath, std::ios::binary);

    if (!inFile || !outFile) {
        throw std::runtime_error("Compressor::decompressFile: Failed to open files");
    }

    // Set position and limit read size
    inFile.seekg(fileInfo.m_position);
    std::streamsize remaining = fileInfo.m_fileSize;

    z_stream strm = {};
    inflateInit(&strm);

    int ret;
    do {
        std::streamsize toRead = std::min(static_cast<std::streamsize>(CHUNK_BUFFER_SIZE), remaining);
        inFile.read(inBuffer, toRead);
        std::streamsize bytesRead = inFile.gcount();
        remaining -= bytesRead;

        if (bytesRead == 0) break;

        strm.avail_in = static_cast<uInt>(bytesRead);
        strm.next_in = reinterpret_cast<Bytef*>(inBuffer);

        do {
            strm.avail_out = CHUNK_BUFFER_SIZE;
            strm.next_out = reinterpret_cast<Bytef*>(outBuffer);
            ret = inflate(&strm, Z_NO_FLUSH);

            if (ret == Z_STREAM_ERROR || ret == Z_DATA_ERROR || ret == Z_MEM_ERROR) {
                inflateEnd(&strm);
                throw std::runtime_error("Compressor::decompressFile: Decompression error");
            }

            size_t have = CHUNK_BUFFER_SIZE - strm.avail_out;
            outFile.write(outBuffer, have);
        } while (strm.avail_out == 0);

    } while (ret != Z_STREAM_END && remaining > 0);

    inflateEnd(&strm);
}

std::streamsize Compressor::getFileSize(const fs::path& filePath)
{
    std::ifstream file(filePath, std::ios::binary | std::ios::ate);
    if (!file) {
        throw std::runtime_error("Compressor::getFileSize: Failed to open file: " + filePath.string());
    }

    return file.tellg(); // Position at end = file size
}

std::string Compressor::sha256HashFile(const fs::path &filePath)
{
    std::ifstream file(filePath, std::ios::binary);
    if (!file) {
        throw std::runtime_error("Compressor::sha256HashFile: Cannot open file: " + filePath.string());
    }

    SHA256_CTX ctx;
    SHA256_Init(&ctx);

    char buffer[SHA256_BUFFER_SIZE];
    while (file.read(buffer, sizeof(buffer)) || file.gcount()) {
        SHA256_Update(&ctx, buffer, file.gcount());
    }

    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256_Final(hash, &ctx);

    std::ostringstream result;
    for (unsigned char c : hash) {
        result << std::hex << std::setw(2) << std::setfill('0') << (int)c;
    }

    return result.str();
}
