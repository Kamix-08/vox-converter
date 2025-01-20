#ifndef READ_HPP
#define READ_HPP

#include <string>
#include "calc.hpp"

namespace vox {
    int load_model(const std::string& filename, vox::out_data& model_data);
}

#endif // READ_HPP