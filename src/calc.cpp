#include "../include/calc.hpp"

#include <tuple>
#include <unordered_map>
#include <cstdint>
#include <vector>

template <typename T1>
size_t vox::tuple_hash::operator()(const std::tuple<T1, T1, T1>& t) const {
    const auto hasher = std::hash<T1>{};
    
    size_t act_p = 1;
    size_t res = 0;

    res += hasher(std::get<0>(t)) * act_p; act_p = act_p * vox::MOD;
    res += hasher(std::get<1>(t)) * act_p; act_p = act_p * vox::MOD;
    res += hasher(std::get<2>(t)) * act_p;

    return res;
}

vox::out_data vox::calculate_vertices(const vox::read_data& read_data) {
    std::unordered_map<std::tuple<float, float, float>, std::tuple<uint16_t, uint16_t, uint16_t, uint8_t, uint8_t>, vox::tuple_hash> ver_col;

    std::vector<std::tuple<float, float, float>> vertices;
    std::vector<uint16_t> indices;

    const float offset[2] = {
        (read_data.size[0] - 1) / 2.0f,
        (read_data.size[2] - 1) / 2.0f
    };

    for(const auto& [pos, color] : read_data.voxels)
        for(uint8_t i=0; i<6; i++) {
            uint8_t center[3] = {
                std::get<0>(pos),
                std::get<1>(pos),
                std::get<2>(pos)
            };

            std::tuple<uint8_t, uint8_t, uint8_t> neighbour_pos = {
                center[0] + vox::neighbour_map[i][0],
                center[1] + vox::neighbour_map[i][1],
                center[2] + vox::neighbour_map[i][2]
            };

            if(read_data.voxels.find(neighbour_pos) != read_data.voxels.end()) 
                continue;

            std::tuple<float, float, float> f_ver[4];

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

                std::get<0>(ver) -= offset[0];
                std::get<2>(ver) -= offset[1];

                if(ver_col.find(ver) == ver_col.end()) {
                    vertices.push_back(ver);
                    idx[ii] = vertices.size() - 1;
                } else
                    idx[ii] = std::get<4>(ver_col[ver]);

                std::get<1>(ver_col[ver]) += read_data.palette[color][1]; // g
                std::get<2>(ver_col[ver]) += read_data.palette[color][2]; // b
                std::get<3>(ver_col[ver])++;                              // n (avg)
                std::get<4>(ver_col[ver]) = idx[ii];                      // idx
            }

            indices.push_back(idx[0]); indices.push_back(idx[1]); indices.push_back(idx[2]); // triangle 1
            indices.push_back(idx[1]); indices.push_back(idx[2]); indices.push_back(idx[3]); // triangle 2
        }

    for(auto& vtx : ver_col) {
        uint8_t n = std::get<3>(vtx.second);

        std::get<0>(vtx.second) /= n;
        std::get<1>(vtx.second) /= n;
        std::get<2>(vtx.second) /= n;
    }

    std::unordered_map<uint16_t, std::tuple<uint8_t, uint8_t, uint8_t>> colors;
    for(auto& vtx : ver_col) {
        const auto& color_data = vtx.second;
        colors[std::get<4>(color_data)] = {std::get<0>(color_data), std::get<1>(color_data), std::get<2>(color_data)};
    }

    return {
        colors,
        vertices,
        indices
    };
}