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

INCDIRS=$(rpi-rgb-led-matrix_inc)
inc=$(addprefix -I,$(INCDIRS))
CFLAGS=-W -Wall -Wextra -Wno-unused-parameter -O3 -g -fPIC
CXXFLAGS=$(CFLAGS)
LDFLAGS+=-L$(rpi-rgb-led-matrix_lib) -l$(rpi-rgb-led-matrix_libname) -lrt -lm -lpthread

HARDWARE_DESC ?= adafruit-hat

.PHONY: all toolshell format deploy clean clean-all

all: $(toolchain) $(rpi-rgb-led-matrix)
	$(build-env) $(MAKE) $(output)

toolshell: $(toolchain) $(rpi-rgb-led-matrix)
	$(build-env) bash

FORMAT_STYLE ?= Google
format: $(toolchain)
	$(build-env) bash -c " \
	    cd $(led-matrix-maze-generator); \
		clang-format -i --style=$(FORMAT_STYLE) *.cc *h; \
	"

$(objects): %.o: $(srcs)
	$(CXX) $(inc) $(CXXFLAGS) -c -o $@ $*.cc

$(output): $(output-folder)/%: $(objects) $(rpi-rgb-led-matrix_static_lib)
	mkdir -p $(output-folder)
	$(CXX) $(objects) -o $@ $(LDFLAGS)

$(toolchain):
ifeq ($(CROSS_COMPILE),y)
	$(docker) build -t $(docker-toolchain) \
		-f Dockerfile.$(MAZE_GENERATOR_DOCKCROSS_BASE) \
		.
	touch $@
else
	touch $@
endif

$(rpi-rgb-led-matrix):
	git clone --branch $(rpi-rgb-led-matrix_branch) \
		$(rpi-rgb-led-matrix_repo) \
		$(rpi-rgb-led-matrix)

$(rpi-rgb-led-matrix_static_lib): $(rpi-rgb-led-matrix) $(toolchain)
	$(MAKE) -C $(rpi-rgb-led-matrix)

DEPLOY_USERNAME ?= pi
DEPLOY_HOST ?= 10.0.0.88
DEPLOY_DIR ?= ~
deploy:
	rsync -avzP \
		$(shell pwd)/$(output-folder) \
		$(DEPLOY_USERNAME)@$(DEPLOY_HOST):$(DEPLOY_DIR)

clean:
	rm -rf $(output-folder) $(objects)

clean-all:
	rm -rf $(rpi-rgb-led-matrix) $(toolchain) $(output-folder) $(objects)
	bash -c " \
	    docker rmi -f $$(docker images -aq $(docker-toolchain)) \
    "
