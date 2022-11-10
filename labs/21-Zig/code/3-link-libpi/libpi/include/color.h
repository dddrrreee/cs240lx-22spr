/**
 * Utilities for working with RGB(A) colors.  Pretty minimal, but the
 * HSV-to-RGB conversion is useful if you want to do anything involving dynamic
 * colors (Hue-Saturation-Value is a more useful parameterization than
 * Red-Green-Blue for pretty much everything except displaying).
 */
#ifndef COLOR_H
#define COLOR_H

#include <stdint.h>

/**
 * Represents a color in RGBA (red, green, blue, alpha). Values are between 0
 * and 255.  You can ignore A if you don't need it, but GCC seems to have
 * issues with arrays of unaligned structs so it helps with that anyway.
 */
struct color {
  uint8_t r:8;
  uint8_t g:8;
  uint8_t b:8;
  uint8_t a:8;
};
_Static_assert(sizeof(struct color) == 4);

typedef struct color color_t;

/**
 * Creates a color_t from red, green, blue, and alpha channels.
 *
 * @param r   The red channel (0-255).
 * @param g   The green channel (0-255).
 * @param b   The blue channel (0-255).
 * @param a   The alpha channel (0-255).
 *
 * @return    A color_t with the specified color.
 */
static inline color_t color_rgba(uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
  return (color_t) {.r = r, .g = g, .b = b, .a=a};
}

/**
 * Creates a color_t from red, green, and blue channels.
 *
 * @param r   The red channel (0-255).
 * @param g   The green channel (0-255).
 * @param b   The blue channel (0-255).
 *
 * @return    A fully opaque (a=0) color_t with the specified color.
 */
static inline color_t color_rgb(uint8_t r, uint8_t g, uint8_t b) {
  return color_rgba(r, g, b, 0xff);
}

/**
 * Creates a color_t from hue, saturation, value, and alpha parameters.
 * Internally does fixed-point arithmetic so floating-point support is
 * unnecessary.
 *
 * See https://en.wikipedia.org/wiki/HSL_and_HSV#HSV_to_RGB for info on the
 * algorithm.  A C++ floating-point implementation is at
 * https://www.codespeedy.com/hsv-to-rgb-in-cpp/.
 *
 * @param h   The hue of the color (0-360).
 * @param s   The saturation of the color (0-100).
 * @param v   The value (brightness) of the color (0-100).
 * @param a   The alpha (opacity) of the color (0-100).
 *
 * @return    A color_t with the specified color converted to RGBA.
 */
static inline color_t color_hsva(int h, uint8_t s, uint8_t v, uint8_t a) {
  h = h % 360;
  if (h < 0) {
    h = 360 + h;
  }
  if (s > 100) s = 100;
  if (v > 100) v = 100;

  int segment = h / 60;

  int C = (v * s); // 0-10000

  int temp = __builtin_abs(h % 120 - 60);

  int X = C*(60 - temp)/60; // 0-10000
  int m = v*100 - C; // 0-10000

  int r = 0, g = 0, b = 0;
  switch (segment) {
    case 0:
      r = C;
      g = X;
      b = 0;
      break;
    case 1:
      r = X;
      g = C;
      b = 0;
      break;
    case 2:
      r = 0;
      g = C;
      b = X;
      break;
    case 3:
      r = 0;
      g = X;
      b = C;
      break;
    case 4:
      r = X;
      g = 0;
      b = C;
      break;
    case 5:
      r = C;
      g = 0;
      b = X;
      break;
  };
  return (color_t) {
    .r = (r + m)*255 / 10000,
    .g = (g + m)*255 / 10000,
    .b = (b + m)*255 / 10000,
    .a = a*255 / 100,
  };
}

/**
 * Creates a color_t from hue, saturation, and value parameters.
 *
 * @param h   The hue of the color (0-360).
 * @param s   The saturation of the color (0-100).
 * @param v   The value (brightness) of the color (0-100).
 *
 * @return    A color_t with the specified color converted to RGB.
 */
static inline color_t color_hsv(int h, uint8_t s, uint8_t v) {
  return color_hsva(h, s, v, 100);
}

/**
 * Compares two colors for equality.
 *
 * @param a   The first color to compare.
 * @param b   The second color to compare.
 *
 * @return    Truthy (!= 0) if the colors are equal, falsey (== 0) if the
 * colors are unequal.
 */
static inline int color_eq(color_t a, color_t b) {
  return a.r == b.r && a.g == b.g && a.b == b.b && a.a == b.a;
}

/**
 * Computes the "distance" between two colors.
 *
 * @param a   The first color to compare.
 * @param b   The second color to compare.
 *
 * @return    A "distance" between two colors, which isn't inherently
 * meaningful but can be compared to other distances.
 */
static inline unsigned color_distance(color_t a, color_t b) {
  const int R = a.r - b.r;
  const int G = a.g - b.g;
  const int B = a.b - b.b;
  const int A = a.a - b.a;
  return R*R + G*G + B*B + A*A;
}

#define COLOR_RGB(R, G, B) ((color_t) {.r=R, .g=G, .b=B})
#define COLOR_BLACK   (COLOR_RGB(0x00, 0x00, 0x00))
#define COLOR_RED     (COLOR_RGB(0xff, 0x00, 0x00))
#define COLOR_GREEN   (COLOR_RGB(0x00, 0xff, 0x00))
#define COLOR_YELLOW  (COLOR_RGB(0xff, 0xff, 0x00))
#define COLOR_BLUE    (COLOR_RGB(0x00, 0x00, 0xff))
#define COLOR_MAGENTA (COLOR_RGB(0xff, 0x00, 0xff))
#define COLOR_CYAN    (COLOR_RGB(0x00, 0xff, 0xff))
#define COLOR_WHITE   (COLOR_RGB(0xff, 0xff, 0xff))

#endif
