led-matrix-maze-generator := ./led-matrix-maze-generator
rpi-rgb-led-matrix := $(led-matrix-maze-generator)/rpi-rgb-led-matrix
rpi-rgb-led-matrix_repo := git@github.com:jesseDMoore1994/rpi-rgb-led-matrix.git
rpi-rgb-led-matrix_branch := master
rpi-rgb-led-matrix_lib := $(rpi-rgb-led-matrix)/lib
rpi-rgb-led-matrix_inc := $(rpi-rgb-led-matrix)/include
rpi-rgb-led-matrix_libname := rgbmatrix
rpi-rgb-led-matrix_static_lib := $(rpi-rgb-led-matrix_lib)/lib$(rpi-rgb-led-matrix_libname).a
docker := docker
               
executables := maze
output_folder := maze-dist
output := $(addprefix $(output_folder)/, $(executables))
srcs := $(wildcard $(led-matrix-maze-generator)/*.cc)
objects := $(patsubst %.cc,%.o,$(srcs))

docker_interactive := $(shell test -t 0 && echo "-it")
docker_args ?= $(docker_interactive) \
			   -u $(shell id -u):$(shell id -g) \
			   -e "HARDWARE_DESC=$(HARDWARE_DESC)" \
			   -e "CFLAGS=$(CFLAGS)" \
			   -e "CXXFLAGS=$(CXXFLAGS)" \
			   -e "INCDIRS=$(INCDIRS)" \
			   -v $(shell pwd):/work 

build_env =
CROSS_COMPILE ?= y
ifeq ($(CROSS_COMPILE),y)
toolchain := .toolchain_get
DOCKER_TOOLCHAIN ?= dockcross/linux-armv6-lts
build_env += $(docker) run $(docker_args) $(DOCKER_TOOLCHAIN)
else
endif     

INCDIRS=$(rpi-rgb-led-matrix_inc)
inc=$(addprefix -I,$(INCDIRS))
CFLAGS=-W -Wall -Wextra -Wno-unused-parameter -O3 -g -fPIC
CXXFLAGS=$(CFLAGS)
LDFLAGS+=-L$(rpi-rgb-led-matrix_lib) -l$(rpi-rgb-led-matrix_libname) -lrt -lm -lpthread

HARDWARE_DESC ?= 'adafruit-hat'

.PHONY: all deploy clean clean-all

all: $(toolchain) $(rpi-rgb-led-matrix)
	$(build_env) $(MAKE) $(output)

$(objects): %.o: $(srcs)
	$(CXX) $(inc) $(CXXFLAGS) -c -o $@ $*.cc

$(output): $(output_folder)/%: $(objects) $(rpi-rgb-led-matrix_static_lib)
	mkdir -p $(output_folder)
	$(CXX) $(objects) -o $@ $(LDFLAGS)

$(toolchain):
ifeq ($(CROSS_COMPILE),y)
	$(docker) pull $(DOCKER_TOOLCHAIN)
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
		$(shell pwd)/$(output_folder) \
		$(DEPLOY_USERNAME)@$(DEPLOY_HOST):$(DEPLOY_DIR)

clean:
	rm -rf $(output_folder) $(objects)

clean-all:
	rm -rf $(rpi-rgb-led-matrix) $(toolchain) $(output_folder) $(objects)
	bash -c " \
	    docker rmi -f $$(docker images -aq $(DOCKER_TOOLCHAIN)) \
    "
