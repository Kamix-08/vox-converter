### vox-converter
![C++](https://img.shields.io/badge/C++-%2300599C.svg?style=for-the-badge&logo=c%2B%2B&logoColor=white)

**vox-converter** is a lightweight and efficient module for converting `.vox` files from _[MagicaVoxel](https://ephtracy.github.io/)_ into a structured list of vertices and their connections.

### Installation & Usage

To use the module, include the `Vox.hpp` header:

```cpp
#include "Vox.hpp"
```

It provides a single function, `load_model`, which loads a `.vox` file into a `vector` of `vox::vertex`:

```cpp
int vox::load_model(const std::string& filename, std::vector<vox::vertex>& model_data);
```

- Returns `0` if the operation is successful, and `1` otherwise.
- Overwrites the provided `vector` object with extracted model data.

#### Example Usage

```cpp
#include "Vox.hpp"
#include <cassert>
#include <vector>

int main() {
    const std::string file = "model.vox";

    std::vector<vox::vertex> model;
    assert(vox::load_model(file, model) == 0);

    return 0;
}
```

### Data Structure

The `vox::vertex` struct stores the information about each vertex:

```cpp
struct vertex {
    std::tuple<float, float, float> pos;
    std::tuple<float, float, float> col;
};
```

- `pos` is the position of the vertex (the model is centered at `(0,0)`).
- `col` is the color data, RGB format, values are in the `(0,1)` range.

#### Reading Triangles Correctly

Since `load_models` return raw vertices, triangles should be read as follows:

```cpp
for (size_t i = 0; i < vertices.size(); i += 3) {
    triangles.push_back({
        veritces[i],
        veritces[i + 1],
        veritces[i + 2]
    });
}
```

### Limitations & Notes

- **Opacity is ignored** — only RGB data is stored.
- **Supports only single-model `.vox` files**—files containing multiple models are not supported.
- **Model size limitation** — the module may not function correctly for models exceeding `255` units in any axis.
- **Coordinate system** matches that of MagicaVoxel - `XZY` (`Z` axis points up), as opposed to "standard" `XYZ`.

If you encounter issues or limitations, feel free to open an issue or submit a pull request!

### Acknowledgments

Special thanks to **[@Pawello09](https://github.com/Pawello09)** for invaluable help in designing, implementing, and debugging this module — it wouldn't exist without him!
