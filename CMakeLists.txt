# CMakeLists.txt

cmake_minimum_required(VERSION 3.10)

set(CMAKE_C_COMPILER "/usr/bin/clang")

project("Oregano"
		VERSION 1.0
		DESCRIPTION "Htop process hollower to hidden file execution"
		LANGUAGES C
		)

add_executable(oregano src/oregano.c)
