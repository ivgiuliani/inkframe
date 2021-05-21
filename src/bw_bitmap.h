#ifndef __BITMAP_H
#define __BITMAP_H

#include "errors.h"

#ifdef ESP32
#  include <Arduino.h>
#endif

struct bmp_pixel_rgba_t {
  // For non-rgb(a) images (e.g. monochrome or paletted bitmaps), we store
  // the intensity of the color in the alpha channel.
  uint8_t alpha;

  uint8_t r;
  uint8_t g;
  uint8_t b;
};

typedef uint8_t bmp_pixel_bw_t;

struct __attribute__((__packed__)) bmp_file_header_t {
  char signature[2]; // Must always be 'BM'
  uint32_t file_size;
  uint16_t reserved[2];
  uint32_t image_offset;
};

struct __attribute__((__packed__)) bmp_image_header_t {
  uint32_t header_size;
  int32_t image_width;
  int32_t image_height;
  uint16_t color_planes;
  uint16_t bpp;
  uint32_t compression_method;
  uint32_t image_size;
  int32_t horizontal_resolution;
  int32_t vertical_resolution;
  uint32_t colors_in_palette;
  uint32_t important_colors;
};

// For bmp_image_header_t.header_size == 108
struct __attribute__((__packed__)) bmp_image_header_ext_v4_t {
  uint32_t red_mask;
  uint32_t green_mask;
  uint32_t blue_mask;
  uint32_t alpha_mask;
  uint32_t color_space_type;
  int32_t  red_x;
  int32_t  red_y;
  int32_t  red_z;
  int32_t  green_x;
  int32_t  green_y;
  int32_t  green_z;
  int32_t  blue_x;
  int32_t  blue_y;
  int32_t  blue_z;
  uint32_t gamma_red;
  uint32_t gamma_green;
  uint32_t gamma_blue;
};

enum BWBinarisationMode {
  // Pick the best binarisation mode depending on the image config.
  AUTO = 0,

  // Simplest B&W conversion: pixels closer to white will be converted to white,
  // the others to black.
  SIMPLE = 1,

  // Uses single-pass adaptive thresholding to calculate the binarisation value.
  ADAPTIVE = 2,
};

/**
 * Simple bitmap handler class for bitmaps to be displayed on the e-ink screen.
 * 
 * Currently only 8, 24 and 32 bit bitmaps are supported.
 * For 24 and 32 bit bitmaps, an adaptive threshold binarisation process is
 * applied to automatically convert the image to black and white without too
 * much loss of quality.
 */
class BWBitmap {
public:
  BWBitmap(const unsigned char *bitmap, const BWBinarisationMode mode = AUTO) :
    bitmap(bitmap)
  {
    binarisation_mode = mode;
    parse_headers();

    if (mode == AUTO) {
      if (image_header.bpp >= 24) {
        binarisation_mode = ADAPTIVE;
      } else {
        binarisation_mode = SIMPLE;
      }
    }
  }

  ~BWBitmap() {
    this->bitmap = NULL;
  }

  bmp_pixel_bw_t read_pixel(const uint32_t x, const uint32_t y) {
    if (x > width()) PANIC("Requested X coordinate exceeds image boundaries");
    if (y > height()) PANIC("Requested Y coordinate exceeds image boundaries");

    // As this can be an expensive operation, only do it when we read the first
    // pixel and then cache the value.
    switch(binarisation_mode) {
      case ADAPTIVE:
        binarisation_threshold = calculate_binarisation_threshold();
        break;
      case AUTO:
        // We should never have to deal with 'AUTO' here, but in that case just
        // make it a warning and do the simplest thing
        WARN("Auto binarisation mode selected. This shouldn't happen.")
      case SIMPLE:
        binarisation_threshold = 255 / 2;
        break;
    }

    if (rgba_to_grayscale(raw_pixel(x, y)) >= binarisation_threshold) {
      // Foreground
      if (invert_pixel_value) return 255;
      return 0;
    } else {
      // Background
      if (invert_pixel_value) return 0;
      return 255;
    }
  }

  const inline uint32_t width() {
    return image_header.image_width;
  }

  const inline uint32_t height() {
    return image_header.image_height < 0 ?
            -image_header.image_height :
             image_header.image_height;
  }

  /**
   * Invert the bitmap's blacks and whites
   */
  const BWBitmap invert() const {
    BWBitmap copy = *this;
    copy.invert_pixel_value = !invert_pixel_value;
    return copy;
  }

  const bmp_image_header_t get_image_header() {
    return image_header;
  }

  const bmp_image_header_ext_v4_t get_image_header_extended() {
    return image_header_ext_v4;
  }

private:
  const unsigned char *bitmap;
  BWBinarisationMode binarisation_mode;
  bool invert_pixel_value;

  bmp_file_header_t file_header;
  bmp_image_header_t image_header;
  bmp_image_header_ext_v4_t image_header_ext_v4;

  int16_t binarisation_threshold = -1;

  #ifndef __bmp_pixel_size
  #  define __bmp_pixel_size() ((image_header.bpp / 8))
  #endif

  #ifndef __bmp_scan_line_size
  #  define __bmp_scan_line_size() \
    ( \
      (image_header.image_width * __bmp_pixel_size() + 3) & (~3) \
    )
  #endif

  void parse_headers() {
    // We ignore the color pallet block as we only support 8, 24 and 32 bits BMPs.
    memcpy(&file_header, bitmap, sizeof(struct bmp_file_header_t));
    memcpy(&image_header, bitmap + sizeof(struct bmp_file_header_t), sizeof(struct bmp_image_header_t));

    if (image_header.header_size == 108 || image_header.header_size == 124) {
      // This header is only read when the reported header size is 108 bytes
      // There's also another extended variant with 124 bytes which includes
      // ICC color profiles but as we don't support that we can skip the parsing
      // of the extra bytes.
      // https://en.wikipedia.org/wiki/BMP_file_format#DIB_header_(bitmap_information_header)
      memcpy(&image_header_ext_v4, bitmap + sizeof(struct bmp_file_header_t) + image_header.header_size,
      sizeof(struct bmp_image_header_ext_v4_t));
    }
  }

  uint8_t calculate_binarisation_threshold(const uint8_t adaptive_passes = 1) {
    // This method uses a single-pass adaptive thresholding to calculate the
    // binarisation threshold (pixels with luminance < this threshold will be
    // black): https://en.wikipedia.org/wiki/Thresholding_(image_processing)#Automatic_thresholding
    const uint16_t pixel_count = width() * height();
    uint32_t running_luminance_sum = 0;
    uint16_t foreground_pixels = 0;
    uint32_t foreground_luminance_sum = 0;
    uint16_t background_pixels = 0;
    uint32_t background_luminance_sum = 0;
    const unsigned char *scan_line;

    // First pass: calculate the average luminance
    for (uint16_t y = 0; y < height(); y++) {
      scan_line = scan_line_offset(y);
      unsigned char *pixel_start = const_cast<unsigned char *>(scan_line);

      for(uint16_t x = 0; x < width(); x++) {
        running_luminance_sum += rgba_to_grayscale(unpack_rgba_pixel(pixel_start));
        pixel_start += __bmp_pixel_size();
      }
    }
    uint8_t average_luminance = running_luminance_sum / pixel_count;

    // Second+ pass: calculate two averages: one for background and one for
    // foreground pixels
    for (uint8_t i = 0; i < adaptive_passes; i++) {
      for (uint16_t y = 0; y < height(); y++) {
        scan_line = scan_line_offset(y);
        unsigned char *pixel_start = const_cast<unsigned char *>(scan_line);

        for(uint16_t x = 0; x < width(); x++) {
          bmp_pixel_bw_t luminance = rgba_to_grayscale(unpack_rgba_pixel(pixel_start));
          if (luminance < average_luminance) {
            background_pixels++;
            background_luminance_sum += luminance;
          } else {
            foreground_pixels++;
            foreground_luminance_sum += luminance;
          }
          pixel_start += __bmp_pixel_size();
        }
      }
      const uint16_t foreground_luminance = foreground_luminance_sum / foreground_pixels;
      const uint16_t background_luminance = background_luminance_sum / background_pixels;

      average_luminance = (foreground_luminance + background_luminance) / 2;
    }

    return average_luminance;
  }

  bmp_pixel_bw_t rgba_to_grayscale(const struct bmp_pixel_rgba_t pixel) {
    if (image_header.bpp < 24) {
      // For non-rgb(a) bitmaps, we store the luminance value in the alpha
      // channel. In that case we just return that value as is.
      return pixel.alpha;
    }

    // https://en.wikipedia.org/wiki/Grayscale#Luma_coding_in_video_systems
    uint8_t r = pixel.r;
    uint8_t g = pixel.g;
    uint8_t b = pixel.b;

    if (pixel.alpha > 128) {
      // If the pixel is transparent enough, consider it invisible and therefore
      // white for all intents and purposes.
      r = 255;
      g = 255;
      b = 255;
    }

    const int16_t luminance = 0.2627 * r +
                              0.6780 * g +
                              0.0593 * b;

    if (luminance > 255) {
      // Account for floating point rounding errors
      return 255;
    }

    return luminance;
  }

  struct bmp_pixel_rgba_t raw_pixel(uint32_t x, uint32_t y) {
    const unsigned char *scan_line = scan_line_offset(y);
    const unsigned char *pixel_start = scan_line + x * __bmp_pixel_size();

    struct bmp_pixel_rgba_t pixel = unpack_rgba_pixel(pixel_start);

    return pixel;
  }

  inline const unsigned char *scan_line_offset(const uint16_t y_index) {
    // Bitmaps start from the bottom left corner so the last row in the file is
    // actually the top row visually. I.e. usually the file follows this layout:
    //
    // p13 p14 p15 p16
    // p9  p10 p11 p12
    // p5  p6  p7  p8
    // p1  p2  p3  p4
    //
    // However if the height is negative, then rows start top to bottom too:
    //
    // p1  p2  p3  p4
    // p5  p6  p7  p8
    // p9  p10 p11 p12
    // p13 p14 p15 p16
    const bool flip_scan_lines = image_header.image_height < 0;
    const unsigned char *scan_line;

    if (!flip_scan_lines) {
      scan_line = (bitmap + file_header.image_offset) + (height() - 1 - y_index) * __bmp_scan_line_size();
    } else {
      scan_line = (bitmap + file_header.image_offset) + (__bmp_scan_line_size() * y_index);
    }

    return scan_line;
  }

  const struct bmp_pixel_rgba_t unpack_rgba_pixel(const unsigned char *offset) {
    struct bmp_pixel_rgba_t pixel;

    // BMP uses little endian, thus bytes are stored as (A)RGB rather than RGB(A)
    if (image_header.bpp == 8) {
      // For grayscale images (or non-rgb(a) images, more generally) we store the
      // luminance/intensity value in the alpha channel.
      pixel.alpha = *(offset);
    } else if (image_header.bpp == 24) {
      pixel.alpha = 0;
      pixel.g = *(offset + 0);
      pixel.b = *(offset + 1);
      pixel.r = *(offset + 2);
    } else if (image_header.bpp == 32) {
      // 32bit bitmaps include the alpha channel
      pixel.alpha = *(offset + 0);
      pixel.g = *(offset + 1);
      pixel.b = *(offset + 2);
      pixel.r = *(offset + 3);
    } else {
      PANIC("Unsupported bitmap depth format");
    }

    return pixel;
  }
};

#endif // __BITMAP_H
