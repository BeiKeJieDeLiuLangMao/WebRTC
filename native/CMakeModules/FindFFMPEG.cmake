# Try to find the ffmpeg libraries and headers for avcodec avformat swscale
#
# FFMPEG_INCLUDE_DIRS
# FFMPEG_LIBRARIES
# FFMPEG_FOUND

# Find header files
FIND_PATH(
        AVCODEC_INCLUDE_DIR libavcodec/avcodec.h
        /usr/include /usr/local/include /opt/local/include
)
FIND_PATH(
        AVFORMAT_INCLUDE_DIR libavformat/avformat.h
        /usr/include /usr/local/include /opt/local/include
)
FIND_PATH(
        AVDEVICE_INCLUDE_DIR libavdevice/avdevice.h
        /usr/include /usr/local/include /opt/local/include
)
FIND_PATH(
        AVUTIL_INCLUDE_DIR libavutil/avutil.h
        /usr/include /usr/local/include /opt/local/include
)
FIND_PATH(
        SWSCALE_INCLUDE_DIR libswscale/swscale.h
        /usr/include /usr/local/include /opt/local/include
)

FIND_PATH(
        AVFILTER_INCLUDE_DIR libavfilter/avfilter.h
        /usr/include /usr/local/include /opt/local/include
)

FIND_PATH(
        SWRESAMPLE_INCLUDE_DIR libswresample/swresample.h
        /usr/include /usr/local/include /opt/local/include
)

# Find Library files
FIND_LIBRARY(
        AVCODEC_LIBRARY
        NAMES avcodec
        PATH /usr/lib /usr/local/lib /opt/local/lib
)
FIND_LIBRARY(
        AVFORMAT_LIBRARY
        NAMES avformat
        PATH /usr/lib /usr/local/lib /opt/local/lib
)
FIND_LIBRARY(
        AVDEVICE_LIBRARY
        NAMES avdevice
        PATH /usr/lib /usr/local/lib /opt/local/lib
)
FIND_LIBRARY(
        AVUTIL_LIBRARY
        NAMES avutil
        PATH /usr/lib /usr/local/lib /opt/local/lib
)
FIND_LIBRARY(
        SWSCALE_LIBRARY
        NAMES swscale
        PATH /usr/lib /usr/local/lib /opt/local/lib
)
FIND_LIBRARY(
        AVFILTER_LIBRARY
        NAMES avfilter
        PATH /usr/lib /usr/local/lib /opt/local/lib
)

FIND_LIBRARY(
        SWRESAMPLE_LIBRARY
        NAMES swresample
        PATH /usr/lib /usr/local/lib /opt/local/lib
)
if (APPLE)
   #Do nothing
   SET(NVENC_INCLUDE_DIR "")
elseif(UNIX)

   FIND_PATH(
           NVENC_INCLUDE_DIR ffnvcodec/nvEncodeAPI.h
           /usr/include /usr/local/include /opt/local/include
   )

   FIND_LIBRARY(
           AAC_LIBRARY
           NAMES fdk-aac
           PATH /usr/lib /usr/local/lib /opt/local/lib
   )

   FIND_LIBRARY(
           MP3_LIBRARY
           NAMES mp3lame
           PATH /usr/lib /usr/local/lib /opt/local/lib
   )

   FIND_LIBRARY(
           X264_LIBRARY
           NAMES x264
           PATH /usr/lib /usr/local/lib /opt/local/lib
   )

   FIND_LIBRARY(
           OPUS_LIBRARY
           NAMES opus
           PATH /usr/lib /usr/local/lib /opt/local/lib
   )

   FIND_LIBRARY(
           VPX_LIBRARY
           NAMES vpx
           PATH /usr/lib /usr/local/lib /opt/local/lib
   )

   FIND_LIBRARY(
           VORBIS_LIBRARY
           NAMES vorbis
           PATH /usr/lib /usr/local/lib /opt/local/lib
   )

   FIND_LIBRARY(
           VORBISENC_LIBRARY
           NAMES vorbisenc
           PATH /usr/lib /usr/local/lib /opt/local/lib
   )

   FIND_LIBRARY(
           VORBISFILE_LIBRARY
           NAMES vorbisfile
           PATH /usr/lib /usr/local/lib /opt/local/lib
   )
endif()


IF (EXISTS "${AVUTIL_INCLUDE_DIR}/libavutil/pixdesc.h")
   SET(AVUTIL_HAVE_PIXDESC TRUE)
endif ()

IF (AVCODEC_INCLUDE_DIR AND AVFORMAT_INCLUDE_DIR AND AVUTIL_INCLUDE_DIR AND AVDEVICE_INCLUDE_DIR AND SWSCALE_INCLUDE_DIR AND AVFILTER_INCLUDE_DIR AND SWRESAMPLE_INCLUDE_DIR AND AVCODEC_LIBRARY AND AVFORMAT_LIBRARY AND AVUTIL_LIBRARY AND SWSCALE_LIBRARY AND AVDEVICE_LIBRARY AND AVFILTER_LIBRARY AND SWRESAMPLE_LIBRARY AND AVUTIL_HAVE_PIXDESC)
   SET(FFMPEG_FOUND TRUE)
   SET(FFMPEG_LIBRARIES ${AVCODEC_LIBRARY} ${AVFORMAT_LIBRARY} ${AVUTIL_LIBRARY} ${SWSCALE_LIBRARY} ${AVDEVICE_LIBRARY} ${AVFILTER_LIBRARY} ${SWRESAMPLE_LIBRARY} ${AAC_LIBRARY} ${MP3_LIBRARY} ${X264_LIBRARY} ${VPX_LIBRARY} ${OPUS_LIBRARY} ${VORBIS_LIBRARY} ${VORBISENC_LIBRARY} ${VORBISFILE_LIBRARY})
   SET(FFMPEG_INCLUDE_DIRS ${AVCODEC_INCLUDE_DIR} ${AVFORMAT_INCLUDE_DIR} ${AVUTIL_INCLUDE_DIR} ${SWSCALE_INCLUDE_DIR} ${AVDEVICE_INCLUDE_DIR} ${AVFILTER_INCLUDE_DIR} ${SWRESAMPLE_INCLUDE_DIR} ${NVENC_INCLUDE_DIR})

   include(CheckCXXSourceCompiles)

   SET(CMAKE_REQUIRED_INCLUDES ${FFMPEG_INCLUDE_DIRS})

   CHECK_CXX_SOURCE_COMPILES(
           "#include \"${AVCODEC_INCLUDE_DIR}/libavformat/avformat.h\"
      int main() {
        sizeof(AVFormatContext::max_analyze_duration);
      }" HAVE_FFMPEG_MAX_ANALYZE_DURATION
   )
   CHECK_CXX_SOURCE_COMPILES(
           "#include \"${AVCODEC_INCLUDE_DIR}/libavformat/avformat.h\"
      int main() {
        &avformat_alloc_output_context2;
      }" HAVE_FFMPEG_AVFORMAT_ALLOC_OUTPUT_CONTEXT2
   )
   CHECK_CXX_SOURCE_COMPILES(
           "#include \"${AVCODEC_INCLUDE_DIR}/libavutil/pixdesc.h\"
      int main() {
        AVPixelFormat test = AV_PIX_FMT_GRAY8;
      }" HAVE_FFMPEG_AVPIXELFORMAT
   )
ENDIF ()

IF (FFMPEG_FOUND)
   IF (NOT FFMPEG_FIND_QUIETLY)
      MESSAGE(STATUS "Found FFMPEG: ${FFMPEG_LIBRARIES}, ${FFMPEG_INCLUDE_DIRS}")
   ENDIF (NOT FFMPEG_FIND_QUIETLY)
ELSE (FFMPEG_FOUND)
   IF (FFMPEG_FIND_REQUIRED)
      MESSAGE(FATAL_ERROR "Could not find FFMPEG")
   ENDIF (FFMPEG_FIND_REQUIRED)
ENDIF (FFMPEG_FOUND)