#include <getopt.h>
#include <signal.h>
#include <time.h>
#include <unistd.h>

#include <chrono>
#include <cstdlib>
#include <exception>
#include <iostream>
#include <tuple>

#include "cell.h"
#include "hunt-and-kill.h"
#include "led-matrix.h"
#include "maze.h"

#define DEFAULT_ROWS 64
#define DEFAULT_COLS 64

/* -- INTERRUPT HANDLING FUNCTION --*/
volatile bool interrupt_received = false;
static void InterruptHandler(int signo) { interrupt_received = true; }

static void usage(const char *progname, const rgb_matrix::RGBMatrix::Options &d,
                  const rgb_matrix::RuntimeOptions &r) {
  fprintf(stderr, "usage: %s [options]\n", progname);
  fprintf(stderr, "This program draws mazes on an LED Matrix.\n");
  fprintf(stderr, "Options:\n");
  fprintf(stderr, "\t-h                        : shows this help dialog.\n");
  fprintf(stderr, "\n");
  rgb_matrix::PrintMatrixFlags(stderr, d, r);
}

rgb_matrix::Canvas *init_canvas_from_opts(int argc, char **argv) {
  srand(time(NULL));

  rgb_matrix::RGBMatrix::Options led_options;
  rgb_matrix::RuntimeOptions runtime;

  // Set defaults
  led_options.hardware_mapping = "adafruit-hat";
  led_options.chain_length = 1;
  led_options.rows = DEFAULT_ROWS;
  led_options.cols = DEFAULT_COLS;
  runtime.drop_privileges = 1;

  int opt;
  while ((opt = getopt(argc, argv, "h:")) != -1) {
    switch (opt) {
      case 'h':
        usage(argv[0], led_options, runtime);
        exit(0);
      default:
        usage(argv[0], led_options, runtime);
        exit(1);
    }
  }
  if (!rgb_matrix::ParseOptionsFromFlags(&argc, &argv, &led_options,
                                         &runtime)) {
    usage(argv[0], led_options, runtime);
    exit(1);
  }
  // Looks like we're ready to start
  rgb_matrix::RGBMatrix *matrix =
      rgb_matrix::RGBMatrix::CreateFromOptions(led_options, runtime);
  if (matrix == NULL) {
    exit(1);
  }
  rgb_matrix::Canvas *canvas = matrix;
  return canvas;
}

/* -- DRIVER FUNCTION == */
int main(int argc, char **argv) {
  // register interrupts
  signal(SIGTERM, InterruptHandler);
  signal(SIGINT, InterruptHandler);

  rgb_matrix::Canvas *canvas = init_canvas_from_opts(argc, argv);

  //  .. now use canvas
  while (!interrupt_received) {
    // Create a maze and tell it to generate
    Maze<HuntAndKillStrategy<>, int > m(canvas);
    while (!interrupt_received && !m.generated) {
      float sleep_time_secs = m.generateStep();
      m.updatePixelMap();
      usleep(sleep_time_secs * 1000000);
    }
    // Transition to solving mode
    usleep(10 * 1000000);
    m.swapToSolvingMode();
    usleep(10 * 1000000);
  }

  // Clear the canvas and remove the resources that are used
  canvas->Clear();
  delete canvas;
  return 0;
}
