/// imageRGB - A simple image module for handling RGB images,
///            pixel color values are represented using a look-up table (LUT)
///
/// This module is part of a programming project
/// for the course AED, DETI / UA.PT
///
/// You may freely use and modify this code, at your own risk,
/// as long as you give proper credit to the original and subsequent authors.
///
/// The AED Team <jmadeira@ua.pt, jmr@ua.pt, ...>
/// 2025

// Student authors (fill in below):
// NMec:
// Name:
// NMec:
// Name:
//
// Date:
//

#include "imageRGB.h"

#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "PixelCoords.h"
#include "PixelCoordsQueue.h"
#include "PixelCoordsStack.h"
#include "instrumentation.h"

// The data structure
//
// A RGB image is stored in a structure containing 5 fields:
// Two integers store the image width and height.
// The next field is a pointer to an array that stores the pointers
// to the image rows.
//
// Clients should use images only through variables of type Image,
// which are pointers to the image structure, and should not access the
// structure fields directly.

// FIXED SIZE of LUT for storing RGB triplets
#define FIXED_LUT_SIZE 1000

// Internal structure for storing RGB images
struct image {
  uint32 width;
  uint32 height;
  uint16** image;  // pointer to an array of pointers referencing the image rows
  uint16 num_colors;  // the number of colors (i.e., pixel labels) used
  rgb_t* LUT;         // table storing (R,G,B) triplets
};

// Design by Contract

// This module follows "design-by-contract" principles.
// `assert` is used to check function preconditions, postconditions
// and type invariants.
// This helps to find programmer errors.

/// Defensive Error Handling

// In this module, only functions dealing with memory allocation or file
// (I/O) operations use defensive techniques.
//
// When one of these functions detects a memory or I/O error,
// it immediately prints an error message and aborts the program.
// This is a Fail-Fast strategy.
//
// You may use the `check` function to check a condition
// and exit the program with an error message if it is false.
// Note that it works similarly to `assert`, but cannot be disabled.
// It should be used to detect "external" uncontrolable errors,
// and not for "internal" programmer errors.
//
// See how it's used in ImageLoadPBM, for example.

// Check a condition and if false, print failmsg and exit.
static void check(int condition, const char* failmsg) {
  if (!condition) {
    perror(failmsg);
    exit(errno || 255);
  }
}

/// Init Image library.  (Call once!)
/// Currently, simply calibrate instrumentation and set names of counters.
void ImageInit(void) {  ///
  InstrCalibrate();
  InstrName[0] = "pixmem";  // InstrCount[0] will count pixel array acesses
  // Name other counters here...
}

// Macros to simplify accessing instrumentation counters:
#define PIXMEM InstrCount[0]
// Add more macros here...

// TIP: Search for PIXMEM or InstrCount to see where it is incremented!

/// Auxiliary (static) functions

static Image AllocateImageHeader(uint32 width, uint32 height) {
  // Create the header of an image data structure
  // Allocate the array of pointers to rows
  // And the look-up table

  Image newHeader = malloc(sizeof(struct image));
  // Error handling
  check(newHeader != NULL, "malloc");

  newHeader->width = width;
  newHeader->height = height;

  // Allocating the array of pointers to image rows
  newHeader->image = malloc(height * sizeof(uint16*));
  // Error handling
  check(newHeader->image != NULL, "Alloc failed ->image array");

  // Allocating the LUT
  newHeader->LUT = malloc(FIXED_LUT_SIZE * sizeof(rgb_t));
  // Error handling
  check(newHeader->LUT != NULL, "Alloc failed ->LUT array");

  // Initialize LUT with 2 fixed colors
  newHeader->num_colors = 2;
  newHeader->LUT[0] = 0xffffff;  // RGB WHITE
  newHeader->LUT[1] = 0x000000;  // RGB BLACK

  return newHeader;
}

// Allocate row of background (label=0) pixels
static uint16* AllocateRowArray(uint32 size) {
  uint16* newArray = calloc((size_t)size, sizeof(uint16));
  // Error handling
  check(newArray != NULL, "AllocateRowArray");

  return newArray;
}

/// Find color label for given RGB color in img LUT.
/// Return the label or -1 if not found.
static int LUTFindColor(Image img, rgb_t color) {
  for (uint16 index = 0; index < img->num_colors; index++) {
    if (img->LUT[index] == color) return index;
  }
  return -1;
}

/// Return color label for RGB color in img LUT.
/// Finds existing color or allocs new one!
static int LUTAllocColor(Image img, rgb_t color) {
  int index = LUTFindColor(img, color);
  if (index < 0) {
    check(img->num_colors < FIXED_LUT_SIZE, "LUT Overflow");
    index = img->num_colors++;
    img->LUT[index] = color;
  }
  return index;
}

/// Return a pseudo-random successor of the given color.
static rgb_t GenerateNextColor(rgb_t color) {
  return (color + 7639) & 0xffffff;
}

/// Image management functions

/// Create a new RGB image. All pixels with the background WHITE color.
///   width, height: the dimensions of the new image.
/// Requires: width and height must be non-negative.
///
/// On success, a new image is returned.
/// (The caller is responsible for destroying the returned image!)
Image ImageCreate(uint32 width, uint32 height) {
  assert(width > 0);
  assert(height > 0);

  // Just two possible pixel colors
  Image img = AllocateImageHeader(width, height);

  // Creating the image rows
  for (uint32 i = 0; i < height; i++) {
    img->image[i] = AllocateRowArray(width);  // Alloc all WHITE row
  }

  return img;
}

/// Create a new RGB image, with a color chess pattern.
/// The background is WHITE.
///   width, height: the dimensions of the new image.
///   edge: the width and height of a chess square.
///   color: the foreground color.
/// Requires: width, height and edge must be non-negative.
///
/// On success, a new image is returned.
/// (The caller is responsible for destroying the returned image!)
Image ImageCreateChess(uint32 width, uint32 height, uint32 edge, rgb_t color) {
  assert(width > 0);
  assert(height > 0);
  assert(edge > 0);

  Image img = ImageCreate(width, height);

  // Alloc color in LUT.
  uint8 label = LUTAllocColor(img, color);

  // Assigning the color to each image pixel

  // Pixel (0, 0) gets the chosen color label
  for (uint32 i = 0; i < height; i++) {
    uint32 I = i / edge;
    for (uint32 j = 0; j < width; j++) {
      uint32 J = j / edge;
      img->image[i][j] = (I + J) % 2 ? 0 : label;
    }
  }

  // Return the created chess image
  return img;
}

/// Create an image with a palete of generated colors.
Image ImageCreatePalete(uint32 width, uint32 height, uint32 edge) {
  assert(width > 0);
  assert(height > 0);
  assert(edge > 0);

  Image img = ImageCreate(width, height);

  // Fill LUT with generated colors
  rgb_t color = 0x000000;
  while (img->num_colors < FIXED_LUT_SIZE) {
    color = GenerateNextColor(color);
    img->LUT[img->num_colors++] = color;
  }

  // number of tiles
  uint32 wtiles = width / edge;

  // Pixel (0, 0) gets the chosen color label
  for (uint32 i = 0; i < height; i++) {
    uint32 I = i / edge;
    for (uint32 j = 0; j < width; j++) {
      uint32 J = j / edge;
      img->image[i][j] = (I * wtiles + J) % FIXED_LUT_SIZE;
    }
  }

  return img;
}

/// Destroy the image pointed to by (*imgp).
///   imgp : address of an Image variable.
/// If (*imgp)==NULL, no operation is performed.
///
/// Ensures: (*imgp)==NULL.
void ImageDestroy(Image* imgp) {
  assert(imgp != NULL);

  Image img = *imgp;

  for (uint32 i = 0; i < img->height; i++) {
    free(img->image[i]);
  }
  free(img->image);
  free(img->LUT);
  free(img);

  *imgp = NULL;
}

/// Create a deep copy of the image pointed to by img.
///   img : address of an Image variable.
///
/// On success, a new copied image is returned.
/// (The caller is responsible for destroying the returned image!)
Image ImageCopy(const Image img) {
  assert(img != NULL);

  // Cria cabeçalho e estruturas base
  Image copy = AllocateImageHeader(img->width, img->height);

  // Copiar LUT
  copy->num_colors = img->num_colors;
  for (uint16 i = 0; i < copy->num_colors; i++) {
    copy->LUT[i] = img->LUT[i];
  }

  // Copiar pixeis (deep copy)
  for (uint32 v = 0; v < img->height; v++) {
    copy->image[v] = AllocateRowArray(img->width);
    for (uint32 u = 0; u < img->width; u++) {
      copy->image[v][u] = img->image[v][u];
      PIXMEM += 2;  // 1 leitura + 1 escrita
    }
  }

  return copy;
}

/// Printing on the console

/// These functions do not modify the image and never fail.

/// Output the raw RGB image (i.e., print the integer value of pixel).
void ImageRAWPrint(const Image img) {
  printf("width = %d height = %d\n", (int)img->width, (int)img->height);
  printf("num_colors = %d\n", (int)img->num_colors);
  printf("RAW image\n");

  // Print the pixel labels of each image row
  for (uint32 i = 0; i < img->height; i++) {
    for (uint32 j = 0; j < img->width; j++) {
      printf("%2d", img->image[i][j]);
    }
    // At current row end
    printf("\n");
  }

  printf("LUT:\n");
  // Print the LUT (R,G,B) values
  for (int i = 0; i < (int)img->num_colors; i++) {
    rgb_t color = img->LUT[i];
    int r = color >> 16 & 0xff;
    int g = color >> 8 & 0xff;
    int b = color & 0xff;
    printf("%3d -> (%3d,%3d,%3d)\n", i, r, g, b);
  }

  printf("\n");
}

/// PBM file operations --- For BW images

// See PBM format specification: http://netpbm.sourceforge.net/doc/pbm.html

//
static void unpackBits(int nbytes, const uint8 bytes[], uint8 raw_row[]) {
  // bitmask starts at top bit
  int offset = 0;
  uint8 mask = 1 << (7 - offset);
  while (offset < 8) {  // or (mask > 0)
    for (int b = 0; b < nbytes; b++) {
      raw_row[8 * b + offset] = (bytes[b] & mask) != 0;
    }
    mask >>= 1;
    offset++;
  }
}

static void packBits(int nbytes, uint8 bytes[], const uint8 raw_row[]) {
  // bitmask starts at top bit
  int offset = 0;
  uint8 mask = 1 << (7 - offset);
  while (offset < 8) {  // or (mask > 0)
    for (int b = 0; b < nbytes; b++) {
      if (offset == 0) bytes[b] = 0;
      bytes[b] |= raw_row[8 * b + offset] ? mask : 0;
    }
    mask >>= 1;
    offset++;
  }
}

// Match and skip 0 or more comment lines in file f.
// Comments start with a # and continue until the end-of-line, inclusive.
// Returns the number of comments skipped.
static int skipComments(FILE* f) {
  char c;
  int i = 0;
  while (fscanf(f, "#%*[^\n]%c", &c) == 1 && c == '\n') {
    i++;
  }
  return i;
}

/// Load a raw PBM file.
/// Only binary PBM files are accepted.
/// On success, a new image is returned.
/// (The caller is responsible for destroying the returned image!)
Image ImageLoadPBM(const char* filename) {  ///
  int w, h;
  char c;
  FILE* f = NULL;
  Image img = NULL;

  check((f = fopen(filename, "rb")) != NULL, "Open failed");
  // Parse PBM header
  check(fscanf(f, "P%c ", &c) == 1 && c == '4', "Invalid file format");
  skipComments(f);
  check(fscanf(f, "%d ", &w) == 1 && w >= 0, "Invalid width");
  skipComments(f);
  check(fscanf(f, "%d", &h) == 1 && h >= 0, "Invalid height");
  check(fscanf(f, "%c", &c) == 1 && isspace(c), "Whitespace expected");

  // Allocate image
  img = AllocateImageHeader((uint32)w, (uint32)h);

  // Read pixels
  int nbytes = (w + 8 - 1) / 8;  // number of bytes for each row
  // using VLAs...
  uint8 bytes[nbytes];
  uint8 raw_row[nbytes * 8];
  for (uint32 i = 0; i < img->height; i++) {
    check(fread(bytes, sizeof(uint8), nbytes, f) == (size_t)nbytes,
          "Reading pixels");
    unpackBits(nbytes, bytes, raw_row);
    img->image[i] = AllocateRowArray((uint32)w);
    for (uint32 j = 0; j < (uint32)w; j++) {
      img->image[i][j] = (uint16)raw_row[j];
    }
  }

  fclose(f);
  return img;
}

/// Save image to PBM file.
/// On success, returns nonzero.
/// On failure, a partial and invalid file may be left in the system.
int ImageSavePBM(const Image img, const char* filename) {  ///
  assert(img != NULL);
  assert(img->num_colors == 2);

  int w = (int)img->width;
  int h = (int)img->height;
  FILE* f = NULL;

  check((f = fopen(filename, "wb")) != NULL, "Open failed");
  check(fprintf(f, "P4\n%d %d\n", w, h) > 0, "Writing header failed");

  // Write pixels
  int nbytes = (w + 8 - 1) / 8;  // number of bytes for each row
  // using VLAs...
  uint8 bytes[nbytes];
  uint8 raw_row[nbytes * 8];
  for (uint32 i = 0; i < img->height; i++) {
    for (uint32 j = 0; j < img->width; j++) {
      raw_row[j] = (uint8)img->image[i][j];
    }
    // Fill padding pixels with WHITE
    memset(raw_row + w, WHITE, nbytes * 8 - w);
    packBits(nbytes, bytes, raw_row);
    check(fwrite(bytes, sizeof(uint8), nbytes, f) == (size_t)nbytes,
          "Writing pixels failed");
  }

  // Cleanup
  fclose(f);

  return 0;
}

/// PPM file operations --- For RGB images

/// Load a raw PPM file.
/// Only ASCII PPM files are accepted.
/// On success, a new image is returned.
/// (The caller is responsible for destroying the returned image!)
Image ImageLoadPPM(const char* filename) {
  assert(filename != NULL);
  int w, h;
  int levels;
  char c;
  FILE* f = NULL;

  check((f = fopen(filename, "rb")) != NULL, "Open failed");
  // Parse PPM header
  check(fscanf(f, "P%c ", &c) == 1 && c == '3', "Invalid file format");
  skipComments(f);
  check(fscanf(f, "%d ", &w) == 1 && w >= 0, "Invalid width");
  skipComments(f);
  check(fscanf(f, "%d", &h) == 1 && h >= 0, "Invalid height");
  skipComments(f);
  check(fscanf(f, "%d", &levels) == 1 && 0 <= levels && levels <= 255,
        "Invalid depth");
  check(fscanf(f, "%c", &c) == 1 && isspace(c), "Whitespace expected");

  // Allocate image
  Image img = ImageCreate((uint32)w, (uint32)h);

  // Read pixels
  for (uint32 i = 0; i < img->height; i++) {
    for (uint32 j = 0; j < img->width; j++) {
      int r, g, b;
      check(fscanf(f, "%d %d %d", &r, &g, &b) == 3 && 0 <= r && r <= levels &&
                0 <= g && g <= levels && 0 <= b && b <= levels,
            "Invalid pixel color");
      rgb_t color = r << 16 | g << 8 | b;
      uint16 index = LUTAllocColor(img, color);
      img->image[i][j] = index;
      // printf("[%u][%u]: (%d,%d,%d) -> %u (%6x)\n", i, j, r,g,b, index,
      // color);
    }
    fprintf(f, "\n");
  }

  fclose(f);
  return img;
}

/// Save image to PPM file.
/// On success, returns nonzero.
/// On failure, a partial and invalid file may be left in the system.
int ImageSavePPM(const Image img, const char* filename) {
  assert(img != NULL);

  int w = (int)img->width;
  int h = (int)img->height;
  FILE* f = NULL;

  check((f = fopen(filename, "wb")) != NULL, "Open failed");
  check(fprintf(f, "P3\n%d %d\n255\n", w, h) > 0, "Writing header failed");

  // The pixel RGB values
  for (uint32 i = 0; i < img->height; i++) {
    for (uint32 j = 0; j < img->width; j++) {
      uint16 index = img->image[i][j];
      rgb_t color = img->LUT[index];
      int r = color >> 16 & 0xff;
      int g = color >> 8 & 0xff;
      int b = color & 0xff;
      fprintf(f, "  %3d %3d %3d", r, g, b);
    }
    fprintf(f, "\n");
  }

  // Cleanup
  fclose(f);

  return 0;
}

/// Information queries

/// These functions do not modify the image and never fail.

/// Get image width
uint32 ImageWidth(const Image img) {
  assert(img != NULL);
  return img->width;
}

/// Get image height
uint32 ImageHeight(const Image img) {
  assert(img != NULL);
  return img->height;
}

/// Get number of image colors
uint16 ImageColors(const Image img) {
  assert(img != NULL);
  return img->num_colors;
}

/// Image comparison

/// These functions do not modify the images and never fail.

/// Check if img1 and img2 represent equal images.
/// NOTE: The same rgb color may correspond to different LUT labels in
/// different images!
int ImageIsEqual(const Image img1, const Image img2) {
  assert(img1 != NULL);
  assert(img2 != NULL);

  // Se dimensões diferentes, não podem ser iguais
  if (img1->width != img2->width || img1->height != img2->height) {
    return 0;
  }

  // Percorrer todos os pixeis e comparar cores RGB
  for (uint32 v = 0; v < img1->height; v++) {
    for (uint32 u = 0; u < img1->width; u++) {
      uint16 label1 = img1->image[v][u];
      uint16 label2 = img2->image[v][u];
      PIXMEM += 2;  // duas leituras de array de pixeis

      rgb_t color1 = img1->LUT[label1];
      rgb_t color2 = img2->LUT[label2];

      // Comparação de cores reais
      if (color1 != color2) {
        // Melhor caso: diferença logo nos primeiros pixeis
        return 0;
      }
    }
  }

  // Pior caso: todas as comparações feitas e todas iguais
  return 1;
}


/// Geometric transformations

/// These functions apply geometric transformations to an image,
/// returning a new image as a result.
///
/// On success, a new image is returned.
/// (The caller is responsible for destroying the returned image!)

/// Rotate 90 degrees clockwise (CW).
/// Returns a rotated version of the image.
/// Ensures: The original img is not modified.
///
/// On success, a new image is returned.
/// (The caller is responsible for destroying the returned image!)
Image ImageRotate90CW(const Image img) {
  assert(img != NULL);

  // Nova imagem com dimensões trocadas
  Image rotated = AllocateImageHeader(img->height, img->width);

  // Copiar LUT inteira relevante
  rotated->num_colors = img->num_colors;
  for (uint16 i = 0; i < rotated->num_colors; i++) {
    rotated->LUT[i] = img->LUT[i];
  }

  // Alocar linhas
  for (uint32 v = 0; v < rotated->height; v++) {
    rotated->image[v] = AllocateRowArray(rotated->width);
  }

  // Mapeamento:
  // original (r, c) -> novo (c, H-1-r)
  uint32 H = img->height;
  uint32 W = img->width;

  for (uint32 r = 0; r < H; r++) {
    for (uint32 c = 0; c < W; c++) {
      uint16 label = img->image[r][c];
      PIXMEM++;  // leitura

      uint32 new_r = c;
      uint32 new_c = H - 1 - r;

      rotated->image[new_r][new_c] = label;
      PIXMEM++;  // escrita
    }
  }

  return rotated;
}


/// Rotate 180 degrees clockwise (CW).
/// Returns a rotated version of the image.
/// Ensures: The original img is not modified.
///
/// On success, a new image is returned.
/// (The caller is responsible for destroying the returned image!)
Image ImageRotate180CW(const Image img) {
  assert(img != NULL);

  // Mesmas dimensões
  Image rotated = AllocateImageHeader(img->width, img->height);

  // Copiar LUT
  rotated->num_colors = img->num_colors;
  for (uint16 i = 0; i < rotated->num_colors; i++) {
    rotated->LUT[i] = img->LUT[i];
  }

  // Alocar linhas
  for (uint32 v = 0; v < rotated->height; v++) {
    rotated->image[v] = AllocateRowArray(rotated->width);
  }

  uint32 H = img->height;
  uint32 W = img->width;

  // Mapeamento:
  // original (r, c) -> novo (H-1-r, W-1-c)
  for (uint32 r = 0; r < H; r++) {
    for (uint32 c = 0; c < W; c++) {
      uint16 label = img->image[r][c];
      PIXMEM++;  // leitura

      uint32 new_r = H - 1 - r;
      uint32 new_c = W - 1 - c;

      rotated->image[new_r][new_c] = label;
      PIXMEM++;  // escrita
    }
  }

  return rotated;
}


/// Check whether pixel coords (u, v) are inside img.
/// ATTENTION
///   u : column index
///   v : row index
int ImageIsValidPixel(const Image img, int u, int v) {
  return 0 <= u && u < (int)img->width && 0 <= v && v < (int)img->height;
}

/// Region Growing

/// The following three *RegionFilling* functions perform region growing
/// using some variation of the 4-neighbors flood-filling algorithm:
///   Given the coordinates (u, v) of a seed pixel,
///   fill all similarly-colored adjacent pixels with a new color label.
///
/// All of these functions receive the same arguments:
///   img: The image to operate on (and modify).
///   u, v: the coordinates of the seed pixel.
///   label: the new color label (LUT index) to fill the region with.
///
/// And return: the number of labeled pixels.

/// Each function carries out a different version of the algorithm.

/// Region growing using the recursive flood-filling algorithm.
int ImageRegionFillingRecursive(Image img, int u, int v, uint16 label) {
  assert(img != NULL);
  assert(ImageIsValidPixel(img, u, v));
  assert(label < FIXED_LUT_SIZE);

  PIXMEM++;  // leitura do pixel seed
  uint16 old_label = img->image[v][u];

  // Nada a fazer se já tem a label pretendida
  if (old_label == label) {
    return 0;
  }

  int filled = FloodFillRecursiveAux(img, u, v, old_label, label);
  return filled;
}


/// Region growing using a STACK of pixel coordinates to
/// implement the flood-filling algorithm.
typedef struct {
  int u;
  int v;
} Coord;

int ImageRegionFillingWithSTACK(Image img, int u, int v, uint16 label) {
  assert(img != NULL);
  assert(ImageIsValidPixel(img, u, v));
  assert(label < FIXED_LUT_SIZE);

  uint32 W = img->width;
  uint32 H = img->height;

  PIXMEM++;  // leitura seed
  uint16 old_label = img->image[v][u];

  if (old_label == label) {
    return 0;
  }

  size_t max = (size_t)W * (size_t)H;
  Coord* stack = malloc(max * sizeof(Coord));
  check(stack != NULL, "Alloc stack");

  int count = 0;
  size_t top = 0;

  // Marca seed logo para não voltar a ser inserida
  img->image[v][u] = label;
  PIXMEM++;  // escrita
  stack[top++] = (Coord){u, v};
  count++;

  while (top > 0) {
    Coord p = stack[--top];
    int x = p.u;
    int y = p.v;

    // Vizinhos 4-conectados
    const int du[4] = {1, -1, 0, 0};
    const int dv[4] = {0, 0, 1, -1};

    for (int k = 0; k < 4; k++) {
      int nx = x + du[k];
      int ny = y + dv[k];

      if (!ImageIsValidPixel(img, nx, ny)) {
        continue;
      }

      PIXMEM++;  // leitura
      if (img->image[ny][nx] == old_label) {
        img->image[ny][nx] = label;
        PIXMEM++;  // escrita
        stack[top++] = (Coord){nx, ny};
        count++;
      }
    }
  }

  free(stack);
  return count;
}

/// Region growing using a QUEUE of pixel coordinates to
/// implement the flood-filling algorithm.
int ImageRegionFillingWithQUEUE(Image img, int u, int v, uint16 label) {
  assert(img != NULL);
  assert(ImageIsValidPixel(img, u, v));
  assert(label < FIXED_LUT_SIZE);

  uint32 W = img->width;
  uint32 H = img->height;

  PIXMEM++;  // leitura seed
  uint16 old_label = img->image[v][u];

  if (old_label == label) {
    return 0;
  }

  size_t max = (size_t)W * (size_t)H;
  Coord* queue = malloc(max * sizeof(Coord));
  check(queue != NULL, "Alloc queue");

  size_t head = 0;
  size_t tail = 0;
  size_t size = 0;
  int count = 0;

  // Marca seed imediatamente
  img->image[v][u] = label;
  PIXMEM++;  // escrita
  queue[tail++] = (Coord){u, v};
  size++;
  count++;

  const int du[4] = {1, -1, 0, 0};
  const int dv[4] = {0, 0, 1, -1};

  while (size > 0) {
    Coord p = queue[head++];
    size--;
    int x = p.u;
    int y = p.v;

    for (int k = 0; k < 4; k++) {
      int nx = x + du[k];
      int ny = y + dv[k];

      if (!ImageIsValidPixel(img, nx, ny)) {
        continue;
      }

      PIXMEM++;  // leitura
      if (img->image[ny][nx] == old_label) {
        img->image[ny][nx] = label;
        PIXMEM++;  // escrita
        queue[tail++] = (Coord){nx, ny};
        size++;
        count++;
      }
    }
  }

  free(queue);
  return count;
}

/// Image Segmentation

// Função auxiliar recursiva para flood fill
static int FloodFillRecursiveAux(Image img, int u, int v,
                                 uint16 old_label, uint16 new_label) {
  if (!ImageIsValidPixel(img, u, v)) {
    return 0;
  }

  PIXMEM++;  // leitura de pixel
  if (img->image[v][u] != old_label) {
    return 0;
  }

  img->image[v][u] = new_label;
  PIXMEM++;  // escrita de pixel

  int count = 1;

  count += FloodFillRecursiveAux(img, u + 1, v, old_label, new_label);
  count += FloodFillRecursiveAux(img, u - 1, v, old_label, new_label);
  count += FloodFillRecursiveAux(img, u, v + 1, old_label, new_label);
  count += FloodFillRecursiveAux(img, u, v - 1, old_label, new_label);

  return count;
}
/// Label each WHITE region with a different color.
/// - WHITE (the background color) has label (LUT index) 0.
/// - Use GenerateNextColor to create the RGB color for each new region.
///
/// One of the region filling functions above is passed as the
/// last argument, using a function pointer.
///
/// Returns the number of image regions found.
int ImageSegmentation(Image img, FillingFunction fillFunct) {
  assert(img != NULL);
  assert(fillFunct != NULL);

  int regions = 0;
  rgb_t color = 0x000000;  // ponto de partida para GenerateNextColor

  for (uint32 v = 0; v < img->height; v++) {
    for (uint32 u = 0; u < img->width; u++) {
      PIXMEM++;  // leitura
      if (img->image[v][u] == WHITE) {  // label 0 -> background
        // Nova região
        color = GenerateNextColor(color);
        uint16 new_label = (uint16)LUTAllocColor(img, color);

        regions++;
        // Chama a função de preenchimento escolhida (recursiva, stack, queue)
        fillFunct(img, (int)u, (int)v, new_label);
      }
    }
  }

  return regions;
}
