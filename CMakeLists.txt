# CMakeLists.txt --- CMake project settings
#    ex) cmake -G "Visual Studio 9 2008" .
#    ex) cmake -DCMAKE_BUILD_TYPE=Release -G "MSYS Makefiles" .
##############################################################################

# CMake minimum version
cmake_minimum_required(VERSION 2.4)

# enable testing
enable_testing()

if(COMMAND cmake_policy)
  cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)

# project name and language
project(xword CXX)

# check build type
if (NOT CMAKE_BUILD_TYPE)
    message(STATUS "No build type selected, default to Debug")
    set(CMAKE_BUILD_TYPE "Debug")
endif()

# make uppercase string of build type
string(TOUPPER "${CMAKE_BUILD_TYPE}" uppercase_CMAKE_BUILD_TYPE)

# check build type again
if (NOT uppercase_CMAKE_BUILD_TYPE MATCHES "^(DEBUG|RELEASE|RELWITHDEBINFO|MINSIZEREL)$")
    message(FATAL_ERROR "Invalid value for CMAKE_BUILD_TYPE: ${CMAKE_BUILD_TYPE}")
endif()

if (WIN32)
    # enable Win32 resource
    enable_language(RC)

    if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
        # using Clang
        if (uppercase_CMAKE_BUILD_TYPE MATCHES "^RELEASE$")
            set(CMAKE_C_FLAGS "-static -O9 -mtune=skylake -Wall -pedantic")
            set(CMAKE_CXX_FLAGS "-static -O9 -mtune=skylake -Wall -pedantic")
        else()
            set(CMAKE_C_FLAGS "-static -g -Wall -pedantic")
            set(CMAKE_CXX_FLAGS "-static -g -Wall -pedantic")
        endif()
    elseif (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        # using GCC
        if (uppercase_CMAKE_BUILD_TYPE MATCHES "^RELEASE$")
            set(CMAKE_C_FLAGS "-static -s -O9 -mtune=skylake -Wall -pedantic")
            set(CMAKE_CXX_FLAGS "-static -s -O9 -mtune=skylake -Wall -pedantic")
        else()
            set(CMAKE_C_FLAGS "-static -g -Wall -pedantic")
            set(CMAKE_CXX_FLAGS "-static -g -Wall -pedantic")
        endif()
    elseif (MSVC)
        # replace "/MD" with "/MT" (building without runtime DLLs)
        set(CompilerFlags
            CMAKE_C_FLAGS
            CMAKE_C_FLAGS_DEBUG
            CMAKE_C_FLAGS_RELEASE
            CMAKE_C_FLAGS_RELWITHDEBINFO
            CMAKE_CXX_FLAGS
            CMAKE_CXX_FLAGS_DEBUG
            CMAKE_CXX_FLAGS_RELEASE
            CMAKE_CXX_FLAGS_RELWITHDEBINFO)
        foreach(CompilerFlags ${CompilerFlags})
            string(REPLACE "/MD" "/MT" ${CompilerFlags} "${${CompilerFlags}}")
        endforeach()
    endif()
endif()

##############################################################################

include_directories(. json/include SaveBitmapToFile)

add_definitions(-DMZC_NO_SHAREWARE -DUNICODE -D_UNICODE)

subdirs(SaveBitmapToFile tests)

if (CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(BITS 64)
else()
    set(BITS 32)
endif()

if (MSVC)
    add_definitions(-DMSVC)
    set(CMAKE_EXE_LINKER_FLAGS    "${CMAKE_EXE_LINKER_FLAGS} /MANIFEST:NO")
    set(CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} /MANIFEST:NO")
    set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} /MANIFEST:NO")
endif()

# xword.exe
add_executable(xword${BITS} WIN32
    Dictionary.cpp
    GUI.cpp
    Input.cpp
    Marks.cpp
    ScrollView.cpp
    UndoBuffer.cpp
    Utils.cpp
    xword.cpp
    xword_res.rc)
target_link_libraries(xword${BITS}
	comctl32 imm32 shell32 ole32 uuid shlwapi SaveBitmapToFile)

##############################################################################
