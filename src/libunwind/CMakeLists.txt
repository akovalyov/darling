project(darling-libsystem_unwind)

cmake_minimum_required(VERSION 2.4.0)
enable_language(C ASM)

set(CMAKE_CXX_LINKER_PREFERENCE_PROPAGATES 0)
set(CMAKE_CXX_IMPLICIT_LINK_LIBRARIES "")
set(CMAKE_CXX_IMPLICIT_LINK_DIRECTORIES "")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -nostdinc -nostdinc++ -ggdb -O0")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -msse -msse2 -msse3 -w -nostdinc -funwind-tables -ggdb -O0")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -D__DARWIN_UNIX03 -fPIC -w")
set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -nostdlib -Wl,--version-script=${DARLING_TOP_DIRECTORY}/darwin.map")

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../external/libcxx/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../libc/locale)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../libc/locale/FreeBSD)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../libc/stdtime/FreeBSD)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../libc/pthreads)
include_directories(${DARLING_TOP_DIRECTORY}/platform-include)

add_definitions(-Ddladdr=__darwin_dladdr)

set(libunwind_sources
	src/libuwind.cxx
	src/Registers.S
	src/unw_getcontext.S
	src/UnwindLevel1.c
	src/UnwindLevel1-gcc-ext.c
	src/Unwind-sjlj.c
)

SET(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}/darling")
SET(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE) 
SET(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

add_library(unwind SHARED ${libunwind_sources})
target_link_libraries(unwind system_c)
set_target_properties(unwind PROPERTIES LINKER_LANGUAGE C)

install(TARGETS unwind DESTINATION ${CMAKE_INSTALL_LIBDIR}/darling)
