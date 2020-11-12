// Copyright 2020 Paul Salvador Inventado and Google LLC
//
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

#ifndef GRAPHICS_IMAGE_EVENT_H
#define GRAPHICS_IMAGE_EVENT_H

namespace graphics {

/**
 * Enum representing whether a button was pressed or released.
 */
enum class MouseAction {
  // Left button down.
  kPressed = 0,
  // Moved while left button was down.
  kDragged,
  // Left button up.
  kReleased,
  // Moved but the left button was not down.
  kMoved,
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

/**
 * Abstract interface for listening to MouseEvents on images.
 * Use Image::AddMouseEventListener and Image::RemoveMouse EventListener to start and stop
 * listening for mouse events on the Image's display, shown with Image::ShowUntilClosed().
 */
class MouseEventListener {
 public:
  virtual void OnMouseEvent(const MouseEvent& event) = 0;
};

/**
 * Abstract interface for listening to AnimationEvents on images. Add and
 * remove with Image::Add/RemoveAnimationEventListener
 * Use Image::ShowUntilClosed with a ms for custom animation duration, and the
 * AnimationListener::OnAnimationStep() will be called at that duration.
 */
class AnimationEventListener {
 public:
  virtual void OnAnimationStep() = 0;
};

}  // namespace graphics

#endif  // GRAPHICS_IMAGE_EVENT_H
