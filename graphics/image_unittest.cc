#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <string>

#include "image.h"

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
  image.Show("Test image");

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

int main(int argc, char **argv) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
