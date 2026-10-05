add_library(common STATIC)

target_sources(common 
    PRIVATE
        src/parse_config.cpp
        src/parse_scene.cpp
        src/pov.cpp
        src/ray.cpp
        src/logic.cpp
        src/scene.cpp
        src/vector.cpp
)

target_include_directories(common PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_link_libraries(common PUBLIC Microsoft.GSL::GSL TBB::tbb)
