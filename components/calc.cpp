#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <iostream>

using namespace std;

struct read_data {
    uint32_t size[3];
    std::unordered_map<std::tuple<uint8_t, uint8_t, uint8_t>, unsigned char> voxels;
    char palette[256][4];
};

const char neighbour_map[6][3] = {
    {1, 0, 0}, {-1, 0, 0}, // +x, -x
    {0, 1, 0}, { 0,-1, 0}, // +y, -y
    {0, 0, 1}, { 0, 0,-1}  // +z, -z
};

void calculate_vertices(const read_data& read_data) {
    unordered_map<tuple<float, float, float>, tuple<char, char, char, float>> vtxs;

    for(const auto& [pos, color] : read_data.voxels)
        for(uint8_t i=0; i<6; i++) {
            uint8_t center[3] = {
                get<0>(pos),
                get<1>(pos),
                get<2>(pos)
            };

            tuple<uint8_t, uint8_t, uint8_t> neighbour_pos = {
                center[0] + neighbour_map[i][0],
                center[1] + neighbour_map[i][1],
                center[2] + neighbour_map[i][2]
            };

            if(read_data.voxels.find(neighbour_pos) != read_data.voxels.end()) 
                continue;

            tuple<float, float, float> vtx[4];

            switch (i)
            {
            case 0:
                vtx[0] = {center[0] + .5f, center[1] + .5f, center[2] + .5f};
                vtx[1] = {center[0] + .5f, center[1] - .5f, center[2] + .5f};
                vtx[2] = {center[0] + .5f, center[1] + .5f, center[2] - .5f};
                vtx[3] = {center[0] + .5f, center[1] - .5f, center[2] - .5f};
                break;
            case 1:
                vtx[0] = {center[0] - .5f, center[1] + .5f, center[2] + .5f};
                vtx[1] = {center[0] - .5f, center[1] - .5f, center[2] + .5f};
                vtx[2] = {center[0] - .5f, center[1] + .5f, center[2] - .5f};
                vtx[3] = {center[0] - .5f, center[1] - .5f, center[2] - .5f};
                break;
            case 2:
                vtx[0] = {center[0] + .5f, center[1] + .5f, center[2] + .5f};
                vtx[1] = {center[0] - .5f, center[1] + .5f, center[2] + .5f};
                vtx[2] = {center[0] + .5f, center[1] + .5f, center[2] - .5f};
                vtx[3] = {center[0] - .5f, center[1] + .5f, center[2] - .5f};
                break;
            case 3:
                vtx[0] = {center[0] + .5f, center[1] - .5f, center[2] + .5f};
                vtx[1] = {center[0] - .5f, center[1] - .5f, center[2] + .5f};
                vtx[2] = {center[0] + .5f, center[1] - .5f, center[2] - .5f};
                vtx[3] = {center[0] - .5f, center[1] - .5f, center[2] - .5f};
                break;
            case 4:
                vtx[0] = {center[0] + .5f, center[1] + .5f, center[2] + .5f};
                vtx[1] = {center[0] - .5f, center[1] + .5f, center[2] + .5f};
                vtx[2] = {center[0] + .5f, center[1] - .5f, center[2] + .5f};
                vtx[3] = {center[0] - .5f, center[1] - .5f, center[2] + .5f};
                break;
            case 5:
                vtx[0] = {center[0] + .5f, center[1] + .5f, center[2] - .5f};
                vtx[1] = {center[0] - .5f, center[1] + .5f, center[2] - .5f};
                vtx[2] = {center[0] + .5f, center[1] - .5f, center[2] - .5f};
                vtx[3] = {center[0] - .5f, center[1] - .5f, center[2] - .5f};
                break;
            }

            for(const auto& v : vtx) {
                get<0>(vtxs[v]) += read_data.palette[color][0];
                get<1>(vtxs[v]) += read_data.palette[color][1];
                get<2>(vtxs[v]) += read_data.palette[color][2];
                get<3>(vtxs[v]) *= (static_cast<float>(read_data.palette[color][3])/255 + 2);
            }
        }

    for(auto& vtx : vtxs) {
        uint8_t a = get<3>(vtx.second);
        uint8_t n = 0;

        while(a != 1) {
            a >>= 1;
            n++;
        }

        get<0>(vtx.second) /= n;
        get<1>(vtx.second) /= n;
        get<2>(vtx.second) /= n;
        get<3>(vtx.second) -= a;
    }
}