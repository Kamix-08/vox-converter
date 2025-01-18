#include <filesystem>

#include "components/files.cpp"

namespace fs = std::filesystem;

int main(int argc, char* argv[])
{
    if(argc != 1)
        for(int i=1; i<argc; i++)
            read_file(argv[i]);
    
    return 0;
}