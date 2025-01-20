#include <filesystem>

#include "components/read.cpp"

namespace fs = std::filesystem;

int main(int argc, char* argv[])
{
    vector<out_data> models;
    out_data tmp;

    if(argc == 1) {
        for(const auto& entry : fs::directory_iterator("./"))
            if(entry.path().extension() == ".vox") {
                cout << "[.] Found model: " << entry.path().filename() << "\n";
                load_model(entry.path().string(), tmp);

                models.push_back(tmp);
            }
    } else {
        models.resize(argc-1);

        for(int i=1; i<argc; i++)
            load_model(argv[i], models[i-1]);
    }

    cout << "[.] Loaded " << models.size() << " models into the vector.\n";
    
    return 0;
}