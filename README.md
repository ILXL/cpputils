# C++ Utils

Libraries for student C++ code.

## Graphics

graphics/image.h can be used to create, view, load and save images. Example:

![example fractal tree](graphics/example_fractal_tree.png)

### TODOs:
1. Set up continuous integration testing of image_unittest
2. Update this README with some instructions and a link to documentation for graphics.

## Usage (for project creators):

To use this as a submodule in another lab or project, run:

```
git submodule add https://github.com/ILXL/cpputils
```

This will create a .gitmodules file which you can add and commit.

You can choose to watch a particular branch so that any changes pushed to
cpputils/master do not impact your project:

```
git config -f .gitmodules submodule.cpputils.branch v1
```

Then make sure everyone who checks out the repo gets the updates, for example adding this
command to your project Makefile to check if image.h is available:

```
cpputils/graphics/image.h:
	@git submodule update --init --recursive
```
