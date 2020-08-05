.PHONY: update_cimg image_unittest

update_cimg:
	@echo -e "Getting CImg..."
	@wget -q https://raw.githubusercontent.com/dtschump/CImg/master/CImg.h
	@mv CImg.h cimg/
	@echo -e "CImg updated"

image_unittest:
	@clang++ -std=c++17 image.cc image_unittest.cc -o image_unittest -pthread -lgtest -lm -lX11 -lpthread && ./image_unittest
