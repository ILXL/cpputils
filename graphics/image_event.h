#ifndef GRAPHICS_IMAGE_EVENT_H
#define GRAPHICS_IMAGE_EVENT_H

namespace graphics {

/**
 * Enum representing whether a button was pressed or released.
 */
enum class MouseAction {
  kPressed = 0,
  kDragged,
  kReleased,
};

/**
 * Represents a left-button mouse event at a particular location within a
 * displayed Image.
 */
class MouseEvent {
 public:
  explicit MouseEvent(int x, int y, MouseAction action) {
    x_ = x;
    y_ = y;
    action_ = action;
  }
  ~MouseEvent() = default;

  int GetX() const { return x_; }
  int GetY() const { return y_; }
  MouseAction GetMouseAction() const { return action_; }

 private:
  int x_;
  int y_;
  MouseAction action_;
};

class MouseEventListener {
 public:
  virtual void OnMouseEvent(const MouseEvent& event) = 0;
};

}  // namespace graphics

#endif  // GRAPHICS_IMAGE_EVENT_H
