#ifndef CALC_HPP
#define CALC_HPP

#include <unordered_map>
#include <cstdint>
#include <tuple>
#include <vector>
#include "merge.hpp"

namespace vox {
    struct read_data {
        uint32_t* size;
        std::unordered_map<std::tuple<uint8_t, uint8_t, uint8_t>, unsigned char, vox::tuple_hash> voxels;
        char (*palette)[4];
    };

    const char neighbour_map[6][3] = {
        {1, 0, 0}, {-1, 0, 0}, // +x, -x
        {0, 1, 0}, { 0,-1, 0}, // +y, -y
        {0, 0, 1}, { 0, 0,-1}  // +z, -z
    };

    model_data calculate_vertices(const read_data&);
}

#endif // CALC_HPP