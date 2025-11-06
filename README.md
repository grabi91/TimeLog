# TimeLog
## Prerequisites
Install Linux packages:

```bash
sudo apt install cmake
sudo apt install g++
sudo apt install libssl-dev
sudo apt install zlib1g-dev
```

## Compilation
Run cmake:
```bash
cmake -H. -Bbuild
```

Compile source code:
```bash
cmake --build build
```

Compiled application can be found here:
```bash
build/timelog_cli
```

## Usage
```bash
timelog_cli
[--mode=<Pack/UnPack>]
[--dir_path=<directory_path>] - mode=Pack: compress this directory, mode=UnPack: decompress to this directory
[--file=<output/input_file>]
[--file_mode<Compressed> - optional parameter for additional compression of result file, mode=Pack: second file will be produced with extension ".compressed", mode=UnPack: ]

Example:
Compression: timelog_cli --dir_path="test_directory" --file="test_output/file.data" --mode=Pack
Decompression: timelog_cli --dir_path="test_output" --file="test_output/file.data" --mode=UnPack

Additional compression:
timelog_cli --dir_path="test_directory" --file="test_output/file.data" --mode=Pack --file_mode=Compressed
timelog_cli --dir_path="test_output" --file="test_output/file.data" --mode=UnPack --file_mode=Compressed
```

## Documantation

### Sequnce diagram for Pack mode without additional compression
```plantuml
@startuml

User -> timelog_cli: Pack
activate timelog_cli
timelog_cli -> Packer: ScanAndPack
activate Packer
Packer -> Packer: CheckArguments and prepare header

loop Iterate over files in directory
  Packer -> Compressor: Get file hash
  activate Compressor
  Compressor -> Packer: Hash
  deactivate Compressor

  Packer -> Manifest: Add Hash and write file
  activate Manifest
  /note over Manifest
    If hash appear
    for the first time,
    file will be compressed
    and saved to result file
  end note
  Manifest -> Compressor: compress and save file
  activate Compressor
  Compressor -> Manifest: file compressed
  deactivate Compressor
  Manifest -> Packer
  deactivate Manifest
end

Packer -> Packer: update header and save manifest

Packer -> Manifest: serialize
activate Manifest
Manifest -> Packer: serialized data
deactivate Manifest

Packer -> Compressor: compress data
activate Compressor
Compressor -> Packer: compressed data
deactivate Compressor

Packer -> Packer: write manifest
Packer -> Packer: update header

Packer -> timelog_cli
deactivate Packer

timelog_cli -> User: Done
deactivate timelog_cli

@enduml
```

### Sequnce diagram for UnPack mode without additional compression
```plantuml
@startuml

User -> timelog_cli: UnPack
activate timelog_cli
timelog_cli -> Unpacker: restore files

activate Unpacker
Unpacker -> Unpacker: CheckArguments and read header
Unpacker -> Unpacker: read compressed manifest

Unpacker -> Compressor: decompress data
activate Compressor
Compressor -> Unpacker: decopressed data
deactivate Compressor

Unpacker -> Manifest: deserialize manifest
activate Manifest
Manifest -> Unpacker
deactivate Manifest

Unpacker -> Manifest: recreate files
activate Manifest

loop Iterate over file hashes in the manifest
  Manifest -> Manifest: Prepared filepaths and hash
  loop Iterate over filepath in filepaths
    Manifest -> Compressor: decompress file with hash
    activate Compressor
    Compressor -> Manifest
    deactivate Compressor
  end
end

Manifest -> Unpacker
deactivate Manifest

Unpacker -> timelog_cli
deactivate Unpacker

timelog_cli -> User: Done
deactivate timelog_cli

@enduml
```

### Class diagram
```plantuml
@startuml

class Packer {
  +scanAndPack(): void
  -writeHeaderAndManifest(): void
  -writeManifestData(data): ofstream::pos_type
  -clearFileAndSaveDefaultHeader(): void
  -updateHeader(): void
  -prepareFilePath(filePath): fs::path
  -m_options: ParsedArgs
  -m_manifest: Manifest
  -m_header: Header
}

class Unpacker {
  +restoreFiles(): void
  -readHeader(): void
  -readFromFile(position, size): string
  -decompressFile(): void
  -m_options: ParsedArgs
  -m_manifest: Manifest
  -m_header: Header
}

struct Chunk {
  +toString(): string
  +static fromString(str):Chunk
  +filesPaths: vector<string>
  +position: ofstream::pos_type
  +fileSize: std::streamsize
}

class Manifest {
  +serialize(): string
  +deserialize(data): void
  +addAndWriteFile(hash, filePath): void
  +recreateFiles(dirPath): void
  -m_manifest: Map<string, Chunk>
  -m_filePath: fs::path
}

struct FileInfo {
  +m_position
  +m_fileSize
}

class Compressor {
  +static compressData(data): string
  +static decompressData(compressedData, originalSize): string
  +static compressFile(inputPath, outputPath): FileInfo
  +static decompressFile(inputPath, outputPath, fileInfo): void
  +static getFileSize(filePath): streamsize
  +static sha256HashFile(filePath): string
}

class ParsedArgs {
  +parse(argc, argv): bool
  +printHelp(): void
  +m_mode: Mode
  +m_fileMode: FileMode
  +m_dirPath: fs::path
  +m_filePath: fs::path
}

class Loading {
  +start(): void
  +stop(): void
  -m_running: atomic<bool>
  -m_loadingThread: thread
}

Compressor *-- FileInfo : contains
Manifest *-- FileInfo : contains
Packer *-- FileInfo : contains

Packer o-- Compressor : uses
Unpacker o-- Compressor : uses
Manifest o-- Compressor : uses

Packer *-- ParsedArgs : contains
Unpacker *-- ParsedArgs : contains

Compressor o-- Loading : uses

Manifest *-- Chunk : contains

Packer *-- Manifest : contains
Unpacker *-- Manifest : contains

@enduml
```