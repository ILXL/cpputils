# Copyright 2020 Paul Salvador Inventado and Google LLC
#
# Use of this source code is governed by an MIT-style
# license that can be found in the LICENSE file or at
# https://opensource.org/licenses/MIT.

.PHONY: install_gtest karel_unittest

OS_NAME 				:= $(shell uname -s | tr A-Z a-z)
SHELL         		:= /bin/bash
HAS_GTEST       := $(shell echo -e "int main() { }" >> test.cc ; clang++ test.cc -o test -lgtest 2> /dev/null; echo $$?; rm -f test.cc test;)
COMPILE_FLAGS 	:= -lm -lX11 -lpthread

ifeq ($(OS_NAME), darwin)
	COMPILE_FLAGS	:= -lm -lpthread -lX11 -I/usr/X11R6/include -L/usr/X11R6/lib
	HAS_BREW	:= $(shell command -v brew 2> /dev/null)
endif

install_gtest:
ifeq ($(HAS_GTEST),1)
	@echo -e "google test not installed\n"
ifeq ($(OS_NAME), darwin)
ifndef ($(HAS_BREW))
	@echo -e "Installing brew, please provide your password when asked.\n"
	@/bin/bash -c "$$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/master/install.sh)"
	@brew install coreutils
	# Need to redefine the path directories otherwise this will fail the first time.
endif
	@echo -e "Installing cmake. Please provide the password when asked\n"
	@brew install cmake
	@echo -e "\nDownloading and installing googletest\n"
	@cd /tmp/; git clone https://github.com/google/googletest; cd googletest; mkdir build; cd build; cmake .. -DCMAKE_CXX_STANDARD=17; make; make install
	@echo -e "Finished installing google test\n"
else
ifneq ($(shell lsb_release -sr), 20.04)
	@cd /tmp/; git clone https://github.com/google/googletest.git; cd googletest; cmake CMakeLists.txt; make; sudo cp -r googletest/include/. /usr/include; sudo cp -r googlemock/include/. /usr/include; sudo cp lib/*.a /usr/lib
else
	@echo -e "Installing cmake. Please provide the password when asked\n"
	@sudo apt-get install cmake # install cmake
	@echo -e "\nDownloading and installing googletest\n"
	@sudo apt-get install libgtest-dev libgmock-dev
	@echo -e "Finished installing google test\n"
endif
endif
endif

karel_unittest: install_gtest
	@clang++ -std=c++17 ../../../graphics/image.cc ../robot.cc ../../karel.cc karel_unittest.cc -o karel_unittest -pthread -lgtest $(COMPILE_FLAGS) && ./karel_unittest
