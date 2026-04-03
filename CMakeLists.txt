cmake_minimum_required(VERSION 3.25)

project(
    quantum-computing-cpp
    VERSION 0.1.0
    DESCRIPTION "A C++23 quantum computing simulation library"
    LANGUAGES CXX
)

# ── C++23 standard ──────────────────────────────────────────────────────────
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# ── Cross-platform compile options ──────────────────────────────────────────
if(MSVC)
    add_compile_options(/W4 /permissive-)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# ── Options ─────────────────────────────────────────────────────────────────
option(BUILD_TESTS "Build unit tests" OFF)

# ── Subdirectories ───────────────────────────────────────────────────────────
add_subdirectory(src)

if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()
