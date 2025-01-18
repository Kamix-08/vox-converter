#include <fstream>

#include "calculation.cpp"

using namespace std;

int read_file(const string& filename) {
    ifstream file(filename, ios::binary);

    if(!file) {
        cerr << "[!] Failed to open file. (" << filename << ")\n";
        return 1;
    }

    char buff[4];

    file.read(buff, 4);
    if(string(buff, 4) != "VOX ") {
        cerr << "[!] Invalid file format: " << string(buff, 4) << ". (" << filename << ")\n";
        return 1;
    }

    file.seekg(4, ios::cur); // version

    file.read(buff, 4);
    if(string(buff, 4) != "MAIN") {
        cerr << "[!] Missing MAIN chunk. (" << filename << ")\n";
        return 1;
    }

    file.seekg(2*4, ios::cur); // chunk content size

    file.read(buff, 4);
    if(string(buff, 4) != "SIZE") {
        cerr << "[!] Missing SIZE chunk. (" << filename << ")\n";
        return 1;
    }

    file.seekg(2*4, ios::cur);

    uint32_t size[3];
    file.read((char*)size, 3*4);

    file.read(buff, 4);
    if(string(buff, 4) != "XYZI") {
        cerr << "[!] Missing XYZI chunk. (" << filename << ")\n";
        return 1;
    }

    file.seekg(2*4, ios::cur);

    uint32_t num_voxels;
    file.read((char*)&num_voxels, 4);

    unordered_map<tuple<uint8_t, uint8_t, uint8_t>, unsigned char> voxels;
    for (uint32_t i = 0; i < num_voxels; i++) {
        file.read((char*)buff, 4);
        voxels[{buff[0], buff[1], buff[2]}] = buff[3];
    }

    file.read(buff, 4);
    if(string(buff, 4) != "RGBA") {
        cerr << "[!] Missing RGBA chunk. (" << filename << ")\n";
        return 1;
    }

    file.seekg(2*4, ios::cur);

    char palette[256][4];
    file.read((char*)palette, 256*4);

    if(file.peek() != EOF) {
        cerr << "[!] Unexpected data after RGBA chunk. (" << filename << ")\n";
        return 1;
    }

    file.close();
}