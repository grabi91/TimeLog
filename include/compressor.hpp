#pragma once
#include <string>
#include <fstream>
#include <filesystem>

namespace fs = std::filesystem;

class Compressor {
    public:
        struct FileInfo {
            std::ofstream::pos_type m_position;
            std::streamsize m_fileSize;
        };

        static std::string compressData(const std::string& data);
        static std::string decompressData(const std::string& compressedData, std::uint64_t originalSize);
        static FileInfo compressFile(const fs::path& inputPath, const fs::path& outputPath);

        static void decompressFile(const fs::path& inputPath, const fs::path& outputPath, const FileInfo& fileInfo);

        static std::streamsize getFileSize(const fs::path& filePath);

        static std::string sha256HashFile(const fs::path& filePath);

        static constexpr size_t CHUNK_BUFFER_SIZE = 16384U;
        static constexpr size_t SHA256_BUFFER_SIZE = 8192U;

};