target_sources(corefw 
    PRIVATE CFArray.c CFBitVector.c CFBool.c CFBox.c CFClass.c CFDouble.c CFFile.c CFInt.c CFMap.c CFObject.c CFRange.c CFRefPool.c CFStream.c CFString.c CFTCPSocket.c CFLog.c
    PUBLIC FILE_SET HEADERS 
    BASE_DIRS "${CMAKE_SOURCE_DIR}/include"
    FILES "${CMAKE_SOURCE_DIR}/include/corefw/corefw.h" 
			"${CMAKE_SOURCE_DIR}/include/corefw/CFArray.h"
			"${CMAKE_SOURCE_DIR}/include/corefw/CFBitVector.h"
			"${CMAKE_SOURCE_DIR}/include/corefw/CFBool.h"
			"${CMAKE_SOURCE_DIR}/include/corefw/CFBox.h"
			"${CMAKE_SOURCE_DIR}/include/corefw/CFClass.h"
			"${CMAKE_SOURCE_DIR}/include/corefw/CFDouble.h"
			"${CMAKE_SOURCE_DIR}/include/corefw/CFFile.h"
			"${CMAKE_SOURCE_DIR}/include/corefw/CFHash.h"
			"${CMAKE_SOURCE_DIR}/include/corefw/CFInt.h"
			"${CMAKE_SOURCE_DIR}/include/corefw/CFMap.h"
			"${CMAKE_SOURCE_DIR}/include/corefw/CFObject.h"
			"${CMAKE_SOURCE_DIR}/include/corefw/CFRange.h"
			"${CMAKE_SOURCE_DIR}/include/corefw/CFRefPool.h"
			"${CMAKE_SOURCE_DIR}/include/corefw/CFStream.h"
			"${CMAKE_SOURCE_DIR}/include/corefw/CFString.h"
			"${CMAKE_SOURCE_DIR}/include/corefw/CFTCPSocket.h"
			"${CMAKE_SOURCE_DIR}/include/corefw/CFLog.h"
    )