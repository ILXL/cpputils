#include <iostream>
#include <memory>
#include <string>
#include <assert.h>

#include "cimg/CImg.h"
#include "image.h"

using std::cout;
using std::endl;
using std::string;

namespace graphics {

namespace {
  constexpr int MAX_PIXEL_VALUE = 255;
}

Color::Color(int red, int green, int blue) {
  if (red < 0 || red > MAX_PIXEL_VALUE) red = 0;
  if (blue < 0 || blue > MAX_PIXEL_VALUE) blue = 0;
  if (green < 0 || green > MAX_PIXEL_VALUE) green = 0;
  red_ = red;
  green_ = green;
  blue_ = blue;
}

Image::Image() = default;

Image::~Image() = default;

Image::Image(int width, int height) {
  assert(width > 0 && height > 0 && "Width and height must be at least 1");
  // Quiet exception mode.
  cimg::exception_mode(0);
  cimage = std::make_unique<cimg_library::CImg<uint8_t>>(width, height, 1, 3, MAX_PIXEL_VALUE);
  width_ = width;
  height_ = height;
}

bool Image::Load(const string& filename) {
  if (filename.length() == 0) {
    cout << "You must provide a non-empty filename" << endl;
    return false;
  }
  cimg::exception_mode(0);
  try {
    cimage = std::make_unique<cimg_library::CImg<uint8_t>>();
    cimage->load(filename.c_str());
  } catch (CImgException &e) {
    cout << "Failed to open image file " << filename << endl;
    return false;
  }
  width_ = cimage->width();
  height_ = cimage->height();
  if (!IsValid()) {
    cout << "Invaild image file " << filename << endl;
    return false;
  }
  return true;
}

bool Image::SaveImageBmp(const string& filename) const {
  if (!IsValid()) {
    return false;
  }
  if (filename.length() == 0) {
    cout << "You must provide a non-empty filename" << endl;
    return false;
  }
  cimage->save_bmp(filename.c_str());
  return true;
}

bool Image::Show(const string& title) {
  if (!IsValid()) return false;
  if (!display) {
    try {
      display = std::make_unique<cimg_library::CImgDisplay>(*cimage, title.c_str());
    } catch (CImgException& ex) {
      cout << "Failed to open display" << endl;
      return false;
    }
  } else {
    display->set_title(title.c_str());
    display->show();
    display->display(*cimage);
  }
  return true;
}

bool Image::ShowUntilClosed(const string& title) {
  if (!Show(title)) {
    return false;
  }
  while (!display->is_closed()) display->wait();
  return true;
}

void Image::Hide() {
  if (display && !display->is_closed()) {
    display->close();
    display.reset();
  }
}

Color Image::GetColor(int x, int y) const {
  if (!CheckPixelInBounds(x, y)) {
    return Color(0, 0, 0);
  }
  Color color(GetRed(x, y), GetGreen(x, y), GetBlue(x, y));
  return color;
}

int Image::GetRed(int x, int y) const { return GetPixel(x, y, 0); }

int Image::GetGreen(int x, int y) const { return GetPixel(x, y, 1); }

int Image::GetBlue(int x, int y) const { return GetPixel(x, y, 2); }

bool Image::SetColor(int x, int y, const Color& color) {
  if (!CheckPixelInBounds(x, y)) {
    return false;
  }
  return SetRed(x, y, color.Red()) && SetGreen(x, y, color.Green())
         && SetBlue(x, y, color.Blue());
}

bool Image::SetRed(int x, int y, int r) { return SetPixel(x, y, 0, r); }

bool Image::SetGreen(int x, int y, int g) { return SetPixel(x, y, 1, g); }

bool Image::SetBlue(int x, int y, int b) { return SetPixel(x, y, 2, b); }

bool Image::DrawLine(int x0, int y0, int x1, int y1, int red, int green,
                     int blue) {
  const int color[] = {red, green, blue};
  if (!CheckPixelInBounds(x0, y0) || !CheckPixelInBounds(x1, y1)
      || !CheckColorInBounds(color)) {
    return false;
  }
  cimage->draw_line(x0, y0, x1, y1, color);
  return true;
}

bool Image::DrawCircle(int x, int y, int radius, int red, int green, int blue) {
  const int color[] = {red, green, blue};
  if (!CheckPixelInBounds(x, y) || !CheckColorInBounds(color)) {
    return false;
  }
  cimage->draw_circle(x, y, radius, color);
  return true;
}

bool Image::DrawRectangle(int x, int y, int width, int height, int red,
                          int green, int blue) {
  const int color[] = {red, green, blue};
  if (!CheckPixelInBounds(x, y) || !CheckColorInBounds(color)) {
    return false;
  }
  cimage->draw_rectangle(x, y, x + width, y + height, color);
  return true;
}

bool Image::DrawText(int x, int y, const string& text, int font_size, int red, int green, int blue) {
  const int color[] = {red, green, blue};
  if (!CheckPixelInBounds(x, y) || !CheckColorInBounds(color)) {
    return false;
  }
  cimage->draw_text(x, y, text.c_str(), color, 0, 1, font_size);
  return true;
}

bool Image::CheckPixelInBounds(int x, int y) const {
  if (x < 0 || y < 0 || x >= GetWidth() || y >= GetHeight()) {
    cout << "(" << x << ", " << y << ") is out of bounds." << endl;
    return false;
  }
  return true;
}

bool Image::CheckColorInBounds(int value) const {
  if (value < 0 || value > MAX_PIXEL_VALUE) {
    cout << value << " is out of range, must be between 0 and 255." << endl;
    return false;
  }
  return true;
}

bool Image::CheckColorInBounds(const int value[]) const {
  for (int i = 0; i < 3; i++) {
    if (value[i] < 0 || value[i] > MAX_PIXEL_VALUE) {
      cout << value[i] << " is out of range, must be between 0 and 255." << endl;
      return false;
    }
  }
  return true;
}

int Image::GetPixel(int x, int y, int channel) const {
  if (!CheckPixelInBounds(x, y)) return -1;
  const uint8_t *r = cimage->data(x, y, channel);
  return static_cast<int>(*r);
}

bool Image::SetPixel(int x, int y, int channel, int value) {
  if (!CheckPixelInBounds(x, y)) return false;
  if (!CheckColorInBounds(value)) return false;
  uint8_t *px = cimage->data(x, y, channel);
  *px = static_cast<uint8_t>(value);
  return true;
}

}  // namespace graphics
