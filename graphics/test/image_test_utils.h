// Copyright 2020 Paul Salvador Inventado and Google LLC
//
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

#include <iostream>
#include <string>

#include "../image.h"

#ifndef IMAGE_TEST_UTILS_H
#define IMAGE_TEST_UTILS_H

enum DiffType {
  kTypeOverlay,
  kTypeHighlight,
  kTypeSideBySide,
};

/*
 * Returns true if the image |expected| and |actual| are a pixel perfect
 * match. If they are not a perfect match, saves a new image in |output_file|
 * depending on the |diff_type| chosen, and returns false.
 */
bool ImagesMatch(graphics::Image* expected, graphics::Image* actual,
                 std::string output_file, DiffType diff_type) {
  int width = expected->GetWidth();
  int height = expected->GetHeight();
  if (width != actual->GetWidth() || height != actual->GetHeight()) {
    std::cout << "Images are different dimensions. Expected: " << width
              << " by " << height << "px" << std::endl;
    return false;
  }

  // Create the output image. If we want a side-by-side comparison, it
  // has twice the width.
  graphics::Image result(diff_type == kTypeSideBySide ? width * 2 : width,
                         height);
  bool matching = true;
  for (int i = 0; i < width; i++) {
    for (int j = 0; j < height; j++) {
      graphics::Color c_actual = actual->GetColor(i, j);
      graphics::Color c_expected = expected->GetColor(i, j);
      if (c_actual.Red() != c_expected.Red() ||
          c_actual.Green() != c_expected.Green() ||
          c_actual.Blue() != c_expected.Blue()) {
        matching = false;
        if (diff_type == kTypeHighlight) {
          // Saturate the red in the result where the channels
          // differ. This is good if the diff is likely to be
          // small or in one particular region.
          graphics::Color red(255, 0, 0);
          result.SetColor(i, j, red);
        } else if (diff_type == kTypeOverlay) {
          // Overlay by averaging pixels. This is good for comparing
          // line drawings or shapes, but harder to look at when
          // comparing pictures.
          result.SetRed(i, j, (c_actual.Red() + c_expected.Red()) / 2);
          result.SetGreen(i, j, (c_actual.Green() + c_expected.Green()) / 2);
          result.SetBlue(i, j, (c_actual.Blue() + c_expected.Blue()) / 2);
        }
      } else if (diff_type != kTypeSideBySide) {
        result.SetColor(i, j, c_actual);
      }
      if (diff_type == kTypeSideBySide) {
        result.SetColor(i + width, j, c_actual);
        result.SetColor(i, j, c_expected);
      }
    }
  }

  if (matching) return true;
  std::cout << "Images do not match. See " << output_file << " for diff."
            << std::endl;
  result.SaveImageBmp(output_file);
  return false;
}

/*
 * Returns true if the file in |expected_file| and in |actual_file|
 * are a pixel perfect match. If they are not a perfect match, saves
 * a new image in |output_file| depending on the |diff_type| chosen,
 * and returns false.
 */
bool ImagesMatch(std::string expected_file, std::string actual_file,
                 std::string output_file, DiffType diff_type) {
  graphics::Image expected;
  if (!expected.Load(expected_file)) return false;
  graphics::Image actual;
  if (!actual.Load(actual_file)) return false;

  bool result = ImagesMatch(&expected, &actual, output_file, diff_type);
  return result;
}

#endif  // IMAGE_TEST_UTILS_H
