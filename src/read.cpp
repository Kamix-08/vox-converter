#include "../include/read.hpp"
#include "../include/calc.hpp"

#include <fstream>
#include <chrono>
#include <iostream>
#include <unordered_map>
#include <cstdint>
#include <tuple>
#include <cstring>

int vox::load_model(const std::string& filename, vox::model_data& model_data) {
    auto start = std::chrono::system_clock::now();

    std::ifstream file(filename, std::ios::binary);

    if(!file) {
        std::cerr << "[!] Failed to open file. (" << filename << ")\n";
        return 1;
    }

    char buff[4];

    file.read(buff, 4);
    if(std::string(buff, 4) != "VOX ") {
        std::cerr << "[!] Invalid file format: " << std::string(buff, 4) << ". (" << filename << ")\n";
        return 1;
    }

    file.seekg(4, std::ios::cur); // version

    file.read(buff, 4);
    if(std::string(buff, 4) != "MAIN") {
        std::cerr << "[!] Missing MAIN chunk. (" << filename << ")\n";
        return 1;
    }

    file.seekg(2*4, std::ios::cur); // chunk content size

    file.read(buff, 4);
    if(std::string(buff, 4) != "SIZE") {
        std::cerr << "[!] Missing SIZE chunk. (" << filename << ")\n";
        return 1;
    }

    file.seekg(2*4, std::ios::cur);

    uint32_t size[3];
    file.read((char*)size, 3*4);

    file.read(buff, 4);
    if(std::string(buff, 4) != "XYZI") {
        std::cerr << "[!] Missing XYZI chunk. (" << filename << ")\n";
        return 1;
    }

    file.seekg(2*4, std::ios::cur);

    uint32_t num_voxels;
    file.read((char*)&num_voxels, 4);

    std::unordered_map<std::tuple<uint8_t, uint8_t, uint8_t>, unsigned char, vox::tuple_hash> voxels;
    for (uint32_t i = 0; i < num_voxels; i++) {
        file.read((char*)buff, 4);
        voxels[{buff[0], buff[1], buff[2]}] = buff[3];
    }

    unsigned char palette[256][4];

    file.read(buff, 4);
    if(std::string(buff, 4) != "RGBA") {
        std::memcpy(palette, default_palette, 256*4);
    } else {
        file.seekg(2 * 4, std::ios::cur);
        file.read((char*)palette, 256 * 4);
    }

    file.close();

    model_data = vox::calculate_vertices({
        size,
        voxels,
        palette
    });

    auto end = std::chrono::system_clock::now();

    std::cout << "[.] Loaded model: " << filename << "\n";
    std::cout << "[.] Time taken: " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << "ms\n\n";

    return 0;
}