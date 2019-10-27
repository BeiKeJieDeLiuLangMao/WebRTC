# Try to find the libjpeg-turbo libraries and headers
#
# LIBYUV_INCLUDE_DIRS
# LIBYUV_LIBRARIES
# LIBYUV_FOUND

# Find header files
FIND_PATH(LIBYUV_INCLUDE_DIRS libyuv.h /usr/local/include/)


FIND_LIBRARY(LIBYUV_LIBRARIE NAMES libyuv_internal.a PATH /usr/local/lib)

IF (LIBYUV_INCLUDE_DIRS)
    SET(LIBYUV_FOUND TRUE)
    SET(LIBYUV_LIBRARIES ${LIBYUV_LIBRARIE})
    MESSAGE(STATUS "Found libyuv library: ${LIBYUV_LIBRARIES}, ${LIBYUV_INCLUDE_DIRS}")
ELSE (LIBYUV_INCLUDE_DIRS)
    MESSAGE(STATUS "Not found libyuv library")
ENDIF ()