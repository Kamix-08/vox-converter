#include "../include/merge.hpp"

#include <set>
#include <map>

bool is_valid(const std::map<std::pair<uint8_t, uint8_t>, unsigned char>& cells, std::set<std::pair<uint8_t, uint8_t>>& visited, const std::pair<uint8_t, uint8_t>& next, unsigned char color) {
    if(visited.find(next) != visited.end()) return false;
    
    const auto& cell = cells.find(next);
    if(cell == cells.end() || cell->second != color) return false;

    return true;
}

std::vector<vox::vertex> vox::merge_voxels(vox::process_data process_data) {
    std::vector<vox::vertex> vertices;

    size_t size = 0;
    for (int i = 0; i < 6; i++)
        for(const auto& [_, second] : process_data.face_map[i])
            size += second.size() * 6;

    vertices.reserve(size);

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

                std::tuple<float, float, float> f_ver[4];

                switch (i) {
                case 0: // +x
                    f_ver[0] = {depth + 0.5f, res[0]       , res[3] + 0.5f};
                    f_ver[1] = {depth + 0.5f, res[2] + 1.0f, res[3] + 0.5f};
                    f_ver[2] = {depth + 0.5f, res[2] + 1.0f, res[1] - 0.5f};
                    f_ver[3] = {depth + 0.5f, res[0]       , res[1] - 0.5f};
                    break;
                case 1: // -x
                    f_ver[0] = {depth - 0.5f, res[2] + 1.0f, res[3] + 0.5f};
                    f_ver[1] = {depth - 0.5f, res[0]       , res[3] + 0.5f};
                    f_ver[2] = {depth - 0.5f, res[0]       , res[1] - 0.5f};
                    f_ver[3] = {depth - 0.5f, res[2] + 1.0f, res[1] - 0.5f};
                    break;
                case 2: // +y
                    f_ver[0] = {res[2] + 0.5f, depth + 1.0f, res[3] + 0.5f};
                    f_ver[1] = {res[0] - 0.5f, depth + 1.0f, res[3] + 0.5f};
                    f_ver[2] = {res[0] - 0.5f, depth + 1.0f, res[1] - 0.5f};
                    f_ver[3] = {res[2] + 0.5f, depth + 1.0f, res[1] - 0.5f};
                    break;
                case 3: // -y
                    f_ver[0] = {res[0] - 0.5f, depth       , res[3] + 0.5f};
                    f_ver[1] = {res[2] + 0.5f, depth       , res[3] + 0.5f};
                    f_ver[2] = {res[2] + 0.5f, depth       , res[1] - 0.5f};
                    f_ver[3] = {res[0] - 0.5f, depth       , res[1] - 0.5f};
                    break;
                case 4: // +z
                    f_ver[0] = {res[0] - 0.5f, res[3] + 1.0f, depth + 0.5f};
                    f_ver[1] = {res[2] + 0.5f, res[3] + 1.0f, depth + 0.5f};
                    f_ver[2] = {res[2] + 0.5f, res[1]       , depth + 0.5f};
                    f_ver[3] = {res[0] - 0.5f, res[1]       , depth + 0.5f};
                    break;
                case 5: // -z
                    f_ver[0] = {res[0] - 0.5f, res[1]       , depth - 0.5f};
                    f_ver[1] = {res[2] + 0.5f, res[1]       , depth - 0.5f};
                    f_ver[2] = {res[2] + 0.5f, res[3] + 1.0f, depth - 0.5f};
                    f_ver[3] = {res[0] - 0.5f, res[3] + 1.0f, depth - 0.5f};
                    break;
                }

                const auto& clr = process_data.palette[color];
                std::tuple<float, float, float> c = {
					clr[0] / 255.0f,
					clr[1] / 255.0f,
					clr[2] / 255.0f
				};

                vertices.push_back({f_ver[2], c}); vertices.push_back({f_ver[1], c}); vertices.push_back({f_ver[0], c}); // triangle 1
                vertices.push_back({f_ver[3], c}); vertices.push_back({f_ver[2], c}); vertices.push_back({f_ver[0], c}); // triangle 2
            }
        }

    return vertices;
}