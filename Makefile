led-matrix-maze-generator := ./led-matrix-maze-generator
rpi-rgb-led-matrix := $(led-matrix-maze-generator)/rpi-rgb-led-matrix
rpi-rgb-led-matrix_repo := git@github.com:jesseDMoore1994/rpi-rgb-led-matrix.git
rpi-rgb-led-matrix_branch := maze-generation
rpi-rgb-led-matrix_lib := $(rpi-rgb-led-matrix)/lib
rpi-rgb-led-matrix_inc := $(rpi-rgb-led-matrix)/include
rpi-rgb-led-matrix_libname := rgbmatrix
rpi-rgb-led-matrix_static_lib := $(rpi-rgb-led-matrix_lib)/lib$(rpi-rgb-led-matrix_libname).a
docker := docker

executables := maze-generator
output-folder := maze-dist
output := $(addprefix $(output-folder)/, $(executables))
srcs := $(wildcard $(led-matrix-maze-generator)/*.cc)
headers := $(patsubst %.cc,%.h,$(srcs))
objects := $(patsubst %.cc,%.o,$(srcs))

docker-interactive := $(shell test -t 0 && echo "-it")
docker-args ?= $(docker-interactive) \
			   -u $(shell id -u):$(shell id -g) \
			   -e "HARDWARE_DESC=$(HARDWARE_DESC)" \
			   -e "CFLAGS=$(CFLAGS)" \
			   -e "CXXFLAGS=$(CXXFLAGS)" \
			   -e "INCDIRS=$(INCDIRS)" \
			   -v $(shell pwd):/work

build-env =
CROSS_COMPILE ?= y
ifeq ($(CROSS_COMPILE),y)
toolchain := .toolchain_get
MAZE_GENERATOR_DOCKCROSS_BASE := linux-armv6-lts
maze-generator-builder-version := 1.0.0
docker-toolchain ?= maze_generator_builder_$(MAZE_GENERATOR_DOCKCROSS_BASE):$(maze-generator-builder-version)
build-env += $(docker) run $(docker-args) $(docker-toolchain)
else
endif

DEBUG ?=
ifneq ($(DEBUG),)
debug-level-opts = -g3 -O0 -ggdb
else
debug-level-opts = -g -O3
endif

INCDIRS=$(rpi-rgb-led-matrix_inc)
inc=$(addprefix -I,$(INCDIRS))
CFLAGS= --std=c++1z -W -Wall -Wextra -Wno-unused-parameter $(debug-level-opts) -fPIC
CXXFLAGS=$(CFLAGS)
LDFLAGS+=-L$(rpi-rgb-led-matrix_lib) -l$(rpi-rgb-led-matrix_libname) -lrt -lm -lpthread

HARDWARE_DESC ?= adafruit-hat

# DEFAULT ALL TARGET

.PHONY: all

all: build test

# TOOLCHAIN TARGETS

$(toolchain):
ifeq ($(CROSS_COMPILE),y)
	$(docker) build -t $(docker-toolchain) \
		-f Dockerfile.$(MAZE_GENERATOR_DOCKCROSS_BASE) \
		.
	touch $@
else
	touch $@
endif

# BUILD TARGETS

.PHONY: build

$(objects): %.o: $(srcs)
	$(CXX) $(inc) $(CXXFLAGS) -c -o $@ $*.cc

$(output): $(output-folder)/%: $(objects) $(rpi-rgb-led-matrix_static_lib)
	mkdir -p $(output-folder)
	$(CXX) $(objects) -o $@ $(LDFLAGS)

$(rpi-rgb-led-matrix):
	git clone --branch $(rpi-rgb-led-matrix_branch) \
		$(rpi-rgb-led-matrix_repo) \
		$(rpi-rgb-led-matrix)

$(rpi-rgb-led-matrix_static_lib): $(rpi-rgb-led-matrix) $(toolchain)
	$(MAKE) -C $(rpi-rgb-led-matrix)

build: $(toolchain) $(rpi-rgb-led-matrix)
	$(build-env) $(MAKE) $(output)

# TEST TARGETS
#
.PHONY: test

testdir = test
test-srcs := $(wildcard $(testdir)/*.cc)
objects-without-main := $(filter-out $(led-matrix-maze-generator)/main.o,$(objects))
test-objects := $(patsubst %.cc,%.o,$(test-srcs))
test-executable := $(addprefix $(output-folder)/, test-maze-generator)
doctest = $(testdir)/doctest.h
doctest-license = $(testdir)/DOCKTEST_LICENSE.txt
DOCTEST_VERSION ?= 2.4.7

TEST_INCDIRS=$(testdir) $(led-matrix-maze-generator)
test-inc=$(inc) $(addprefix -I,$(TEST_INCDIRS))

$(doctest):
	wget -O $@ \
		https://raw.githubusercontent.com/onqtam/doctest/$(DOCTEST_VERSION)/doctest/doctest.h
	wget -O $(doctest-license) \
		https://github.com/doctest/doctest/blob/$(DOCTEST_VERSION)/LICENSE.txt

$(test-objects): %.o: $(test-srcs)
	$(CXX) $(test-inc) $(CXXFLAGS) -c -o $@ $*.cc

$(test-executable): $(output-folder)/%: $(src-objects) $(test-objects) $(doctest)
	mkdir -p $(output-folder)
	$(CXX) $(objects-without-main) $(test-objects) -o $@ $(LDFLAGS)

test: $(output) $(doctest)
	$(build-env) $(MAKE) $(test-executable)
	$(MAKE) deploy
	ssh $(DEPLOY_USERNAME)@$(DEPLOY_HOST) $(led-matrix-maze-generator)/$(test-executable)

# UTILITY TARGETS

.PHONY: toolshell format deploy clean clean-all

toolshell: $(toolchain) $(rpi-rgb-led-matrix)
	$(build-env) bash

FORMAT_STYLE ?= Google
format: $(toolchain)
	$(build-env) bash -c " \
	    cd $(led-matrix-maze-generator); \
		clang-format -i --style=$(FORMAT_STYLE) *.cc *h; \
	    cd -; \
	    cd $(testdir); \
		clang-format -i --style=$(FORMAT_STYLE) *.cc *h; \
	"

DEPLOY_USERNAME ?= pi
DEPLOY_HOST ?= 10.0.0.88
DEPLOY_DIR ?= ~
deploy:
	rsync -avzP \
		$(shell pwd) \
		$(DEPLOY_USERNAME)@$(DEPLOY_HOST):$(DEPLOY_DIR)

clean:
	rm -rf $(output-folder) $(objects) $(test-objects) $(test-output)

clean-all: clean
	rm -rf $(rpi-rgb-led-matrix) $(toolchain) $(output-folder) $(objects) \
		$(doctest) $(doctest-license)
	bash -c " \
	    docker rmi -f $$(docker images -aq $(docker-toolchain)) \
    "
