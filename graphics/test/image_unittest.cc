// Copyright 2020 Paul Salvador Inventado and Google LLC
//
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

#include "../image.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <string>

#include "image_test_utils.h"
#include "test_event_generator.h"

TEST(ImageTest, CreatesImageWithoutCrashing) {
  graphics::Image image(100, 100);
}

TEST(ImageTest, LoadsImageWithoutCrashing) {
  graphics::Image image;
  image.Load("example_bmp.bmp");
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

TEST(ColorTest, ColorOperators) {
  graphics::Color black(0, 0, 0);
  graphics::Color red(255, 0, 0);
  ASSERT_NE(black, red);
  ASSERT_EQ(red, graphics::Color(255, 0, 0));

  graphics::Color red_copy = red;
  ASSERT_EQ(red, red_copy);

  graphics::Color& red_ref = red;
  red_ref.SetBlue(255);
  ASSERT_EQ(red.Blue(), 255);
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
  graphics::Color red(255, 0, 0);
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
  std::vector<int> points = {20, 20, 20, 22, 22, 21};
  image.DrawPolygon(points, red);
  EXPECT_EQ(image.GetColor(20, 21), red);
  EXPECT_EQ(image.GetColor(21, 21), red);  

  // Drawing something out of bounds doesn't work.
  image.DrawRectangle(-1, -1, 50, 50, 0, 255, 0);
  EXPECT_EQ(image.GetColor(0, 0), white);

  image.DrawCircle(40, 50, 100, 0, 255, 0);
  EXPECT_EQ(image.GetColor(0, 0), white);

  std::vector<int> out_points = {-1, 0, 0, 0, -2, 0};
  image.DrawPolygon(out_points, red);
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

  // Use an odd size thickness. Even size thickness isn't evenly divisible
  // by 2, meaning we can't create a rectangle of the right size.
  int thickness = 21;
  int size = 100;
  graphics::Color blue(0, 0, 255);

  graphics::Image expected(size, size);
  graphics::Image actual(size, size);

  // Horizontal rectangle is the same as a thick line.
  expected.DrawRectangle(10, 40, 81, thickness, blue);
  actual.DrawLine(10, 50, 90, 50, blue, thickness);
  EXPECT_TRUE(ImagesMatch(&expected, &actual,
                          "DrawsLinesWithThicknessHorizontal.bmp",
                          DiffType::kTypeHighlight));

  // Vertical rectangle is the same as a thick line.
  expected.DrawRectangle(40, 5, thickness, 91, blue);
  actual.DrawLine(50, 5, 50, 95, blue, thickness);
  EXPECT_TRUE(ImagesMatch(&expected, &actual,
                          "DrawsLinesWithThicknessVertical.bmp",
                          DiffType::kTypeHighlight));

  // 45 degree rectangle is as thick as expected. Check some points that
  // wouldn't be colored unless the line had appropriate thickness.
  graphics::Color green(0, 255, 0);
  actual.DrawLine(0, 0, size - 1, size - 1, green, thickness);
  EXPECT_EQ(actual.GetColor(thickness / 2, 0), green);
  EXPECT_EQ(actual.GetColor(0, thickness / 2), green);
  EXPECT_EQ(actual.GetColor(size / 2, size / 2 - thickness / 2), green);
  EXPECT_EQ(actual.GetColor(size / 2, size / 2 + thickness / 2), green);

  // But not too wide!
  EXPECT_NE(actual.GetColor(size / 2,
                            size / 2 + std::sqrt(2 * thickness * thickness)),
            green);
}

TEST(ImageTest, DrawsLinesWithThicknessOrderDoesntMatter) {
  remove("DrawsLinesWithThicknessOrderDiagonal1.bmp");
  remove("DrawsLinesWithThicknessOrderDiagonal2.bmp");
  remove("DrawsLinesWithThicknessOrderDiagonal3.bmp");
  remove("DrawsLinesWithThicknessOrderDiagonal4.bmp");
  remove("DrawsLinesWithThicknessOrderHorizontal.bmp");
  remove("DrawsLinesWithThicknessOrderVertical.bmp");

  int thickness = 10;
  int size = 100;
  graphics::Color blue(0, 0, 255);
  graphics::Color green(0, 255, 0);
  graphics::Color black(0, 0, 0);

  graphics::Image expected(size, size);
  graphics::Image actual(size, size);

  // Expected has smaller x0 than x1, smaller y0 than y1.
  expected.DrawLine(20, 31, 17, 80, blue, thickness);
  actual.DrawLine(17, 80, 20, 31, blue, thickness);
  ASSERT_TRUE(ImagesMatch(&expected, &actual,
                          "DrawsLinesWithThicknessOrderDiagonal1.bmp",
                          DiffType::kTypeHighlight));

  // Expected has bigger x0 than x1, smaller y0 than y1.
  expected.DrawLine(77, 20, 80, 80, black, thickness);
  actual.DrawLine(80, 80, 77, 20, black, thickness);
  ASSERT_TRUE(ImagesMatch(&expected, &actual,
                          "DrawsLinesWithThicknessOrderDiagonal2.bmp",
                          DiffType::kTypeHighlight));

  // Expected has smaller x0 than x1, bigger y0 than y1.
  expected.DrawLine(10, 78, 80, 13, black, thickness);
  actual.DrawLine(80, 13, 10, 78, black, thickness);
  ASSERT_TRUE(ImagesMatch(&expected, &actual,
                          "DrawsLinesWithThicknessOrderDiagonal3.bmp",
                          DiffType::kTypeHighlight));

  // Expected has bigger x0 than x1, bigger y0 than y1.
  expected.DrawLine(57, 80, 80, 20, blue, thickness);
  actual.DrawLine(80, 20, 57, 80, blue, thickness);
  ASSERT_TRUE(ImagesMatch(&expected, &actual,
                          "DrawsLinesWithThicknessOrderDiagonal4.bmp",
                          DiffType::kTypeHighlight));

  // Horizontal rectangle is the same as a thick line.
  expected.DrawLine(10, 50, 90, 50, green, thickness);
  actual.DrawLine(90, 50, 10, 50, green, thickness);
  ASSERT_TRUE(ImagesMatch(&expected, &actual,
                          "DrawsLinesWithThicknessOrderHorizontal.bmp",
                          DiffType::kTypeHighlight));

  // Vertical rectangle is the same as a thick line.
  expected.DrawLine(50, 5, 50, 95, black, thickness);
  actual.DrawLine(50, 95, 50, 5, black, thickness);
  ASSERT_TRUE(ImagesMatch(&expected, &actual,
                          "DrawsLinesWithThicknessOrderVertical.bmp",
                          DiffType::kTypeHighlight));
}

class TestEventListener : public graphics::MouseEventListener {
 public:
  TestEventListener() = default;
  ~TestEventListener() = default;
  void OnMouseEvent(const graphics::MouseEvent& event) override {
    latest_event_ = graphics::MouseEvent(event.GetX(), event.GetY(),
                                         event.GetMouseAction());
  }

  graphics::MouseEvent GetLatestEvent() { return latest_event_; }

 private:
  graphics::MouseEvent latest_event_ =
      graphics::MouseEvent(0, 0, graphics::MouseAction::kReleased);
};

// We can send fake events if we have a reference to the image on which to send
// events, and we are not in the ShowUntilClosed loop.
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
  EXPECT_EQ(listener.GetLatestEvent().GetMouseAction(),
            graphics::MouseAction::kPressed);

  generator.MoveMouseTo(30, 80);
  EXPECT_EQ(listener.GetLatestEvent().GetX(), 30);
  EXPECT_EQ(listener.GetLatestEvent().GetY(), 80);
  EXPECT_EQ(listener.GetLatestEvent().GetMouseAction(),
            graphics::MouseAction::kDragged);

  generator.MoveMouseTo(90, 80);
  EXPECT_EQ(listener.GetLatestEvent().GetX(), 90);
  EXPECT_EQ(listener.GetLatestEvent().GetY(), 80);
  EXPECT_EQ(listener.GetLatestEvent().GetMouseAction(),
            graphics::MouseAction::kDragged);

  generator.MouseUp();
  EXPECT_EQ(listener.GetLatestEvent().GetX(), 90);
  EXPECT_EQ(listener.GetLatestEvent().GetY(), 80);
  EXPECT_EQ(listener.GetLatestEvent().GetMouseAction(),
            graphics::MouseAction::kReleased);

  generator.MoveMouseTo(30, 30);
  // Mouse moved event.
  EXPECT_EQ(listener.GetLatestEvent().GetX(), 30);
  EXPECT_EQ(listener.GetLatestEvent().GetY(), 30);
  EXPECT_EQ(listener.GetLatestEvent().GetMouseAction(),
            graphics::MouseAction::kMoved);

  // Ensure other mouse buttons don't interfere.
  generator.MouseDown(10, 20);
  generator.RightMouseDown();
  generator.RightMouseUp();
  EXPECT_EQ(listener.GetLatestEvent().GetX(), 10);
  EXPECT_EQ(listener.GetLatestEvent().GetY(), 20);
  EXPECT_EQ(listener.GetLatestEvent().GetMouseAction(),
            graphics::MouseAction::kPressed);
  generator.RightMouseDown();
  generator.MouseUp();
  EXPECT_EQ(listener.GetLatestEvent().GetMouseAction(),
            graphics::MouseAction::kReleased);

  image.RemoveMouseEventListener(listener);
  image.Hide();
}

class TestAnimationEventListener : public graphics::AnimationEventListener {
 public:
  TestAnimationEventListener() = default;
  ~TestAnimationEventListener() = default;
  void OnAnimationStep() override { num_events_++; }

  int GetNumEvents() { return num_events_; }

 private:
  int num_events_ = 0;
};

TEST(AnimationEventTest, CallsAnimationListeners) {
  TestAnimationEventListener listener;
  ASSERT_EQ(0, listener.GetNumEvents());
  graphics::Image image(50, 50);
  graphics::TestEventGenerator generator(&image);
  image.Show();

  image.AddAnimationEventListener(listener);
  ASSERT_EQ(0, listener.GetNumEvents());
  generator.SendAnimationEvent();
  ASSERT_EQ(1, listener.GetNumEvents());
  generator.SendAnimationEvent();
  ASSERT_EQ(2, listener.GetNumEvents());
  image.RemoveAnimationEventListener(listener);

  // Actually removed.
  generator.SendAnimationEvent();
  ASSERT_EQ(2, listener.GetNumEvents());
}

int main(int argc, char** argv) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
