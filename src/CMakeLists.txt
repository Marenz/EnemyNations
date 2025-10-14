cmake_minimum_required(VERSION 3.20)

project(EnemyNations LANGUAGES CXX)

include(CTest)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_MFC_FLAG 2)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../windward ${CMAKE_BINARY_DIR}/windward)

file(READ ${CMAKE_CURRENT_SOURCE_DIR}/enations.dsp ENATIONS_DSP_CONTENT)
string(REGEX MATCHALL "SOURCE=\\.\\\\([^\\r\\n]+)" ENATIONS_DSP_MATCHES "${ENATIONS_DSP_CONTENT}")

file(
    GLOB_RECURSE
    ENATIONS_SOURCE_CANDIDATES
    RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    CONFIGURE_DEPENDS
    *.c
    *.C
    *.cxx
    *.cpp
    *.CPP
    *.rc
    *.RC
)

set(_VALID_EXTENSIONS .c .cpp .cxx .rc)
set(GAME_SOURCES)
foreach(_entry IN LISTS ENATIONS_DSP_MATCHES)
    string(REGEX REPLACE "SOURCE=\\.\\\\([^\\r\\n]+)" "\\1" _relative "${_entry}")
    string(REPLACE "\\\" "\"" _relative "${_relative}")
    string(REPLACE "\\\\" "/" _relative "${_relative}")
    string(TOLOWER "${_relative}" _relative_lower)
    get_filename_component(_ext "${_relative_lower}" EXT)

    set(_ext_valid OFF)
    foreach(_valid_ext IN LISTS _VALID_EXTENSIONS)
        if(_ext STREQUAL _valid_ext)
            set(_ext_valid ON)
            break()
        endif()
    endforeach()

    if(NOT _ext_valid)
        continue()
    endif()

    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${_relative}")
        set(_resolved "${_relative}")
    else()
        set(_resolved "")
        foreach(_candidate IN LISTS ENATIONS_SOURCE_CANDIDATES)
            string(TOLOWER "${_candidate}" _candidate_lower)
            if(_candidate_lower STREQUAL _relative_lower)
                set(_resolved "${_candidate}")
                break()
            endif()
        endforeach()
        if(_resolved STREQUAL "")
            message(FATAL_ERROR "Unable to resolve source '${_relative}' listed in enations.dsp")
        endif()
    endif()

    list(APPEND GAME_SOURCES "${_resolved}")
endforeach()

list(REMOVE_DUPLICATES GAME_SOURCES)

if(GAME_SOURCES STREQUAL "")
    message(FATAL_ERROR "No source files were discovered in enations.dsp")
endif()

set(GAME_SOURCES_ABSOLUTE)
foreach(_src IN LISTS GAME_SOURCES)
    list(APPEND GAME_SOURCES_ABSOLUTE "${CMAKE_CURRENT_SOURCE_DIR}/${_src}")
endforeach()

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${GAME_SOURCES_ABSOLUTE})

add_executable(EnemyNations WIN32 ${GAME_SOURCES})

if(MSVC)
    target_compile_options(EnemyNations PRIVATE /utf-8)
endif()

target_precompile_headers(EnemyNations PRIVATE stdafx.h)

target_include_directories(EnemyNations
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/../windward/include
)

target_compile_definitions(EnemyNations
    PRIVATE
        WIN32
        _WINDOWS
        _MBCS
        _AFXDLL
        VDMPLAY_STATIC
)

target_link_libraries(EnemyNations
    PRIVATE
        windward
        shlwapi
        vfw32
        winmm
        version
        comctl32
        comdlg32
        user32
        gdi32
)

if(BUILD_TESTING)
    add_subdirectory(tests)
endif()

