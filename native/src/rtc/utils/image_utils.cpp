//
// Created by 贝克街的流浪猫 on 2019-06-12.
//

#include "image_utils.h"

int getScaled(int length, int num, int denom) {
  return (length * num + denom - 1) / denom;
}

JNIEXPORT jint JNICALL Java_bbm_webrtc_rtc4j_core_ImageUtils_scaleImage
        (JNIEnv *env, jclass, jobject source, jobject tmp_buffer, jobject target, jint numerator, jint denominator,
         jint quality) {
  tjhandle handle = nullptr;
  int width, height, subsample, colorspace;
  int flags = 0;
  int pixel_fmt = TJPF_RGB;
  const auto *jpeg_buffer = (const unsigned char *) env->GetDirectBufferAddress(source);
  auto jpeg_size = (unsigned long) env->GetDirectBufferCapacity(source);
  auto *rgb_buffer = (unsigned char *) env->GetDirectBufferAddress(tmp_buffer);
  auto *target_buffer = (unsigned char *) env->GetDirectBufferAddress(target);
  unsigned long target_jpeg_size;
  handle = tjInitDecompress();
  tjDecompressHeader3(handle, jpeg_buffer, jpeg_size, &width, &height, &subsample, &colorspace);
  flags |= TJFLAG_ACCURATEDCT;
  flags |= TJFLAG_FASTDCT;
  flags |= TJFLAG_FASTUPSAMPLE;
  tjDecompress2(handle, jpeg_buffer, jpeg_size, rgb_buffer, getScaled(width, numerator, denominator), 0,
                getScaled(height, numerator, denominator), pixel_fmt, flags);
  tjDestroy(handle);
  handle = tjInitCompress();
  tjCompress2(handle, rgb_buffer, getScaled(width, numerator, denominator), 0,
              getScaled(height, numerator, denominator), pixel_fmt, &target_buffer, &target_jpeg_size, TJSAMP_422,
              quality, flags);
  tjDestroy(handle);
  return static_cast<jint>(target_jpeg_size);
}