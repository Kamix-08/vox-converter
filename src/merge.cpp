#include "../include/merge.hpp"

#include <set>
#include <map>

bool is_valid(const std::map<std::pair<uint8_t, uint8_t>, unsigned char>& cells, std::set<std::pair<uint8_t, uint8_t>>& visited, const std::pair<uint8_t, uint8_t>& next, unsigned char color) {
    if(visited.find(next) != visited.end()) return false;
    
    const auto& cell = cells.find(next);
    if(cell == cells.end() || cell->second != color) return false;

    return true;
}

vox::model_data vox::merge_voxels(vox::process_data process_data) {
    std::vector<std::tuple<float, float, float>> vertices;
    std::vector<uint16_t> indices; 
    std::unordered_map<uint16_t, std::tuple<uint8_t, uint8_t, uint8_t>> colors;

    std::unordered_map<std::tuple<float, float, float>, std::tuple<uint16_t, uint16_t, uint16_t, uint8_t, uint16_t>, vox::tuple_hash> ver_col;

    for(int i=0; i<6; i++)
        for(const auto& [depth, layer] : process_data.face_map[i]) {
            std::map<std::pair<uint8_t, uint8_t>, unsigned char> cells;
            std::set<std::pair<uint8_t, uint8_t>> visited;

            for(const auto& [x, y, color] : layer)
                cells[{x, y}] = color;

            for(const auto& [pos, color] : cells) {
                if(visited.find(pos) != visited.end())
                    continue;

                uint8_t res[4] = {
                    pos.first, pos.second, 
                    pos.first, pos.second
                };

                while(true) {
                    std::pair<uint8_t, uint8_t> next = {res[2]+1, res[1]};

                    if(!is_valid(cells, visited, next, color))
                        break;

                    ++res[2];
                    visited.insert(next);
                }

                while (true) {
                    bool fine = true;
                    for (int j = res[0]; j <= res[2]; ++j) 
                        if(!(fine = is_valid(cells, visited, {j, res[3]+1}, color)))
                            break;

                    if(!fine) break;

                    ++res[3];
                    for (int j = res[0]; j <= res[2]; ++j) 
						visited.insert({j, res[3]});
                }

                std::tuple<float, float, float> f_ver[4] = {
                    {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}
                };

                switch (i) {
                case 0: // +x
                    f_ver[0] = {depth + 0.5f, res[0] - 0.5f, res[3] + 0.5f};
                    f_ver[1] = {depth + 0.5f, res[2] + 0.5f, res[3] + 0.5f};
                    f_ver[2] = {depth + 0.5f, res[2] + 0.5f, res[1] - 0.5f};
                    f_ver[3] = {depth + 0.5f, res[0] - 0.5f, res[1] - 0.5f};
                    break;
                case 1: // -x
                    f_ver[0] = {depth - 0.5f, res[2] + 0.5f, res[3] + 0.5f};
                    f_ver[1] = {depth - 0.5f, res[0] - 0.5f, res[3] + 0.5f};
                    f_ver[2] = {depth - 0.5f, res[0] - 0.5f, res[1] - 0.5f};
                    f_ver[3] = {depth - 0.5f, res[2] + 0.5f, res[1] - 0.5f};
                    break;
                case 2: // +y
                    f_ver[0] = {res[2] + 0.5f, depth + 0.5f, res[3] + 0.5f};
                    f_ver[1] = {res[0] - 0.5f, depth + 0.5f, res[3] + 0.5f};
                    f_ver[2] = {res[0] - 0.5f, depth + 0.5f, res[1] - 0.5f};
                    f_ver[3] = {res[2] + 0.5f, depth + 0.5f, res[1] - 0.5f};
                    break;
                case 3: // -y
                    f_ver[0] = {res[0] - 0.5f, depth - 0.5f, res[3] + 0.5f};
                    f_ver[1] = {res[2] + 0.5f, depth - 0.5f, res[3] + 0.5f};
                    f_ver[2] = {res[2] + 0.5f, depth - 0.5f, res[1] - 0.5f};
                    f_ver[3] = {res[0] - 0.5f, depth - 0.5f, res[1] - 0.5f};
                    break;
                case 4: // +z
                    f_ver[0] = {res[0] - 0.5f, res[3] + 0.5f, depth + 0.5f};
                    f_ver[1] = {res[2] + 0.5f, res[3] + 0.5f, depth + 0.5f};
                    f_ver[2] = {res[2] + 0.5f, res[1] - 0.5f, depth + 0.5f};
                    f_ver[3] = {res[0] - 0.5f, res[1] - 0.5f, depth + 0.5f};
                    break;
                case 5: // -z
                    f_ver[0] = {res[0] - 0.5f, res[1] - 0.5f, depth - 0.5f};
                    f_ver[1] = {res[2] + 0.5f, res[1] - 0.5f, depth - 0.5f};
                    f_ver[2] = {res[2] + 0.5f, res[3] + 0.5f, depth - 0.5f};
                    f_ver[3] = {res[0] - 0.5f, res[3] + 0.5f, depth - 0.5f};
                    break;
                }

                uint16_t ind[4];
                for(uint8_t j=0; j<4; j++) {
                    const auto& v = f_ver[j];
                    auto ver = ver_col.find(v);

                    if(ver == ver_col.end()) {
                        vertices.push_back(v);
                        std::get<4>(ver_col[v]) = vertices.size() - 1; // idx
                        ver = ver_col.find(v); // refresh ptr
                    }

                    const auto& clr = process_data.palette[color]; 

                    std::get<0>(ver->second) += clr[0]; // r
                    std::get<1>(ver->second) += clr[1]; // g
                    std::get<2>(ver->second) += clr[2]; // b

                    std::get<3>(ver->second)++; // n (avg)

                    ind[j] = std::get<4>(ver->second);
                }

                indices.push_back(ind[2]); indices.push_back(ind[1]); indices.push_back(ind[0]); // triangle 1
                indices.push_back(ind[3]); indices.push_back(ind[2]); indices.push_back(ind[0]); // triangle 2
            }
        }

    vertices.resize(ver_col.size());

    for(const auto& [ver, data] : ver_col) {
        uint8_t n = std::get<3>(data);
        uint16_t ind = std::get<4>(data);

        colors[ind] = {
            static_cast<uint8_t>(std::get<0>(data) / n),
            static_cast<uint8_t>(std::get<1>(data) / n),
            static_cast<uint8_t>(std::get<2>(data) / n)
        };

        vertices[ind] = ver;
    }

    return {
        colors,
        vertices,
        indices
    };
}