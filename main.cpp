#include <filesystem>

#include "components/read.cpp"

namespace fs = std::filesystem;

int main(int argc, char* argv[])
{
    if(argc != 1)
        for(int i=1; i<argc; i++)
            load_model(argv[i]);
    
    return 0;
}