#ifndef MERGE_HPP
#define MERGE_HPP

#include <unordered_map>
#include <cstdint>
#include <tuple>
#include <vector>

namespace vox {    
    const size_t MOD = 0xFFFFFFFFFFFFFFC5ULL;

    struct tuple_hash {
        template <typename T1>
        size_t operator()(const std::tuple<T1, T1, T1>& t) const {
            const auto hasher = std::hash<T1>{};
    
            size_t act_p = 1;
            size_t res = 0;

            res += hasher(std::get<0>(t)) * act_p; act_p = act_p * vox::MOD;
            res += hasher(std::get<1>(t)) * act_p; act_p = act_p * vox::MOD;
            res += hasher(std::get<2>(t)) * act_p;

            return res;
        }
    };

    struct model_data {
        std::unordered_map<uint16_t, std::tuple<uint8_t, uint8_t, uint8_t>> colors;
        std::vector<std::tuple<float, float, float>> vertices;
        std::vector<uint16_t> indices;
    };

    struct process_data {
        std::unordered_map<uint8_t, std::vector<std::tuple<uint8_t, uint8_t, unsigned char>>>* face_map;
        const float* offset;
        unsigned char (*palette)[4];
    };

    model_data merge_voxels(process_data);
}

#endif // MERGE_HPP