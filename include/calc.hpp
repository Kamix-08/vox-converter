#ifndef CALC_HPP
#define CALC_HPP

#include <unordered_map>
#include <cstdint>
#include <tuple>
#include <vector>

namespace vox {
    // why is this not built in??
    struct tuple_hash {
        template <typename T1, typename T2, typename T3>
        size_t operator()(const std::tuple<T1, T2, T3>& t) const;
    };

    struct read_data {
        uint32_t* size;
        std::unordered_map<std::tuple<uint8_t, uint8_t, uint8_t>, unsigned char, vox::tuple_hash> voxels;
        char (*palette)[4];
    };
    
    struct out_data {
        std::unordered_map<uint16_t, std::tuple<uint8_t, uint8_t, uint8_t>> colors;
        std::vector<std::tuple<float, float, float>> vertices;
        std::vector<uint16_t> indices;
    };

    const char neighbour_map[6][3] = {
        {1, 0, 0}, {-1, 0, 0}, // +x, -x
        {0, 1, 0}, { 0,-1, 0}, // +y, -y
        {0, 0, 1}, { 0, 0,-1}  // +z, -z
    };

    out_data calculate_vertices(const read_data& read_data);
}

#endif // CALC_HPP