target_sources(corefw 
    PRIVATE array.c bool.c box.c class.c double.c file.c int.c map.c object.c range.c refpool.c stream.c string.c tcpsocket.c 
    PUBLIC FILE_SET HEADERS 
    BASE_DIRS "${CMAKE_SOURCE_DIR}/include"
    FILES "${CMAKE_SOURCE_DIR}/include/corefw/corefw.h" 
			"${CMAKE_SOURCE_DIR}/include/corefw/array.h"
			"${CMAKE_SOURCE_DIR}/include/corefw/bool.h"
			"${CMAKE_SOURCE_DIR}/include/corefw/box.h"
			"${CMAKE_SOURCE_DIR}/include/corefw/class.h"
			"${CMAKE_SOURCE_DIR}/include/corefw/double.h"
			"${CMAKE_SOURCE_DIR}/include/corefw/file.h"
			"${CMAKE_SOURCE_DIR}/include/corefw/hash.h"
			"${CMAKE_SOURCE_DIR}/include/corefw/int.h"
			"${CMAKE_SOURCE_DIR}/include/corefw/map.h"
			"${CMAKE_SOURCE_DIR}/include/corefw/object.h"
			"${CMAKE_SOURCE_DIR}/include/corefw/range.h"
			"${CMAKE_SOURCE_DIR}/include/corefw/refpool.h"
			"${CMAKE_SOURCE_DIR}/include/corefw/stream.h"
			"${CMAKE_SOURCE_DIR}/include/corefw/string.h"
			"${CMAKE_SOURCE_DIR}/include/corefw/tcpsocket.h"
    )