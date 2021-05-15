#ifndef __BITMAP_H
#define __BITMAP_H

#ifdef ESP32
#  include <Arduino.h>
#endif

struct bmp_pixel {
  uint8_t alpha;
  uint8_t r;
  uint8_t g;
  uint8_t b;
};

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

class Bitmap {
public:
  Bitmap(const unsigned char *bitmap) {
    this->bitmap = bitmap;
    parse_headers();
  }

  ~Bitmap() {
    this->bitmap = NULL;
  }

  uint8_t read_pixel(uint32_t x, uint32_t y) {
    if (x > width()) throw "Requested X coordinate exceeds boundaries";
    if (y > height()) throw "Requested Y coordinate exceeds boundaries";

    struct bmp_pixel pixel = raw_pixel(x, y);

    // TODO: improve B&W threshold calculation
    if ((pixel.alpha> 0) || ((pixel.r + pixel.g + pixel.b) > 100)) {
      return 1;
    }

    return 0;
  }

  const inline uint32_t width() {
    return image_header.image_width;
  }

  const inline uint32_t height() {
    return image_header.image_height < 0 ?
            -image_header.image_height :
             image_header.image_height;
  }

private:
  const unsigned char *bitmap;

  bmp_file_header_t file_header;
  bmp_image_header_t image_header;

  void parse_headers() {
    // We ignore the color pallet block as we only support 24 and 32 bits BMPs.
    memcpy(&file_header, bitmap, sizeof(struct bmp_file_header_t));
    memcpy(&image_header, bitmap + sizeof(struct bmp_file_header_t), sizeof(struct bmp_image_header_t));
  }

  struct bmp_pixel raw_pixel(uint32_t x, uint32_t y) {
    struct bmp_pixel pixel;

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
    const uint8_t pixel_size = image_header.bpp / 8;
    const unsigned char *scan_line;

    // Rows are always padded to 4 bytes
    const uint32_t scan_line_size = (image_header.image_width * pixel_size + 3) & (~3);

    if (!flip_scan_lines) {
      scan_line = (bitmap + file_header.image_offset) + (height() - 1 - y) * scan_line_size;
    } else {
      scan_line = (bitmap + file_header.image_offset) + (scan_line_size * y);
    }

    const unsigned char *pixel_start = scan_line + x * pixel_size;

    // BMP uses little endian, thus bytes are stored as (A)RGB rather than RGB(A)
    if (image_header.bpp < 32) {
      pixel.alpha = 0;
      pixel.g = *(pixel_start + 0);
      pixel.b = *(pixel_start + 1);
      pixel.r = *(pixel_start + 2);
    } else {
      // 32bit bitmaps include the alpha channel
      pixel.alpha = *(pixel_start + 0);
      pixel.g = *(pixel_start + 1);
      pixel.b = *(pixel_start + 2);
      pixel.r = *(pixel_start + 3);
    }

    return pixel;
  }
};

#endif // __BITMAP_H
