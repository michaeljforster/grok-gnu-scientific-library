// Standard C headers
#include <assert.h>
#include <libgen.h> // basename()
#include <unistd.h> // getops()

// Standard C++ headers
#include <cmath>
#include <ctime>
#include <fstream>
#include <iomanip>
#include <iostream>

// Third-party C/C++ headers
#include <gsl/gsl_errno.h> // gsl_strerror(), GSL_SUCCESS, etc.
#include <gsl/gsl_integration.h>

// Project headers
#include "quad2d.h"

//
// 1. The 2D function to integrate.
//

double f (double x, double y, double z)
{
  return exp(-(x+y)) * z;
}

//
// 2. Adapt the function for quad2d
//

double f_2d (double x, double y, void * params)
{
  double z = *(double *)params;
  return f(x, y, z);
}

//
// 3. The ymin and ymax functions.
//

double ymin(double x)
{
  return 0; //constant for now
}

double ymax(double x)
{
  return x;
}

//
// 4. The program.
//

void cerr_usage(const char *command_name) {
  std::cerr << "usage: "
	    << command_name
	    << " -H [-A epsabs] [-R epsrel] [-L limit] [-K 1|2|3|4|5|6] [-x xmin] input_file"
	    << std::endl;
}

int main (int argc, char* argv[])
{
  const char *command_name = basename(argv[0]);

  int option_print_header = 0;
  double epsabs = 5.0e-3;
  double epsrel = 5.0e-3;
  size_t limit = 1000;
  int key = GSL_INTEG_GAUSS31; // 3
  double xmin = 1.0;

  //
  // Get command-line options.
  //

  int opt;
  
  while ((opt = getopt(argc, argv, "HA:R:L:K:x:")) != -1) {
    switch (opt) {
    case 'H':
      option_print_header = 1;
      break;
    case 'A':
      epsabs = atof(optarg);
      break;
    case 'R':
      epsrel = atof(optarg);
      break;
    case 'L':
      limit = atoi(optarg);
      break;
    case 'K':
      key = atoi(optarg);
      if (!((key == GSL_INTEG_GAUSS15) ||
	    (key == GSL_INTEG_GAUSS21) ||
	    (key == GSL_INTEG_GAUSS31) ||
	    (key == GSL_INTEG_GAUSS41) ||
	    (key == GSL_INTEG_GAUSS51) ||
	    (key == GSL_INTEG_GAUSS61))) {
	cerr_usage(command_name);
	exit(EXIT_FAILURE);
      }
      break;
    case 'x':
      xmin = atof(optarg);
      break;
    default:
      cerr_usage(command_name);
      exit(EXIT_FAILURE);
    }
  }
  
  //
  // OPTIONAL: Get command-line arguments following options.
  //

  // if (optind >= argc) {
  //   std::cerr << "Expected argument after options." << std::endl;
  //   cerr_usage(command_name);
  //   exit(EXIT_FAILURE);
  // }

  //
  // Process.
  //

  // OPTIONAL: char *filename = argv[optind];
  // process input file ...

  double result;

  double z = 42.1;
  
  quad2d_function FXY;
  FXY.function = &f_2d;
  FXY.params = &z;

  int status = quad2d(&FXY, xmin, &ymin, &ymax, epsabs, epsrel, limit, key, &result);

  if (status != GSL_SUCCESS) {
    std::cerr << "FAILED: " << gsl_strerror(status) << " (" << status << ")" << std::endl;
    exit(EXIT_FAILURE);
  }  
  
  //
  // Output parameters and results.
  //

  // ISO 8601 date/time format
  const char *fmt_iso_8601 = "%FT%T%z";
  
  std::time_t timestamp_tt = std::time(nullptr);
  std::tm timestamp_tm = *std::localtime(&timestamp_tt);

  if (option_print_header == 1) {
    std::cout << std::left     // column alignment
	      << "timestamp" << '\t'
	      << "command" << '\t'
	      << "xmin" << '\t'
	      << "epsabs" << '\t'
	      << "epsrel" << '\t'
	      << "limit" << '\t'
	      << "key" << '\t'
	      << "result" << std::endl;
  }
  
  std::cout << std::left
	    << std::put_time(&timestamp_tm, fmt_iso_8601) << '\t'
	    << command_name << '\t'
	    << xmin << '\t'
	    << epsabs << '\t'
	    << epsrel << '\t'
	    << limit << '\t'
	    << key << '\t'
	    << std::scientific << result << std::endl;

  exit(EXIT_SUCCESS);
}
