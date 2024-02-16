// Project UID af1f95f547e44c8ea88730dfb185559d

#include <cassert>
#include "Image.h"

// REQUIRES: img points to an Image
//           0 < width && width <= MAX_MATRIX_WIDTH
//           0 < height && height <= MAX_MATRIX_HEIGHT
// MODIFIES: *img
// EFFECTS:  Initializes the Image with the given width and height.
// NOTE:     Do NOT use new or delete here.
void Image_init(Image* img, int width, int height) {
    *img = {width, height};
    Matrix_init(&img->red_channel, width, height);
    Matrix_init(&img->green_channel, width, height);
    Matrix_init(&img->blue_channel, width, height);
}

// REQUIRES: img points to an Image
//           is contains an image in PPM format without comments
//           (any kind of whitespace is ok)
// MODIFIES: *img
// EFFECTS:  Initializes the Image by reading in an image in PPM format
//           from the given input stream.
// NOTE:     See the project spec for a discussion of PPM format.
// NOTE:     Do NOT use new or delete here.
void Image_init(Image* img, std::istream& is) {
    std::string Plain_PPM;
    is >> Plain_PPM;
    int width;
    is >> width;
    int height;
    is >> height;
    int max;
    is >> max;
    
    int red = 0;
    int green = 0;
    int blue = 0;
    *img = {width, height};
    Matrix_init(&img->red_channel, width, height);
    Matrix_init(&img->green_channel, width, height);
    Matrix_init(&img->blue_channel, width, height);
    
    for(int i = 0; i < height; i++){
        for(int j = 0; j < width; j++){
            is >> red;
            is >> green;
            is >> blue;
            *Matrix_at(&img->red_channel, i, j) = red;
            *Matrix_at(&img->green_channel, i, j) = green;
            *Matrix_at(&img->blue_channel, i, j) = blue;
        }
    }
}

// REQUIRES: img points to a valid Image
// EFFECTS:  Writes the image to the given output stream in PPM format.
//           You must use the kind of whitespace specified here.
//           First, prints out the header for the image like this:
//             P3 [newline]
//             WIDTH [space] HEIGHT [newline]
//             255 [newline]
//           Next, prints out the rows of the image, each followed by a
//           newline. Each pixel in a row is printed as three ints
//           for its red, green, and blue components, in that order. Each
//           int is followed by a space. This means that there will be an
//           "extra" space at the end of each line. See the project spec
//           for an example.
void Image_print(const Image* img, std::ostream& os) {
    os << "P3" << "\n";
    os << img->width << " " << img->height << "\n";
    os << "255" << "\n";
    for(int i = 0; i < (img->height); i++){
        for(int j = 0; j < img->width; j++){
            os << *Matrix_at(&img->red_channel, i, j) << " ";
            os << *Matrix_at(&img->green_channel, i, j) << " ";
            os << *Matrix_at(&img->blue_channel, i, j) << " ";
        }
        os << "\n";
    }
}

// REQUIRES: img points to a valid Image
// EFFECTS:  Returns the width of the Image.
int Image_width(const Image* img) {
    int wid = 0;
    wid = img->width;
    return wid;
}

// REQUIRES: img points to a valid Image
// EFFECTS:  Returns the height of the Image.
int Image_height(const Image* img) {
    int hi = 0;
    hi = img->height;
    return hi;
}

// REQUIRES: img points to a valid Image
//           0 <= row && row < Image_height(img)
//           0 <= column && column < Image_width(img)
// EFFECTS:  Returns the pixel in the Image at the given row and column.
Pixel Image_get_pixel(const Image* img, int row, int column) {
    Pixel pix;
    int red = *Matrix_at(&img->red_channel, row, column);
    int green = *Matrix_at(&img->green_channel, row, column);
    int blue = *Matrix_at(&img->blue_channel, row, column);
    pix = {red, green, blue};
    return pix;
}

// REQUIRES: img points to a valid Image
//           0 <= row && row < Image_height(img)
//           0 <= column && column < Image_width(img)
// MODIFIES: *img
// EFFECTS:  Sets the pixel in the Image at the given row and column
//           to the given color.
void Image_set_pixel(Image* img, int row, int column, Pixel color) {
    int red = color.r;
    int green = color.g;
    int blue = color.b;
    
    *Matrix_at(&img->red_channel, row, column) = red;
    *Matrix_at(&img->green_channel, row, column) = green;
    *Matrix_at(&img->blue_channel, row, column) = blue;
}

// REQUIRES: img points to a valid Image
// MODIFIES: *img
// EFFECTS:  Sets each pixel in the image to the given color.
void Image_fill(Image* img, Pixel color) {
    int red = color.r;
    int green = color.g;
    int blue = color.b;
    
    Matrix_fill(&img->red_channel, red);
    Matrix_fill(&img->green_channel, green);
    Matrix_fill(&img->blue_channel, blue);
}
