#include <filesystem>
#include <vector>
#include <iostream>
#include <fstream>
#include <string>

#include "../Vox.hpp"

using namespace std;
using namespace vox;

namespace fs = std::filesystem;

int main(int argc, char* argv[])
{
    vector<model_data> models;
    vector<string> names;
    
    model_data tmp;

    if(argc == 1) {
        for(const auto& entry : fs::directory_iterator("./"))
            if(entry.path().extension() == ".vox") {
                auto& name = entry.path();

                cout << "[.] Found model: " << name.filename() << "\n";
                load_model(name.string(), tmp);

                models.push_back(tmp);
                names.push_back(name.string());
            }
    } else {
        models.resize(argc-1);

        for(int i=1; i<argc; i++) {
            load_model(argv[i], models[i-1]);
            names.push_back(argv[i]);
        }
    }

    cout << "[.] Loaded " << models.size() << " models into the vector.\n";

    ofstream out("output.txt");

    for(size_t i=0; i<models.size(); i++) {
        out << "=== " << names[i] << " ===\n\n";

        for(size_t j=0; j<models[i].vertices.size(); j++) {
            const auto& [x, y, z] = models[i].vertices[j];
            out << x << ", " << y << ", " << z << "\n";
        }

        out << "\n";

        for(size_t j=0; j<models[i].indices.size(); j++) {
            out << models[i].indices[j] << ", ";
            if(j % 3 == 2) out << "\n";
        }

        out << "\n";

        for(const auto& [idx, color] : models[i].colors) {
            const auto& [r, g, b] = color;
            out << (int)r << ", " << (int)g << ", " << (int)b << "\n";
        }

        out << "\n";
    }

    out.close();

    cout << "[.] Output saved to output.txt.\n";
    
    return 0;
}