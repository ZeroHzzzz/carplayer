# CMAKE generated file: DO NOT EDIT!
# Generated by "MinGW Makefiles" Generator, CMake Version 3.30

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:

#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:

# Disable VCS-based implicit rules.
% : %,v

# Disable VCS-based implicit rules.
% : RCS/%

# Disable VCS-based implicit rules.
% : RCS/%,v

# Disable VCS-based implicit rules.
% : SCCS/s.%

# Disable VCS-based implicit rules.
% : s.%

.SUFFIXES: .hpux_make_needs_suffix_list

# Command-line flag to silence nested $(MAKE).
$(VERBOSE)MAKESILENT = -s

#Suppress display of executed commands.
$(VERBOSE).SILENT:

# A target that is always out of date.
cmake_force:
.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

SHELL = cmd.exe

# The CMake executable.
CMAKE_COMMAND = "C:\Program Files\CMake\bin\cmake.exe"

# The command to remove a file.
RM = "C:\Program Files\CMake\bin\cmake.exe" -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = C:\Users\ZeroHzzzz\Desktop\QT_new\src\NewPlayer

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = C:\Users\ZeroHzzzz\Desktop\QT_new\src\NewPlayer\build

# Utility rule file for NewPlayer_autogen.

# Include any custom commands dependencies for this target.
include CMakeFiles/NewPlayer_autogen.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/NewPlayer_autogen.dir/progress.make

CMakeFiles/NewPlayer_autogen:
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --blue --bold --progress-dir=C:\Users\ZeroHzzzz\Desktop\QT_new\src\NewPlayer\build\CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Automatic MOC and UIC for target NewPlayer"
	"C:\Program Files\CMake\bin\cmake.exe" -E cmake_autogen C:/Users/ZeroHzzzz/Desktop/QT_new/src/NewPlayer/build/CMakeFiles/NewPlayer_autogen.dir/AutogenInfo.json Debug

NewPlayer_autogen: CMakeFiles/NewPlayer_autogen
NewPlayer_autogen: CMakeFiles/NewPlayer_autogen.dir/build.make
.PHONY : NewPlayer_autogen

# Rule to build all files generated by this target.
CMakeFiles/NewPlayer_autogen.dir/build: NewPlayer_autogen
.PHONY : CMakeFiles/NewPlayer_autogen.dir/build

CMakeFiles/NewPlayer_autogen.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles\NewPlayer_autogen.dir\cmake_clean.cmake
.PHONY : CMakeFiles/NewPlayer_autogen.dir/clean

CMakeFiles/NewPlayer_autogen.dir/depend:
	$(CMAKE_COMMAND) -E cmake_depends "MinGW Makefiles" C:\Users\ZeroHzzzz\Desktop\QT_new\src\NewPlayer C:\Users\ZeroHzzzz\Desktop\QT_new\src\NewPlayer C:\Users\ZeroHzzzz\Desktop\QT_new\src\NewPlayer\build C:\Users\ZeroHzzzz\Desktop\QT_new\src\NewPlayer\build C:\Users\ZeroHzzzz\Desktop\QT_new\src\NewPlayer\build\CMakeFiles\NewPlayer_autogen.dir\DependInfo.cmake "--color=$(COLOR)"
.PHONY : CMakeFiles/NewPlayer_autogen.dir/depend

