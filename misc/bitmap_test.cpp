/**
 * Sample program to manually test bitmaps using the internal bitmap parser.
 */

#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <cstdio>

#include "src/bw_bitmap.h"

// TODO: error handling (e.g. file does not exist)
const unsigned char *read_file(const char *path) {
  FILE *fp;
  long file_size;
  char unsigned *buffer;

  fp = fopen(path, "rb");
  if (!fp) exit(1);

  fseek(fp, 0L, SEEK_END);
  file_size = ftell(fp);
  rewind(fp);

  buffer = (unsigned char *)malloc(file_size + 1);
  fread(buffer, file_size, 1, fp);
  fclose(fp);

  return buffer;
}

void info(BWBitmap *bmp) {
  printf("image_header:\n");
  printf("  .header_size           = %d\n", bmp->get_image_header().header_size);
  printf("  .image_width           = %d\n", bmp->get_image_header().image_width);
  printf("  .image_height          = %d\n", bmp->get_image_header().image_height);
  printf("  .color_planes          = %d\n", bmp->get_image_header().color_planes);
  printf("  .bpp                   = %d\n", bmp->get_image_header().bpp);
  printf("  .compression_method    = %d\n", bmp->get_image_header().compression_method);
  printf("  .image_size            = %d\n", bmp->get_image_header().image_size);
  printf("  .horizontal_resolution = %d\n", bmp->get_image_header().horizontal_resolution);
  printf("  .vertical_resolution   = %d\n", bmp->get_image_header().vertical_resolution);
  printf("  .colors_in_palette     = %d\n", bmp->get_image_header().colors_in_palette);
  printf("  .important_colors      = %d\n", bmp->get_image_header().important_colors);
  printf("image_header_ext_v4:\n");
  printf("  .red_mask         = #%X\n", bmp->get_image_header_extended().red_mask);
  printf("  .green_mask       = #%X\n", bmp->get_image_header_extended().green_mask);
  printf("  .blue_mask        = #%X\n", bmp->get_image_header_extended().blue_mask);
  printf("  .alpha_mask       = #%X\n", bmp->get_image_header_extended().alpha_mask);
  printf("  .color_space_type = %d\n", bmp->get_image_header_extended().color_space_type);
  printf("  .red_x            = 0x%X\n", bmp->get_image_header_extended().red_x);
  printf("  .red_y            = 0x%X\n", bmp->get_image_header_extended().red_y);
  printf("  .red_z            = 0x%X\n", bmp->get_image_header_extended().red_z);
  printf("  .green_x          = 0x%X\n", bmp->get_image_header_extended().green_x);
  printf("  .green_y          = 0x%X\n", bmp->get_image_header_extended().green_y);
  printf("  .green_z          = 0x%X\n", bmp->get_image_header_extended().green_z);
  printf("  .blue_x           = 0x%X\n", bmp->get_image_header_extended().blue_x);
  printf("  .blue_y           = 0x%X\n", bmp->get_image_header_extended().blue_y);
  printf("  .blue_z           = 0x%X\n", bmp->get_image_header_extended().blue_z);
  printf("  .gamma_red        = 0x%X\n", bmp->get_image_header_extended().gamma_red);
  printf("  .gamma_green      = 0x%X\n", bmp->get_image_header_extended().gamma_green);
  printf("  .gamma_blue       = 0x%X\n", bmp->get_image_header_extended().gamma_blue);
  printf(".color_palette:\n");
  for(uint32_t p = 0; p < bmp->get_image_header().colors_in_palette; p++) {
    printf("  0x%02X: #%02X%02X%02X(%02X)\n", p,
        bmp->get_palette()[p].r,
        bmp->get_palette()[p].g,
        bmp->get_palette()[p].b,
        bmp->get_palette()[p].alpha
    );
  }
}

void draw(BWBitmap *bmp) {
  for (uint32_t y = 0; y < bmp->height(); y++) {
    for (uint32_t x = 0; x < bmp->width(); x++) {
      const uint8_t p = bmp->read_pixel(x, y);
      if (p > 0) {
        printf("█");
      } else {
        printf(" ");
      }
    }
    printf("\n");
  }
}

int main(int argc, char **argv) {
  if (argc < 2) {
    fprintf(stderr, "Usage: %s <file.bmp> [<invert>] [<binarisation_mode>]\n", argv[0]);

    fprintf(stderr, "  invert=0 => do not invert pixel values [default]\n");
    fprintf(stderr, "  invert=1 => invert pixel values\n");

    fprintf(stderr, "  binarisation_mode=0 => AUTO [default]\n");
    fprintf(stderr, "  binarisation_mode=1 => SIMPLE\n");
    fprintf(stderr, "  binarisation_mode=2 => ADAPTIVE\n");
    return -1;
  }

  bool invert = false;
  if (argc >= 3) {
    invert = atoi(argv[2]) == 1;
  }

  BWBinarisationMode mode = AUTO;
  if (argc >= 4) {
    switch(atoi(argv[3])) {
      case 0: mode = AUTO; break;
      case 1: mode = SIMPLE; break;
      case 2: mode = ADAPTIVE; break;
      default: mode = AUTO; break;
    }
  }

  const unsigned char *bitmap_raw = read_file(argv[1]);
  BWBitmap bmp = BWBitmap(bitmap_raw, mode);
  if (invert) bmp = bmp.invert();

  info(&bmp);
  draw(&bmp);
}
