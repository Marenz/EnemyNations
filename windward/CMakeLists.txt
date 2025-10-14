cmake_minimum_required(VERSION 3.20)

project(WindwardSupport LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_MFC_FLAG 2)

set(WINDWARD_SOURCES
    src/Acmutil.cpp
    src/BITBUFFE.CPP
    src/BTREE.CPP
    src/Bpecodec.cpp
    src/DAVIDINL.CPP
    src/DIRCORE.CPP
    src/Datafile.cpp
    src/DlgMsg.cpp
    src/FIXPOINT.CPP
    src/FLCCTRL.CPP
    src/GLOBAL.CPP
    src/HUFFMANC.CPP
    src/INIT.CPP
    src/LZSSCODE.CPP
    src/LZWCODEC.CPP
    src/MMIO.CPP
    src/MSG_BOX.CPP
    src/Music.cpp
    src/RAND.CPP
    src/SCANLIST.CPP
    src/STDAFX.CPP
    src/STREXT.CPP
    src/Threads.cpp
    src/apppalet.cpp
    src/blt.cpp
    src/cache.cpp
    src/codec.cpp
    src/dib.cpp
    src/logging.CPP
    src/mss_stub.cpp
    src/subclass.cpp
    src/vdmplay_stub.cpp
    src/wndbase.cpp
)

add_library(windward STATIC ${WINDWARD_SOURCES})

if(MSVC)
    target_compile_options(windward PRIVATE /utf-8)
endif()

target_include_directories(windward
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${CMAKE_CURRENT_SOURCE_DIR}/../src
)

target_precompile_headers(windward PRIVATE src/STDAFX.H)

target_compile_definitions(windward
    PRIVATE
        WIN32
        _WINDOWS
        _MBCS
        VDMPLAY_STATIC
)

target_link_libraries(windward
    PUBLIC
        winmm
        ddraw
        dsound
        dxguid
        msacm32
)

