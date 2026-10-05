# Parallel Rendering Engine

A high-performance, multi-threaded 3D rendering engine developed in modern C++23. This project leverages Intel Threading Building Blocks (TBB) to efficiently distribute intensive ray-tracing computational tasks across multiple CPU cores, outputting 2D images in PPM format.

## Technical Features
* **Core Language:** Built entirely with C++23, enforcing strict modern standards and memory safety.
* **Parallelization:** Utilizes Intel TBB for concurrent execution, evaluating dynamic workload distribution strategies such as `simple_partitioner`, `static_partitioner`, and `auto_partitioner`.
* **Concurrency Safety:** Implements thread-local pseudo-random number generation (Mersenne-Twister 64-bit) via `tbb::enumerable_thread_specific` to prevent race conditions without locking bottlenecks.
* **Code Quality:** Enforces strict adherence to the C++ Core Guidelines using Microsoft's Guidelines Support Library (GSL). The codebase is thoroughly validated through `.clang-format` and `.clang-tidy` static analysis.
* **Build & Testing:** Managed via CMake (v4.0+) with multi-config presets (`default`, `clang-tidy`, `gcc-release`) and fully integrated with GoogleTest for automated unit testing.

## Compilation & Execution Instructions
1. Configure the project:

   ```bash
   # Standard configuration
   cmake --preset default

   # Configuration with Clang-Tidy enabled for static analysis
   cmake --preset clang-tidy
   ```
2. Compile the project:

   ```bash
   # Standard builds (if configured with 'default')
   cmake --build --preset gcc-release    # Recommended for performance evaluation
   cmake --build --preset gcc-debug      # Recommended for development and debugging

   # Clang-Tidy builds (if configured with 'clang-tidy')
   cmake --build --preset clang-tidy-release
   cmake --build --preset clang-tidy-debug
   ```
3. Run the engine:

   ```bash
   # If configured with 'default' preset:
   ./out/build/default/render-par config.txt scene.txt output.ppm

   # If configured with 'clang-tidy' preset:
   ./out/build/clang-tidy/render-par config.txt scene.txt output.ppm
   ```
4. Run unit tests:

   ```bash
   cd out/build/default
   ctest --output-on-failure
   ```
