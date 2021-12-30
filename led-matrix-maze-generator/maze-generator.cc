#include "led-matrix.h"
#include <exception>
#include <signal.h>
#include <time.h>
#include <unistd.h>

#include "direction.h"
#include "cell.h"
#include "grid.h"
#include "maze.h"

// TODO: Make ROWS and COLS variable, and make
// GRID_WIDTH and GRID_HEIGHT a function of
// ROWS and COLS.
#define ROWS 64
#define COLS 64
#define GRID_WIDTH 32
#define GRID_HEIGHT 32

/* -- INTERRUPT HANDLING FUNCTION --*/
volatile bool interrupt_received = false;
static void InterruptHandler(int signo) {
  interrupt_received = true;
}

/* -- DRIVER FUNCTION == */
int main(int argc, char **argv) {
  srand(time(NULL));

  rgb_matrix::RGBMatrix::Options led_options;
  rgb_matrix::RuntimeOptions runtime;
  // Set defaults
  led_options.hardware_mapping = "adafruit-hat";
  led_options.chain_length = 1;
  //led_options.show_refresh_rate = true;
  led_options.rows = ROWS;
  led_options.cols = COLS;
  runtime.drop_privileges = 1;
  if (!rgb_matrix::ParseOptionsFromFlags(&argc, &argv, &led_options, &runtime)) {
    rgb_matrix::PrintMatrixFlags(stderr);
    return 1;
  }
  // Looks like we're ready to start
  rgb_matrix::RGBMatrix *matrix = rgb_matrix::RGBMatrix::CreateFromOptions(led_options, runtime);
  if (matrix == NULL) {
    return 1;
  }
  rgb_matrix::Canvas *canvas = matrix;

  // register interrupts
  signal(SIGTERM, InterruptHandler);
  signal(SIGINT, InterruptHandler);

  //  .. now use matrix
  while(!interrupt_received) {
      Maze m(canvas, GRID_HEIGHT, GRID_WIDTH);
      usleep(10*1000000);
  }

  //Clear the matrix and remove the resources that are used
  canvas->Clear();
  delete canvas;
  canvas = nullptr;
  matrix = nullptr;
  return 0;
}
