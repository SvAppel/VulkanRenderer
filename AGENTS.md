# VulkanRenderer

CMake C++20 Vulkan rendering engine. No tests, no CI, no linter configured.

## Build

Out-of-source build in `build/`:

```bash
cmake -B build && cmake --build build
./build/VulkanRenderer
```

Ninja generator is recommended (install script uses `-G Ninja`).

## Setup

Run `./install_dependencies_linux.sh` for apt/dnf/pacman dep installation. The script does NOT install the Vulkan SDK — follow its printed instructions to download from lunarg and source `setup-env.sh`.

## Source Layout

- `src/main.cpp` — entrypoint; creates window, spawns render thread, runs App
- `src/backend/` — GLFW window creation (`build_window`)
- `src/renderer/` — `Engine` class; Vulkan instance creation using `vk::raii`
- `src/controller/` — `App` class; main loop
- `src/logging/` — singleton `Logger` (access via `Logger::get_logger()`)
- `dependencies/` — vendored Vulkan headers and GLFW headers

## Include Paths & Macros

- GLFW headers are included as `#include "GLFW/include/GLFW/glfw3.h"` (vendored path), NOT `<GLFW/glfw3.h>`
- Vulkan headers are included as `#include <vulkan/vulkan_raii.hpp>` — the `dependencies/` directory is the include root
- Internal source files use mixed include styles: `src/main.cpp` uses `"subdir/file.h"` while other files use relative paths like `"../logging/logger.h"`
- Before including any Vulkan C++ headers, define:
  ```cpp
  #define VULKAN_HPP_NO_EXCEPTIONS
  #define VULKAN_HPP_NO_STRUCT_CONSTRUCTORS
  ```
- Vulkan RAII (`vulkan_raii.hpp`) is used exclusively, not raw `vulkan.hpp`

## CMake Notes

- Custom `cmake/Findglfw3.cmake` handles GLFW discovery; contains hardcoded Windows paths — do NOT copy them
- `CMakeSettings.json` is VS 2022 config (Windows); Linux builds use standard cmake CLI
- `dependencies/vulkan/` and `dependencies/GLFW/include/` are vendored; GLFW `lib-vc2022/` contains Windows-only `.lib` files
