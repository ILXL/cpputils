.PHONY: update_cimg image_unittest

update_cimg:
	@echo -e "Getting CImg..."
	@wget -q https://raw.githubusercontent.com/dtschump/CImg/master/CImg.h
	@mv CImg.h ../cimg/
	@echo -e "CImg updated"

/usr/lib/libgtest.a:
	@echo -e "google test library not installed\n"
	@echo -e "Installing cmake. Please provide the password when asked\n"
	@sudo apt-get install cmake # install cmake
	@echo -e "\nDownloading and installing googletest\n"
	@cd /tmp/; git clone https://github.com/google/googletest.git; cd googletest; cmake CMakeLists.txt; make; sudo cp -r googletest/include/. /usr/include; sudo cp -r googlemock/include/. /usr/include; sudo cp lib/*.a /usr/lib
	@echo -e "Finished installing google test library\n"

image_unittest: /usr/lib/libgtest.a
	@clang++ -std=c++17 ../image.cc image_unittest.cc -o image_unittest -pthread -lgtest -lm -lX11 -lpthread && ./image_unittest
