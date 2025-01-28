#include "../include/calc.hpp"

#include <tuple>
#include <unordered_map>
#include <cstdint>
#include <vector>

std::vector<vox::vertex> vox::calculate_vertices(const vox::read_data& read_data) {
    const float offset[2] = {
        (read_data.size[0] - 1) / 2.0f,
        (read_data.size[1] - 1) / 2.0f
    };

    std::unordered_map<uint8_t, std::vector<std::tuple<uint8_t, uint8_t, unsigned char>>> face_map[6];

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

            uint8_t ver[2];

            switch (i)
            {
            case 0:
            case 1:
                ver[0] = center[1];
                ver[1] = center[2];
                break;
            case 2:
            case 3:
                ver[0] = center[0];
                ver[1] = center[2];
                break;
            case 4:
            case 5:
                ver[0] = center[0];
                ver[1] = center[1];
                break;
            }

            face_map[i][center[i/2]].push_back({ver[0], ver[1], color});
        }

    return vox::merge_voxels({
        face_map,
        offset,
        read_data.palette
    });
}