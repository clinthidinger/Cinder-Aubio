cmake_minimum_required( VERSION 3.0 FATAL_ERROR )
set( CMAKE_VERBOSE_MAKEFILE ON )

project( aubio LANGUAGES C )

file(GLOB_RECURSE SRC src/*.c src/*.h)

add_library( ${PROJECT_NAME} ${SRC} )

target_include_directories( ${PROJECT_NAME} PUBLIC src )

target_compile_definitions( ${PROJECT_NAME} PUBLIC _USE_MATH_DEFINES=1 HAVE_C99_VARARGS_MACROS=1 NULL=0)

function(makeGroups fileList baseDir)
	foreach(FILE ${fileList}) 
	    # Get the directory of the source file
	    get_filename_component(PARENT_DIR "${FILE}" DIRECTORY)
	    # Remove common directory prefix to make the group
	    string(REPLACE "${baseDir}" "" GROUP "${PARENT_DIR}")
	    # Make sure we are using windows slashes
	    string(REPLACE "/" "\\" GROUP "${GROUP}")
	    source_group("${GROUP}" FILES "${FILE}")
	endforeach()
endfunction()

makeGroups("${SRC}" ${CMAKE_SOURCE_DIR})

