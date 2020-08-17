#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <string>

#include "../image.h"
#include "image_test_utils.h"
#include "test_event_generator.h"

TEST(ImageTest, CreatesImageWithoutCrashing) {
  graphics::Image image(100, 100);
}

TEST(ImageTest, LoadsImageWithoutCrashing) {
  graphics::Image image;
  image.Load("example_fractal_tree.png");
  EXPECT_TRUE(image.GetWidth() > 0);
  EXPECT_TRUE(image.GetHeight() > 0);
}

TEST(ImageTest, NonExistantImage) {
  graphics::Image image;
  ASSERT_FALSE(image.Load("does_not_exist"));

  // Shouldn't crash even if we use getters and setters.
  EXPECT_FALSE(image.SetRed(0, 0, 250));
  EXPECT_EQ(image.GetRed(0, 0), -1);
  EXPECT_EQ(image.GetGreen(1, 1), -1);
  EXPECT_EQ(image.GetBlue(-1, 1), -1);

  // Shouldn't be able to save or show the image.
  EXPECT_FALSE(image.SaveImageBmp("invalid.bmp"));

  // Should do nothing but not crash.
  EXPECT_FALSE(image.Show());
}

TEST(ImageTest, InvalidImageFile) {
  graphics::Image image;
  EXPECT_FALSE(image.Load(""));
  EXPECT_FALSE(image.Load("image.cpp"));
}

TEST(ImageDeathTest, InvalidSizeConstructorZero) {
  ASSERT_DEATH(graphics::Image image(0, 0), "");
}

TEST(ImageDeathTest, InvalidSizeConstructorNegative) {
  ASSERT_DEATH(graphics::Image image(10, -1), "");
}

TEST(ImageTest, BlankImageCreation) {
  // Check size is correct.
  graphics::Image image(10, 10);
  EXPECT_EQ(image.GetWidth(), 10);
  EXPECT_EQ(image.GetHeight(), 10);

  // Check initial color is correct.
  const graphics::Color white(255, 255, 255);
  for (int i = 0; i < 10; i++) {
    for (int j = 0; j < 10; j++) {
      EXPECT_EQ(image.GetColor(i, j), white);
    }
  }

  // Doesn't crash when accessing out of bounds pixels.
  const graphics::Color black{0, 0, 0};
  EXPECT_EQ(image.GetColor(-1, 0), black);
  EXPECT_EQ(image.GetColor(5, 50), black);
}

TEST(ImageTest, InvalidDrawing) {
  graphics::Image image(50, 50);
  graphics::Color white(255, 255, 255);

  // Doesn't crash when setting an invalid color.
  image.SetRed(0, 0, -1);
  image.SetBlue(0, 0, 256);

  // Nothing is changed.
  EXPECT_EQ(image.GetColor(0, 0), white);

  image.SetRed(0, 0, -20);
  EXPECT_EQ(image.GetColor(0, 0), white);
}

TEST(ImageTest, Drawing) {
  graphics::Image image(50, 50);
  graphics::Color white(255, 255, 255);
  graphics::Color blue(0, 0, 255);
  image.DrawCircle(20, 20, 5, blue);
  // Spot check some pixels.
  EXPECT_EQ(image.GetColor(20, 20), blue);
  EXPECT_EQ(image.GetColor(25, 20), blue);
  EXPECT_EQ(image.GetColor(26, 20), white);

  image.DrawRectangle(2, 2, 15, 10, 255, 0, 0);
  for (int i = 2; i < 17; i++) {
    for (int j = 2; j < 12; j++) {
      EXPECT_EQ(image.GetRed(i, j), 255);
      EXPECT_EQ(image.GetGreen(i, j), 0);
      EXPECT_EQ(image.GetBlue(i, j), 0);
    }
  }

  // Drawing something out of bounds doesn't work.
  image.DrawRectangle(-1, -1, 50, 50, 0, 255, 0);
  EXPECT_EQ(image.GetColor(0, 0), white);

  image.DrawCircle(40, 50, 100, 0, 255, 0);
  EXPECT_EQ(image.GetColor(0, 0), white);

  // Hard to test the line because of anti-aliasing.
  image.DrawLine(0, 0, 40, 40, 255, 0, 0);
  EXPECT_EQ(image.GetGreen(0, 0), 0);
  EXPECT_EQ(image.GetBlue(0, 0), 0);

  // Drawing text works (hard to check sizing...)
  image.DrawText(50, 50, "Kitties are cute", 32, 100, 200, 255);

  // Show works.
  EXPECT_TRUE(image.Show("Test image"));

  // Hide works.
  image.Hide();
}

TEST(ImageTest, SavesAndLoadsImage) {
  int size = 50;
  graphics::Image image(size, size);

  // Try drawing a circle that goes off the edge to ensure
  // no crashes from CImg.
  image.DrawCircle(size / 2, size / 2, 40, 100, 100, 100);
  std::string filename = "test_image.bmp";
  EXPECT_TRUE(image.SaveImageBmp(filename));

  graphics::Image loaded;
  EXPECT_TRUE(loaded.Load(filename));
  EXPECT_EQ(loaded.GetWidth(), image.GetWidth());
  EXPECT_EQ(loaded.GetHeight(), image.GetHeight());

  // Check every pixel is correct.
  for (int i = 0; i < size; i++) {
    for (int j = 0; j < size; j++) {
      EXPECT_EQ(image.GetColor(i, j), loaded.GetColor(i, j));
    }
  }

  // Delete the result.
  remove(filename.c_str());
}

TEST(ImageTest, DrawsLinesWithThickness) {
  remove("DrawsLinesWithThicknessHorizontal.bmp");
  remove("DrawsLinesWithThicknessVertical.bmp");

  int thickness = 20;
  int size = 100;
  graphics::Color blue(0, 0, 255);

  graphics::Image expected(size, size);
  graphics::Image actual(size, size);

  // Horizontal rectangle is the same as a thick line.
  expected.DrawRectangle(10, 40, 80, thickness, blue);
  actual.DrawLine(10, 50, 90, 50, blue, thickness);
  EXPECT_TRUE(ImagesMatch(&expected, &actual, "DrawsLinesWithThicknessHorizontal.bmp",
      DiffType::kTypeHighlight));

  // Vertical rectangle is the same as a thick line.
  expected.DrawRectangle(40, 5, thickness, 90, blue);
  actual.DrawLine(50, 5, 50, 95, blue, thickness);
  EXPECT_TRUE(ImagesMatch(&expected, &actual, "DrawsLinesWithThicknessVertical.bmp",
      DiffType::kTypeHighlight));

  // 45 degree rectangle is as thick as expected. Check some points that wouldn't be
  // colored unless the line had appropriate thickness.
  graphics::Color green(0, 255, 0);
  actual.DrawLine(0, 0, size - 1, size - 1, green, thickness);
  EXPECT_EQ(actual.GetColor(thickness / 2, 0), green);
  EXPECT_EQ(actual.GetColor(0, thickness / 2), green);
  EXPECT_EQ(actual.GetColor(size / 2, size / 2 - thickness / 2), green);
  EXPECT_EQ(actual.GetColor(size / 2, size / 2 + thickness / 2), green);

  // But not too wide!
  EXPECT_NE(actual.GetColor(size / 2, size / 2 + std::sqrt(2 * thickness * thickness)), green);
}

class TestEventListener : public graphics::MouseEventListener {
 public:
  TestEventListener() = default;
  ~TestEventListener() = default;
  void OnMouseEvent(const graphics::MouseEvent& event) override {
    latest_event_ = graphics::MouseEvent(event.GetX(), event.GetY(), event.GetMouseAction());
  }

  graphics::MouseEvent GetLatestEvent() {
    return latest_event_;
  }
 private:
  graphics::MouseEvent latest_event_ = graphics::MouseEvent(0, 0, graphics::MouseAction::kReleased);
};

// We can send fake events if we have a reference to the image on which to send events, and we
// are not in the ShowUntilClosed loop.
TEST(ImageEventTest, HandlesEvents) {
  TestEventListener listener;
  int size = 100;
  graphics::Image image(size, size);
  image.AddMouseEventListener(listener);
  image.Show();

  graphics::TestEventGenerator generator(&image);
  generator.MouseDown(10, 20);
  EXPECT_EQ(listener.GetLatestEvent().GetX(), 10);
  EXPECT_EQ(listener.GetLatestEvent().GetY(), 20);
  EXPECT_EQ(listener.GetLatestEvent().GetMouseAction(), graphics::MouseAction::kPressed);

  generator.MoveMouseTo(30, 80);
  EXPECT_EQ(listener.GetLatestEvent().GetX(), 30);
  EXPECT_EQ(listener.GetLatestEvent().GetY(), 80);
  EXPECT_EQ(listener.GetLatestEvent().GetMouseAction(), graphics::MouseAction::kDragged);

  generator.MoveMouseTo(90, 80);
  EXPECT_EQ(listener.GetLatestEvent().GetX(), 90);
  EXPECT_EQ(listener.GetLatestEvent().GetY(), 80);
  EXPECT_EQ(listener.GetLatestEvent().GetMouseAction(), graphics::MouseAction::kDragged);

  generator.MouseUp();
  EXPECT_EQ(listener.GetLatestEvent().GetX(), 90);
  EXPECT_EQ(listener.GetLatestEvent().GetY(), 80);
  EXPECT_EQ(listener.GetLatestEvent().GetMouseAction(), graphics::MouseAction::kReleased);

  generator.MoveMouseTo(30, 30);
  // Nothing happens, mosue is up.
  EXPECT_EQ(listener.GetLatestEvent().GetX(), 90);
  EXPECT_EQ(listener.GetLatestEvent().GetY(), 80);
  EXPECT_EQ(listener.GetLatestEvent().GetMouseAction(), graphics::MouseAction::kReleased);

  // Ensure other mouse buttons don't interfere.
  generator.MouseDown(10, 20);
  generator.RightMouseDown();
  generator.RightMouseUp();
  EXPECT_EQ(listener.GetLatestEvent().GetX(), 10);
  EXPECT_EQ(listener.GetLatestEvent().GetY(), 20);
  EXPECT_EQ(listener.GetLatestEvent().GetMouseAction(), graphics::MouseAction::kPressed);
  generator.RightMouseDown();
  generator.MouseUp();
  EXPECT_EQ(listener.GetLatestEvent().GetMouseAction(), graphics::MouseAction::kReleased);

  image.RemoveMouseEventListener(listener);
  image.Hide();
}

int main(int argc, char **argv) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
