# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.12

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:


# Remove some rules from gmake that .SUFFIXES does not remove.
SUFFIXES =

.SUFFIXES: .hpux_make_needs_suffix_list


# Suppress display of executed commands.
$(VERBOSE).SILENT:


# A target that is always out of date.
cmake_force:

.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/lyra/tmp/Norilsk/pult411

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/lyra/tmp/Norilsk/pult411/build

# Utility rule file for pult411.hex.

# Include the progress variables for this target.
include CMakeFiles/pult411.hex.dir/progress.make

CMakeFiles/pult411.hex: pult411.elf
	arm-none-eabi-objcopy -Oihex pult411.elf pult411.hex

pult411.hex: CMakeFiles/pult411.hex
pult411.hex: CMakeFiles/pult411.hex.dir/build.make

.PHONY : pult411.hex

# Rule to build all files generated by this target.
CMakeFiles/pult411.hex.dir/build: pult411.hex

.PHONY : CMakeFiles/pult411.hex.dir/build

CMakeFiles/pult411.hex.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/pult411.hex.dir/cmake_clean.cmake
.PHONY : CMakeFiles/pult411.hex.dir/clean

CMakeFiles/pult411.hex.dir/depend:
	cd /home/lyra/tmp/Norilsk/pult411/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/lyra/tmp/Norilsk/pult411 /home/lyra/tmp/Norilsk/pult411 /home/lyra/tmp/Norilsk/pult411/build /home/lyra/tmp/Norilsk/pult411/build /home/lyra/tmp/Norilsk/pult411/build/CMakeFiles/pult411.hex.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/pult411.hex.dir/depend

