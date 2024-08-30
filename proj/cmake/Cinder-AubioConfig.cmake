
if( NOT TARGET Cinder-Aubio )
	get_filename_component( Aubio_SOURCE_PATH "${CMAKE_CURRENT_LIST_DIR}/../../deps/aubio/src" ABSOLUTE )
	get_filename_component( Cinder-Aubio_INC_PATH "${CMAKE_CURRENT_LIST_DIR}/../../include" ABSOLUTE )
	get_filename_component( Cinder-Aubio_SOURCE_PATH "${CMAKE_CURRENT_LIST_DIR}/../../src" ABSOLUTE )
	
	message(Aubio_SOURCE_PATH..........${Aubio_SOURCE_PATH})
	file(GLOB_RECURSE Aubio_SOURCES ${Aubio_SOURCE_PATH}/*.c ${Aubio_SOURCE_PATH}/*.h)

	set( Cinder-Aubio_INCLUDES
		${Cinder-Aubio_INC_PATH}/CinderAubioBeat.h
		${Cinder-Aubio_INC_PATH}/CinderAubioBlock.h
		${Cinder-Aubio_INC_PATH}/CinderAubioMelBands.h
		${Cinder-Aubio_INC_PATH}/CinderAubioOnset.h
		${Cinder-Aubio_INC_PATH}/CinderAubioPitch.h
	)

	set( Cinder-Aubio_SOURCES
		${Cinder-Aubio_SOURCE_PATH}/CinderAubioBeat.cpp
		${Cinder-Aubio_SOURCE_PATH}/CinderAubioBlock.cpp
		${Cinder-Aubio_SOURCE_PATH}/CinderAubioMelBands.cpp
		${Cinder-Aubio_SOURCE_PATH}/CinderAubioOnset.cpp
		${Cinder-Aubio_SOURCE_PATH}/CinderAubioPitch.cpp
	)

	list( APPEND Cinder-Aubio_LIBRARIES
		${Aubio_SOURCES}
		${Cinder-Aubio_INCLUDES}
		${Cinder-Aubio_SOURCES}
	)

	source_group( aubio FILES ${Aubio_SOURCES} )
	source_group( "deps\\aubio\\src" FILES ${Aubio_SOURCES} )
	source_group( "src" FILES ${Cinder-Aubio_SOURCES} )
	source_group( "include" FILES ${Cinder-Aubio_INCLUDES} )

	add_library( Cinder-Aubio ${Cinder-Aubio_LIBRARIES} )
	target_include_directories( Cinder-Aubio PUBLIC "${CINDER_PATH}/include" ${Aubio_SOURCE_PATH} ${Cinder-Aubio_INC_PATH} )
	target_compile_definitions( Cinder-Aubio PUBLIC 
		_USE_MATH_DEFINES=1
		HAVE_C99_VARARGS_MACROS=1
		NULL=0
		HAVE_STDLIB_H=1
		HAVE_STDIO_H=1
		HAVE_MATH_H=1
		HAVE_STDARG_H=1
		HAVE_LIMITS_H=1
		HAVE_STRING_H=1
		HAVE_ERRNO_H=1
	)
endif()
