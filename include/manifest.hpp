#pragma once
#include <unordered_map>
#include <vector>
#include <fstream>
#include <filesystem>

namespace fs = std::filesystem;

class Manifest {
    public:
        Manifest(std::string filePath);

        struct Chunk {
            std::vector<std::string> filesPaths;
            std::ofstream::pos_type position;
            std::streamsize fileSize;

            std::string toString() const;
            static Chunk fromString(const std::string& str);
        };
        
        std::string serialize();
        
        void deserialize(const std::string& data);

        void addAndWriteFile(std::string hash, const fs::path &filePath);

        void recreateFiles(std::string dirPath);
        
    private:
        std::unordered_map<std::string, Chunk> m_manifest;
        std::string m_filePath;
};