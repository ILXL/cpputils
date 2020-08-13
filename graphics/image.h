#include <iostream>
#include <memory>
#include <set>
#include <string>

#include "image_event.h"

#ifndef GRAPHICS_IMAGE_H
#define GRAPHICS_IMAGE_H

namespace cimg_library {
template <class>
class CImg;
class CImgDisplay;
}  // namespace cimg_library

using namespace cimg_library;

namespace graphics {

/**
 * Represents an RGB pixel color, where |red|, |green| and |blue|
 * may be between 0 and 255, inclusive.
 */
class Color {
 public:
  explicit Color(int red, int green, int blue);
  ~Color() = default;

  bool operator==(const Color& other) const {
    return red_ == other.Red() && green_ == other.Green() && blue_ == other.Blue();
  }

  bool operator!=(const Color& other) const {
    return red_ != other.Red() || green_ != other.Green() || blue_ != other.Blue();
  }

  int Red() const { return red_; }
  int Green() const { return green_; }
  int Blue() const { return blue_; }

 private:
  int red_;
  int green_;
  int blue_;
};

// Use by gtest.
static void PrintTo(const Color& color, std::ostream* stream) {
  *stream << "Color: (" << color.Red() << "," << color.Green() << ","
          << color.Blue() << ")";
}

class Image {
 public:
  Image();
  ~Image();

  /**
   * Creates a new blank image of size (width, height).
   */
  explicit Image(int width, int height);

  // Disallow copy and assign.
  Image(const Image&) = delete;
  Image& operator=(const Image&) = delete;

  /*
   * Loads an image from a file. Returns false if the image could
   * not be loaded. Note: this clears any current state, including
   * pixel values, width and height.
   */
  bool Load(const std::string& filename);

  /*
   * Resets the image to be a blank white image size |width| by |height|,
   * returns false if unsuccessful (if |width| or |height| are less than 1).
   */
  bool Initialize(int width, int height);

  /**
   * Saves the current image to the file with |filename| in bitmap
   * format. Returns false if saving failed.
   */
  bool SaveImageBmp(const std::string& filename) const;

  /**
   * Shows the current image. Returns false if the image could not be shown.
   */
  bool Show() { return Show("Image"); }

  /**
   * Shows the image in a window with the title |title|. Returns false if
   * the image could not be shown.
   */
  bool Show(const std::string& title);

  /**
   * Shows the current image until the window is closed. Returns false if
   * the image could not be shown.
   */
  bool ShowUntilClosed() { return ShowUntilClosed("Image"); }

  /**
   * Shows the current image until the window is closed.
   * Optional |title| for the window. Returns false if the image
   * could not be shown.
   */
  bool ShowUntilClosed(const std::string& title);

  /**
   * Refreshes the display with any update to the image. Does nothing if the
   * image is not displayed.
   */
  void Flush();

  /**
   * Hides the image if it is currently being shown.
   */
  void Hide();

  /**
   * Returns the width of the loaded image, in pixels.
   */
  int GetWidth() const { return width_; }

  /**
   * Returns the height of the loaded image, in pixels.
   */
  int GetHeight() const { return height_; }

  /**
   * Gets the color at pixel at position (x, y) in the image.
   * Returns (-1, -1, -1) if (x, y) is out of bounds.
   */
  Color GetColor(int x, int y) const;

  /**
   * Returns the red component of the RGB pixel at position
   * (x, y) in the image. Returns -1 if (x, y) is out of bounds.
   */
  int GetRed(int x, int y) const;

  /**
   * Returns the green component of the RGB pixel at position
   * (x, y) in the image. Returns -1 if (x, y) is out of bounds.
   */
  int GetGreen(int x, int y) const;

  /**
   * Returns the blue component of the RGB pixel at position
   * (x, y) in the image. Returns -1 if (x, y) is out of bounds.
   */
  int GetBlue(int x, int y) const;

  /**
   * Sets the color of the RGB pixel at position (x, y)
   * in the image. Returns false if (x, y) is out of bounds or
   * red, green or blue are out of range [0, 255].
   */
  bool SetColor(int x, int y, const Color& color);

  /**
   * Sets the red component of the RGB pixel at position (x, y)
   * in the image. Returns false if (x, y) is out of bounds or
   * |r| is out of range [0, 255].
   */
  bool SetRed(int x, int y, int r);

  /**
   * Sets the green component of the RGB pixel at position (x, y)
   * in the image. Returns false if (x, y) is out of bounds or
   * |g| is out of range [0, 255].
   */
  bool SetGreen(int x, int y, int g);

  /**
   * Sets the blue component of the RGB pixel at position (x, y)
   * in the image. Returns false if (x, y) is out of bounds or
   * |b| is out of range [0, 255].
   */
  bool SetBlue(int x, int y, int b);

  /**
   * Draws a line from (x0, y0) to (x1, y1) with color |color| and optional width |thickness|.
   * Returns false if params are out of bounds.
   */
  bool DrawLine(int x0, int y0, int x1, int y1, const Color& color, int thickness = 1) {
    return DrawLine(x0, y0, x1, y1, color.Red(), color.Green(), color.Blue(), thickness);
  }

  /**
   * Draws a line from (x0, y0) to (x1, y1) with color specified  by |red|, |green| and
   * |blue| channels, and optional width |thickness|. Returns false if params are out of bounds.
   */
  bool DrawLine(int x0, int y0, int x1, int y1, int red, int green, int blue, int thickness = 1);

  /**
   * Draws a circle centered at (x, y) with radius |radius|, and color
   * |color|. Returns false if params are out of bounds.
   */
  bool DrawCircle(int x, int y, int radius, const Color& color) {
    return DrawCircle(x, y, radius, color.Red(), color.Green(), color.Blue());
  }

  /**
   * Draws a circle centered at (x, y) with radius |radius|, and color
   * specified by |red|, |green| and |blue| channels. Returns false if
   * params are out of bounds.
   */
  bool DrawCircle(int x, int y, int radius, int red, int green, int blue);

  /**
   * Draws a rectangle with upper left corner at (x, y) and size
   * |width| by |height|, colored by |color|. Returns false if
   * params are out of bounds.
   */
  bool DrawRectangle(int x, int y, int width, int height, const Color& color) {
    return DrawRectangle(x, y, width, height, color.Red(), color.Green(),
                         color.Blue());
  }

  /**
   * Draws a rectangle with upper left corner at (x, y) and size
   * |width| by |height|, colored by |red|, |green| and |blue|.
   * Returns false if params are out of bounds.
   */
  bool DrawRectangle(int x, int y, int width, int height, int red, int green,
                     int blue);

  /**
   * Draws the string |text| with position (x,y) at the top left corner,
   * with |font_size| in pixels, colored by |color|. Returns false if the
   * params are out of bounds.
   */
  bool DrawText(int x, int y, const std::string& text, int font_size,
                const Color& color) {
    return DrawText(x, y, text, font_size, color.Red(), color.Green(),
                    color.Blue());
  }

  /**
   * Draws the string |text| with position (x,y) at the top left corner,
   * with |font_size| in pixels, colored by |red|, |green| and |blue|.
   * Returns false if the params are out of bounds.
   */
  bool DrawText(int x, int y, const std::string& text, int font_size, int red,
                int green, int blue);

  /**
   * Adds a MouseEventListener to this image. This EventListener's OnMouseEvent
   * function will be called whenever the display receives left-button mouse
   * events.
   */
  void AddMouseEventListener(MouseEventListener& listener) {
    if (mouse_listeners_.find(&listener) == mouse_listeners_.end()) {
      mouse_listeners_.insert(&listener);
    }
  }

  /**
   * Removes a MouseEventListener if it was added. This EventListener's
   * OnMouseEvent function will no longer be called when the display receives
   * mouse events.
   */
  void RemoveMouseEventListener(MouseEventListener& listener) {
    if (mouse_listeners_.find(&listener) != mouse_listeners_.end()) {
      mouse_listeners_.erase(&listener);
    }
  }

 private:
  friend class TestEventGenerator;

  CImgDisplay* GetDisplayForTesting() {
    if (!display_) return nullptr;
    return display_.get();
  }

  void ProcessEvent();

  bool IsValid() const { return height_ > 0 && width_ > 0; }

  bool CheckPixelInBounds(int x, int y) const;

  bool CheckColorInBounds(int value) const;

  bool CheckColorInBounds(const int value[]) const;

  int GetPixel(int x, int y, int channel) const;

  bool SetPixel(int x, int y, int channel, int value);

  int width_ = 0;
  int height_ = 0;
  std::unique_ptr<CImg<uint8_t>> cimage_;
  std::unique_ptr<CImgDisplay> display_;

  // Mouse listeners. Unowned.
  std::set<MouseEventListener*> mouse_listeners_;

  MouseEvent latest_event_ = MouseEvent(0, 0, MouseAction::kReleased);
};

}  // namespace graphics

#endif  // GRAPHICS_IMAGE_H
