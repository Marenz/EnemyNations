add_executable(EnemyNationsTests
    scanline_sprite_tests.cpp
)

target_include_directories(EnemyNationsTests
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/..
        ${CMAKE_CURRENT_SOURCE_DIR}/../detail
        ${CMAKE_CURRENT_SOURCE_DIR}/../../windward/include
)

target_compile_features(EnemyNationsTests PRIVATE cxx_std_17)

add_test(NAME EnemyNationsTests COMMAND EnemyNationsTests)
