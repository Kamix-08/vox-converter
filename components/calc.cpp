#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <iostream>

using namespace std;

struct read_data {
    uint32_t* size;
    unordered_map<tuple<uint8_t, uint8_t, uint8_t>, unsigned char> voxels;
    char (*palette)[4];
};

struct out_data {
    unordered_map<uint16_t, tuple<uint8_t, uint8_t, uint8_t>> colors;
    vector<tuple<float, float, float>> vertices;
    vector<uint16_t> indices;
};

const char neighbour_map[6][3] = {
    {1, 0, 0}, {-1, 0, 0}, // +x, -x
    {0, 1, 0}, { 0,-1, 0}, // +y, -y
    {0, 0, 1}, { 0, 0,-1}  // +z, -z
};

out_data calculate_vertices(const read_data& read_data) {
    unordered_map<tuple<float, float, float>, tuple<uint16_t, uint16_t, uint16_t, uint8_t, uint8_t>> ver_col;

    vector<tuple<float, float, float>> vertices;
    vector<uint16_t> indices;

    const float offset[2] = {
        (read_data.size[0] - 1) / 2.0f,
        (read_data.size[2] - 1) / 2.0f
    };

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

            tuple<float, float, float> f_ver[4];

            switch (i)
            {
            case 0:
                f_ver[0] = {center[0] + .5f, center[1] - .5f, center[2] - .5f};
                f_ver[1] = {center[0] + .5f, center[1] - .5f, center[2] + .5f};
                f_ver[2] = {center[0] + .5f, center[1] + .5f, center[2] - .5f};
                f_ver[3] = {center[0] + .5f, center[1] + .5f, center[2] + .5f};
                break;
            case 1:
                f_ver[0] = {center[0] - .5f, center[1] - .5f, center[2] - .5f};
                f_ver[1] = {center[0] - .5f, center[1] - .5f, center[2] + .5f};
                f_ver[2] = {center[0] - .5f, center[1] + .5f, center[2] - .5f};
                f_ver[3] = {center[0] - .5f, center[1] + .5f, center[2] + .5f};
                break;
            case 2:
                f_ver[0] = {center[0] - .5f, center[1] + .5f, center[2] - .5f};
                f_ver[1] = {center[0] - .5f, center[1] + .5f, center[2] + .5f};
                f_ver[2] = {center[0] + .5f, center[1] + .5f, center[2] - .5f};
                f_ver[3] = {center[0] + .5f, center[1] + .5f, center[2] + .5f};
                break;
            case 3:
                f_ver[0] = {center[0] - .5f, center[1] - .5f, center[2] - .5f};
                f_ver[1] = {center[0] - .5f, center[1] - .5f, center[2] + .5f};
                f_ver[2] = {center[0] + .5f, center[1] - .5f, center[2] - .5f};
                f_ver[3] = {center[0] + .5f, center[1] - .5f, center[2] + .5f};
                break;
            case 4:
                f_ver[0] = {center[0] - .5f, center[1] - .5f, center[2] + .5f};
                f_ver[1] = {center[0] - .5f, center[1] + .5f, center[2] + .5f};
                f_ver[2] = {center[0] + .5f, center[1] - .5f, center[2] + .5f};
                f_ver[3] = {center[0] + .5f, center[1] + .5f, center[2] + .5f};
                break;
            case 5:
                f_ver[0] = {center[0] - .5f, center[1] - .5f, center[2] - .5f};
                f_ver[1] = {center[0] - .5f, center[1] + .5f, center[2] - .5f};
                f_ver[2] = {center[0] + .5f, center[1] - .5f, center[2] - .5f};
                f_ver[3] = {center[0] + .5f, center[1] + .5f, center[2] - .5f};
                break;
            }

            uint16_t idx[4];

            for(uint8_t ii=0; ii<4; ii++) {
                auto& ver = f_ver[ii];

                get<0>(ver) -= offset[0];
                get<2>(ver) -= offset[1];

                if(ver_col.find(ver) == ver_col.end()) {
                    vertices.push_back(ver);
                    idx[ii] = vertices.size() - 1;
                } else
                    idx[ii] = get<4>(ver_col[ver]);

                get<0>(ver_col[ver]) += read_data.palette[color][0]; // r
                get<1>(ver_col[ver]) += read_data.palette[color][1]; // g
                get<2>(ver_col[ver]) += read_data.palette[color][2]; // b
                get<3>(ver_col[ver])++;                              // n (avg)
                get<4>(ver_col[ver]) = idx[ii];                      // idx
            }

            indices.push_back(idx[0]); indices.push_back(idx[1]); indices.push_back(idx[2]); // triangle 1
            indices.push_back(idx[1]); indices.push_back(idx[2]); indices.push_back(idx[3]); // triangle 2
        }

    for(auto& vtx : ver_col) {
        uint8_t n = get<3>(vtx.second);

        get<0>(vtx.second) /= n;
        get<1>(vtx.second) /= n;
        get<2>(vtx.second) /= n;
    }

    unordered_map<uint16_t, tuple<uint8_t, uint8_t, uint8_t>> colors;
    for(auto& vtx : ver_col) {
        const auto& color_data = vtx.second;
        colors[get<4>(color_data)] = {get<0>(color_data), get<1>(color_data), get<2>(color_data)};
    }

    return {
        colors,
        vertices,
        indices
    };
}