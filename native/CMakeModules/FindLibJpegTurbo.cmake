# Try to find the libjpeg-turbo libraries and headers
#
# TURBO_INCLUDE_DIRS
# TURBO_LIBRARIES
# TURBO_FOUND

# Find header files
FIND_PATH(
    TURBO_INCLUDE_DIRS turbojpeg.h
    /opt/libjpeg-turbo/include/
)

FIND_LIBRARY(
    TURBO_LIBRARY
    NAMES libturbojpeg.a
    PATH /opt/libjpeg-turbo/lib64
)

FIND_LIBRARY(
    JPEG_LIBRARY
    NAMES libjpeg.a
    PATH /opt/libjpeg-turbo/lib64
)


IF (TURBO_LIBRARY)
    SET(TURBO_FOUND TRUE)
ENDIF ()

IF (TURBO_FOUND AND TURBO_INCLUDE_DIRS)
    SET(TURBO_FOUND TRUE)
    SET(TURBO_LIBRARIES ${TURBO_LIBRARY} ${JPEG_LIBRARY})
    MESSAGE(STATUS "Found Turbo library: ${TURBO_LIBRARIES}, ${TURBO_INCLUDE_DIRS}")
ELSE (TURBO_FOUND AND TURBO_INCLUDE_DIRS)
    MESSAGE(STATUS "Not found Turbo library")
ENDIF ()