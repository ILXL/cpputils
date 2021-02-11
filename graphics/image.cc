// Copyright 2020 Paul Salvador Inventado and Google LLC
//
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

#include "image.h"

#include <assert.h>

#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include "cimg/CImg.h"

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
  Initialize(width, height);
}

bool Image::Load(const string& filename) {
  if (filename.length() == 0) {
    cout << "You must provide a non-empty filename" << endl;
    return false;
  }
  cimg::exception_mode(0);
  try {
    cimage_ = std::make_unique<cimg_library::CImg<uint8_t>>();
    cimage_->load(filename.c_str());
  } catch (CImgException& e) {
    cout << "Failed to open image file " << filename << endl;
    return false;
  }
  width_ = cimage_->width();
  height_ = cimage_->height();
  if (!IsValid()) {
    cout << "Invaild image file " << filename << endl;
    return false;
  }
  return true;
}

bool Image::Initialize(int width, int height) {
  if (width < 1 || height < 1) return false;
  // Quiet exception mode.
  cimg::exception_mode(0);
  cimage_ = std::make_unique<cimg_library::CImg<uint8_t>>(width, height, 1, 3,
                                                          MAX_PIXEL_VALUE);
  width_ = width;
  height_ = height;
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
  cimage_->save_bmp(filename.c_str());
  return true;
}

bool Image::ShowForMs(int milliseconds, const std::string& title) {
  if (!IsValid()) return false;
  if (!display_) {
    try {
      display_ =
          std::make_unique<cimg_library::CImgDisplay>(*cimage_, title.c_str());
    } catch (CImgException& ex) {
      cout << "Failed to open display" << endl;
      return false;
    }
  } else {
    display_->set_title("%s", title.c_str());
    display_->show();
    display_->display(*cimage_);
    if (milliseconds > 0) display_->wait(milliseconds);
  }
  return true;
}

bool Image::ShowUntilClosed(const string& title, int animation_ms) {
  if (!Show(title)) {
    return false;
  }
  while (!display_->is_closed()) {
    ProcessEvent();
    if (timer_ > animation_ms) {
      ProcessAnimation();
      // Reset the timer.
      timer_ = timer_ % animation_ms;
    }
    // May need to tweak this for performance.
    const int kEventCheckMs = 5;
    display_->wait(kEventCheckMs);
    timer_ += kEventCheckMs;
  }
  timer_ = 0;
  return true;
}

void Image::Flush() {
  if (display_ && !display_->is_closed()) {
    display_->display(*cimage_);
  }
}

void Image::Hide() {
  if (display_ && !display_->is_closed()) {
    display_->close();
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
  return SetRed(x, y, color.Red()) && SetGreen(x, y, color.Green()) &&
         SetBlue(x, y, color.Blue());
}

bool Image::SetRed(int x, int y, int r) { return SetPixel(x, y, 0, r); }

bool Image::SetGreen(int x, int y, int g) { return SetPixel(x, y, 1, g); }

bool Image::SetBlue(int x, int y, int b) { return SetPixel(x, y, 2, b); }

bool Image::DrawLine(int x0, int y0, int x1, int y1, int red, int green,
                     int blue, int thickness) {
  const int color[] = {red, green, blue};
  if (thickness < 1 || !CheckPixelInBounds(x0, y0) ||
      !CheckPixelInBounds(x1, y1) || !CheckColorInBounds(color)) {
    return false;
  }
  if (x0 == x1 && y0 == y1) {
    return true;
  }
  if (thickness == 1) {
    cimage_->draw_line(x0, y0, x1, y1, color);
    return true;
  }

  // Swap x0 and y0 with x1 and y1 so the lower x goes first.
  // Fixes test DrawsLinesWithThicknessOrderDoesntMatter.
  if (x1 < x0) {
    int x_tmp = x1;
    int y_tmp = y1;
    x1 = x0;
    y1 = y0;
    x0 = x_tmp;
    y0 = y_tmp;
  }

  // Use CImage::draw_polygon to draw a thick line.
  const double diff_x = x0 - x1;
  const double diff_y = y0 - y1;
  const double theta = std::atan(-diff_y / diff_x);
  const double hyp = thickness / 2.0;

  // Convert to integer to get nearest pixel.
  const int delta_x = hyp * std::sin(theta);
  const int delta_y = hyp * std::cos(theta);

  CImg<int> points(4, 2);
  points(0, 0) = x0 + delta_x;
  points(0, 1) = y0 + delta_y;
  points(1, 0) = x0 - delta_x;
  points(1, 1) = y0 - delta_y;
  points(2, 0) = x1 - delta_x;
  points(2, 1) = y1 - delta_y;
  points(3, 0) = x1 + delta_x;
  points(3, 1) = y1 + delta_y;

  cimage_->draw_polygon(points, color);
  return true;
}

bool Image::DrawCircle(int x, int y, int radius, int red, int green, int blue) {
  const int color[] = {red, green, blue};
  if (!CheckPixelInBounds(x, y) || !CheckColorInBounds(color)) {
    return false;
  }
  cimage_->draw_circle(x, y, radius, color);
  return true;
}

bool Image::DrawRectangle(int x, int y, int width, int height, int red,
                          int green, int blue) {
  const int color[] = {red, green, blue};
  if (!CheckPixelInBounds(x, y) || !CheckColorInBounds(color)) {
    return false;
  }
  if (width < 0 || height < 0) {
    return false;
  }
  cimage_->draw_rectangle(x, y, x + width - 1, y + height - 1, color);
  return true;
}

bool Image::DrawPolygon(const std::vector<int>& points, int red, int green, int blue) {
  const int color[] = {red, green, blue};
  if (!CheckColorInBounds(color)) {
    return false;
  }
  if (points.size() % 2 != 0) {
    cout << "Invalid vector of vertices. Each vertex should be represented by 2 integers." << endl;
    return false;
  }
  for (int i = 0; i < points.size(); i += 2) {
    if (!CheckPixelInBounds(points[i], points[i + 1])) {
      return false;
    }
  }
  CImg<int> c_points(points.size() / 2, 2);
  int c_point_loc = 0;
  for (int i = 0; i < points.size(); i += 2) {
    c_points(c_point_loc, 0) = points[i];
    c_points(c_point_loc++, 1) = points[i+1];
  }
  cimage_->draw_polygon(c_points, color);

  return true;
}

bool Image::DrawText(int x, int y, const string& text, int font_size, int red,
                     int green, int blue) {
  const int color[] = {red, green, blue};
  if (!CheckPixelInBounds(x, y) || !CheckColorInBounds(color)) {
    return false;
  }
  cimage_->draw_text(x, y, text.c_str(), color, 0, 1, font_size);
  return true;
}

void Image::ProcessEvent() {
  int mouse_x = display_->mouse_x();
  int mouse_y = display_->mouse_y();
  if (display_->button() & 1 && mouse_x >= 0 && mouse_y >= 0) {
    // Left button has been pressed or moved.
    MouseAction action;
    if (latest_event_.GetMouseAction() == MouseAction::kReleased ||
        latest_event_.GetMouseAction() == MouseAction::kMoved) {
      action = MouseAction::kPressed;
    } else {
      if (mouse_x == latest_event_.GetX() && mouse_y == latest_event_.GetY()) {
        // Mouse position hasn't changed, so don't send a drag event.
        return;
      }
      action = MouseAction::kDragged;
    }
    latest_event_ = MouseEvent(mouse_x, mouse_y, action);
    for (auto listener : mouse_listeners_) {
      listener->OnMouseEvent(latest_event_);
    }
  } else if (!(display_->button() & 1)) {
    // Left button is not clicked.
    if (latest_event_.GetMouseAction() == MouseAction::kDragged ||
        latest_event_.GetMouseAction() == MouseAction::kPressed) {
      // We were dragging or pressing, send a release.
      latest_event_ = MouseEvent(latest_event_.GetX(), latest_event_.GetY(),
                                 MouseAction::kReleased);
      for (auto listener : mouse_listeners_) {
        listener->OnMouseEvent(latest_event_);
      }
    } else if ((mouse_x != latest_event_.GetX() ||
                mouse_y != latest_event_.GetY()) &&
               (mouse_x >= 0 && mouse_y >= 0)) {
      // Mouse position has changed, send a move.
      latest_event_ = MouseEvent(mouse_x, mouse_y, MouseAction::kMoved);
      for (auto listener : mouse_listeners_) {
        listener->OnMouseEvent(latest_event_);
      }
    }
  }
}

void Image::ProcessAnimation() {
  for (auto listener : animation_listeners_) {
    listener->OnAnimationStep();
  }
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
      cout << value[i] << " is out of range, must be between 0 and 255."
           << endl;
      return false;
    }
  }
  return true;
}

int Image::GetPixel(int x, int y, int channel) const {
  if (!CheckPixelInBounds(x, y)) return -1;
  const uint8_t* r = cimage_->data(x, y, channel);
  return static_cast<int>(*r);
}

bool Image::SetPixel(int x, int y, int channel, int value) {
  if (!CheckPixelInBounds(x, y)) return false;
  if (!CheckColorInBounds(value)) return false;
  uint8_t* px = cimage_->data(x, y, channel);
  *px = static_cast<uint8_t>(value);
  // Inefficient. Should we have a "flush" or similar?
  return true;
}

}  // namespace graphics
