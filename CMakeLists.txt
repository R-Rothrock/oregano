# CMakeLists.txt

cmake_minimum_required(VERSION 3.10)

set(CMAKE_C_COMPILER "/usr/bin/gcc")

project("Oregano"
		VERSION 1.0
		DESCRIPTION "PoC Linux process injection (process hollowing) to hide execution of Linux binary in Htop process."
		LANGUAGES C
		)

add_executable(oregano src/oregano.c)
