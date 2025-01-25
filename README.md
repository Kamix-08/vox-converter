### vox-converter
![C++](https://img.shields.io/badge/C++-%2300599C.svg?style=for-the-badge&logo=c%2B%2B&logoColor=white)

**vox-converter** is a lightweight and efficient module for converting `.vox` files from _[MagicaVoxel](https://ephtracy.github.io/)_ into a structured list of vertices and their connections.

### Installation & Usage

To use the module, include the `Vox.hpp` header:

```cpp
#include "Vox.hpp"
```

It provides a single function, `load_model`, which loads a `.vox` file into a `model_data` structure:

```cpp
int vox::load_model(const std::string& filename, vox::model_data& model_data);
```

- Returns `0` if the operation is successful, and `1` otherwise.
- Overwrites the provided `model_data` object with extracted model data.

#### Example Usage

```cpp
#include "Vox.hpp"
#include <cassert>

int main() {
    const std::string file = "model.vox";

    vox::model_data model;
    assert(vox::load_model(file, model) == 0);

    return 0;
}
```

### Data Structure

The `vox::model_data` struct stores the extracted model information:

```cpp
struct model_data {
    std::unordered_map<uint16_t, std::tuple<uint8_t, uint8_t, uint8_t>> colors;
    std::vector<std::tuple<float, float, float>> vertices;
    std::vector<uint16_t> indices;
};
```

- `colors` maps each vertex (by index) to its RGB color `(0-255)`.
- `vertices` is a list of vertex positions, with the model centered at `(0,0,0)`.
- `indices` stores the vertex indices forming triangles.

#### Reading Triangles Correctly

Since `indices` represents raw vertex connections, triangles should be read as follows:

```cpp
for (size_t i = 0; i < indices.size(); i += 3) {
    triangles.push_back({
        indices[i],
        indices[i + 1],
        indices[i + 2]
    });
}
```

### Limitations & Notes

- **Opacity is ignored**—only RGB data is stored.
- **Supports only single-model `.vox` files**—files containing multiple models are not supported.
- **Missing `RGBA` chunks will cause an error.**
- **Model size limitation**—the module may not function correctly for models exceeding `255` units in any axis.

If you encounter issues or limitations, feel free to open an issue or submit a pull request!

### Acknowledgments

Special thanks to [@Pawello09](https://github.com/Pawello09) for invaluable help in designing, implementing, and debugging this module—it wouldn't exist without him!
