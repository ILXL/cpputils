#include "../cimg/CImg.h"
#include "../image.h"

#ifndef GRAPHICS_TEST_EVENT_GENERATOR_H
#define GRAPHICS_TEST_EVENT_GENERATOR_H

namespace graphics {

class TestEventGenerator {
 public:
  TestEventGenerator(graphics::Image* image) { image_ = image; }

  void MouseDown(int x, int y) {
    if (!image_->GetDisplayForTesting()) return;
    cimg_library::CImgDisplay* display = image_->GetDisplayForTesting();
    display->set_mouse(x, y);
    display->set_button(1, true /* is pressed */);
    image_->ProcessEvent();
  }

  void MoveMouseTo(int x, int y) {
    if (!image_->GetDisplayForTesting()) return;
    cimg_library::CImgDisplay* display = image_->GetDisplayForTesting();
    display->set_mouse(x, y);
    image_->ProcessEvent();
  }

  void MouseUp() {
    if (!image_->GetDisplayForTesting()) return;
    cimg_library::CImgDisplay* display = image_->GetDisplayForTesting();
    display->set_button(1, false /* is pressed*/);
    image_->ProcessEvent();
  }

  void RightMouseDown() {
    if (!image_->GetDisplayForTesting()) return;
    cimg_library::CImgDisplay* display = image_->GetDisplayForTesting();
    display->set_button(0x2, true /* is pressed*/);
    image_->ProcessEvent();
  }

  void RightMouseUp() {
    if (!image_->GetDisplayForTesting()) return;
    cimg_library::CImgDisplay* display = image_->GetDisplayForTesting();
    display->set_button(0x2, false /* is pressed*/);
    image_->ProcessEvent();
  }

 private:
  graphics::Image* image_;  // Unowned
};

}  // namespace graphics

#endif  // GRAPHICS_TEST_EVENT_GENERATOR_H
