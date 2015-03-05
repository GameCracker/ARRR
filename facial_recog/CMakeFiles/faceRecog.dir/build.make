# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 2.8

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
CMAKE_SOURCE_DIR = /media/psf/Dropbox/bci/x/ar/facial_recog

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /media/psf/Dropbox/bci/x/ar/facial_recog

# Include any dependencies generated for this target.
include CMakeFiles/faceRecog.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/faceRecog.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/faceRecog.dir/flags.make

CMakeFiles/faceRecog.dir/faceRecog.cpp.o: CMakeFiles/faceRecog.dir/flags.make
CMakeFiles/faceRecog.dir/faceRecog.cpp.o: faceRecog.cpp
	$(CMAKE_COMMAND) -E cmake_progress_report /media/psf/Dropbox/bci/x/ar/facial_recog/CMakeFiles $(CMAKE_PROGRESS_1)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building CXX object CMakeFiles/faceRecog.dir/faceRecog.cpp.o"
	/usr/bin/c++   $(CXX_DEFINES) $(CXX_FLAGS) -o CMakeFiles/faceRecog.dir/faceRecog.cpp.o -c /media/psf/Dropbox/bci/x/ar/facial_recog/faceRecog.cpp

CMakeFiles/faceRecog.dir/faceRecog.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/faceRecog.dir/faceRecog.cpp.i"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -E /media/psf/Dropbox/bci/x/ar/facial_recog/faceRecog.cpp > CMakeFiles/faceRecog.dir/faceRecog.cpp.i

CMakeFiles/faceRecog.dir/faceRecog.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/faceRecog.dir/faceRecog.cpp.s"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -S /media/psf/Dropbox/bci/x/ar/facial_recog/faceRecog.cpp -o CMakeFiles/faceRecog.dir/faceRecog.cpp.s

CMakeFiles/faceRecog.dir/faceRecog.cpp.o.requires:
.PHONY : CMakeFiles/faceRecog.dir/faceRecog.cpp.o.requires

CMakeFiles/faceRecog.dir/faceRecog.cpp.o.provides: CMakeFiles/faceRecog.dir/faceRecog.cpp.o.requires
	$(MAKE) -f CMakeFiles/faceRecog.dir/build.make CMakeFiles/faceRecog.dir/faceRecog.cpp.o.provides.build
.PHONY : CMakeFiles/faceRecog.dir/faceRecog.cpp.o.provides

CMakeFiles/faceRecog.dir/faceRecog.cpp.o.provides.build: CMakeFiles/faceRecog.dir/faceRecog.cpp.o

# Object files for target faceRecog
faceRecog_OBJECTS = \
"CMakeFiles/faceRecog.dir/faceRecog.cpp.o"

# External object files for target faceRecog
faceRecog_EXTERNAL_OBJECTS =

faceRecog: CMakeFiles/faceRecog.dir/faceRecog.cpp.o
faceRecog: CMakeFiles/faceRecog.dir/build.make
faceRecog: /usr/local/lib/libopencv_videostab.so.2.4.9
faceRecog: /usr/local/lib/libopencv_video.so.2.4.9
faceRecog: /usr/local/lib/libopencv_ts.a
faceRecog: /usr/local/lib/libopencv_superres.so.2.4.9
faceRecog: /usr/local/lib/libopencv_stitching.so.2.4.9
faceRecog: /usr/local/lib/libopencv_photo.so.2.4.9
faceRecog: /usr/local/lib/libopencv_ocl.so.2.4.9
faceRecog: /usr/local/lib/libopencv_objdetect.so.2.4.9
faceRecog: /usr/local/lib/libopencv_nonfree.so.2.4.9
faceRecog: /usr/local/lib/libopencv_ml.so.2.4.9
faceRecog: /usr/local/lib/libopencv_legacy.so.2.4.9
faceRecog: /usr/local/lib/libopencv_imgproc.so.2.4.9
faceRecog: /usr/local/lib/libopencv_highgui.so.2.4.9
faceRecog: /usr/local/lib/libopencv_gpu.so.2.4.9
faceRecog: /usr/local/lib/libopencv_flann.so.2.4.9
faceRecog: /usr/local/lib/libopencv_features2d.so.2.4.9
faceRecog: /usr/local/lib/libopencv_core.so.2.4.9
faceRecog: /usr/local/lib/libopencv_contrib.so.2.4.9
faceRecog: /usr/local/lib/libopencv_calib3d.so.2.4.9
faceRecog: /usr/lib/x86_64-linux-gnu/libGLU.so
faceRecog: /usr/lib/x86_64-linux-gnu/libGL.so
faceRecog: /usr/lib/x86_64-linux-gnu/libSM.so
faceRecog: /usr/lib/x86_64-linux-gnu/libICE.so
faceRecog: /usr/lib/x86_64-linux-gnu/libX11.so
faceRecog: /usr/lib/x86_64-linux-gnu/libXext.so
faceRecog: /usr/local/lib/libopencv_nonfree.so.2.4.9
faceRecog: /usr/local/lib/libopencv_ocl.so.2.4.9
faceRecog: /usr/local/lib/libopencv_gpu.so.2.4.9
faceRecog: /usr/local/lib/libopencv_photo.so.2.4.9
faceRecog: /usr/local/lib/libopencv_objdetect.so.2.4.9
faceRecog: /usr/local/lib/libopencv_legacy.so.2.4.9
faceRecog: /usr/local/lib/libopencv_video.so.2.4.9
faceRecog: /usr/local/lib/libopencv_ml.so.2.4.9
faceRecog: /usr/local/lib/libopencv_calib3d.so.2.4.9
faceRecog: /usr/local/lib/libopencv_features2d.so.2.4.9
faceRecog: /usr/local/lib/libopencv_highgui.so.2.4.9
faceRecog: /usr/local/lib/libopencv_imgproc.so.2.4.9
faceRecog: /usr/local/lib/libopencv_flann.so.2.4.9
faceRecog: /usr/local/lib/libopencv_core.so.2.4.9
faceRecog: CMakeFiles/faceRecog.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --red --bold "Linking CXX executable faceRecog"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/faceRecog.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/faceRecog.dir/build: faceRecog
.PHONY : CMakeFiles/faceRecog.dir/build

CMakeFiles/faceRecog.dir/requires: CMakeFiles/faceRecog.dir/faceRecog.cpp.o.requires
.PHONY : CMakeFiles/faceRecog.dir/requires

CMakeFiles/faceRecog.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/faceRecog.dir/cmake_clean.cmake
.PHONY : CMakeFiles/faceRecog.dir/clean

CMakeFiles/faceRecog.dir/depend:
	cd /media/psf/Dropbox/bci/x/ar/facial_recog && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /media/psf/Dropbox/bci/x/ar/facial_recog /media/psf/Dropbox/bci/x/ar/facial_recog /media/psf/Dropbox/bci/x/ar/facial_recog /media/psf/Dropbox/bci/x/ar/facial_recog /media/psf/Dropbox/bci/x/ar/facial_recog/CMakeFiles/faceRecog.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/faceRecog.dir/depend

