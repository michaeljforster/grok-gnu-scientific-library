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
#include <gsl/gsl_integration.h>


////////////////////////////////////////////////////////////////////////
//
// The 2-ary function to be integrated, f(x, y).
//

double f(double x, double y)
{
  return exp(-(x+y));
}
////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////
//
// QUAD2D Implementation
//
// TODO extract to a library.
//

//
// If C++ supported true lambda expressions with closure (it cannot
// because it cannot solve the "upwards funargs" problem), we could
// write something like:
//
//    xval = 3;
//    integrand.function = &((y) -> f(xval, y))
//
// Option 1: One work-around for this limitation is to declare a
// global variable and wrapper function referencing the global as a
// free variable. See "Option 1" in quad1d().
//

/*
  double xval = 1;

  double f_y(double y, void *params) 
  {
  return f(xval, y);
  }
*/

// Option 2: Another way is to pass the x value via the "params"
// parameter. See "Option 2" in quad1d().
double f_y(double y, void * params)
{
  double x = *(double *) params;
  return f(x, y);
}

//
// Integrate f_y(y) from y = ymin(x) to ymax(x) where f_y(y) = f(x, y)
//

double ymin(double x)
{
  return 0; //constant for now
}

double ymax(double x)
{
  return x;
}

struct quad1d_params {
  double epsabs;
  double epsrel;
  size_t limit;
  int key;
};

double quad1d(double x, void *params)
{
  struct quad1d_params *p = (struct quad1d_params *)params;
  double epsabs = p->epsabs;
  double epsrel = p->epsrel;
  size_t limit = p->limit;
  int key = p->key;

  double result;	
  double abserr;

  gsl_function integrand;
  integrand.function = &f_y;

  // Option 1: f_y(y) = f(x, y)
  // xval = x;

  // Option 2: f_y(y) = f(x, y)
  integrand.params = &x;

  gsl_integration_workspace *workspace;

  // TODO why 1000?  related to limit?
  workspace = gsl_integration_workspace_alloc(1000);
  
  // int gsl_integration_qag(const gsl_function *f, double a, double b, double epsabs, double epsrel, size_t limit, int key,
  //                         gsl_integration_workspace *workspace, double *result, double *abserr)
  int gsl_result = gsl_integration_qag(&integrand,
				       ymin(x),
				       ymax(x),
				       epsabs,
				       epsrel,
				       limit,
				       key,
				       workspace,
				       &result,
				       &abserr);

  assert(gsl_result == 0);
  
  // TODO workspace intervals?
  std::cerr << "QUAD1D INTERVALS: " << workspace->size << std::endl;

  // TODO quad1d abserr?
  std::cerr << "QUAD1D ABSERR: " << std::scientific << abserr << std::endl;

  gsl_integration_workspace_free(workspace);

  return result;
}

//
// Integrate f(x, y) from x = xmin to x = infinity and y = ymin(x) to y = ymax(x)
//

double quad2d(double xmin, double epsabs, double epsrel, size_t limit, int key, double *result, double *abserr)
{
  gsl_function integrand;
  integrand.function = &quad1d;
  struct quad1d_params quad1d_p = { epsabs, epsrel, limit, key };
  integrand.params = &quad1d_p;

  gsl_integration_workspace *workspace;

  // TODO why 1000?  related to limit?
  workspace = gsl_integration_workspace_alloc(1000);

  // int gsl_integration_qagiu(gsl_function *f, double a, double epsabs, double epsrel, size_t limit,
  //                           gsl_integration_workspace *workspace, double *result, double *abserr)
  int gsl_result = gsl_integration_qagiu(&integrand,
					 xmin,
					 epsabs,
					 epsrel,
					 limit,
					 workspace,
					 result,
					 abserr);

  // TODO workspace intervals?
  std::cerr << "QUAD2D INTERVALS: " << workspace->size << std::endl;
  
  gsl_integration_workspace_free(workspace);

  return gsl_result;
}
////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////
//
// Main for this program.
//

void cerr_usage(const char *command_name) {
  std::cerr << "USAGE: " << command_name << " -h [-x xmin] [-a epsabs] [-r epsrel] [-l limit] [-k key] input_file" << std::endl;
}

int main (int argc, char* argv[])
{
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
  
  while ((opt = getopt(argc, argv, "hx:a:r:l:k:")) != -1) {
    switch (opt) {
    case 'h':
      option_print_header = 1;
      break;
    case 'x':
      xmin = atof(optarg);
      break;
    case 'a':
      epsabs = atof(optarg);
      break;
    case 'r':
      epsrel = atof(optarg);
      break;
    case 'l':
      limit = atoi(optarg);
      break;
    case 'k':
      key = atoi(optarg);
      break;
    default:
      cerr_usage(argv[0]);
      exit(EXIT_FAILURE);
    }
  }
  
  //
  // OPTIONAL: Get command-line arguments following options.
  //

  // if (optind >= argc) {
  //   std::cerr << "Expected argument after options." << std::endl;
  //   cerr_usage(argv[0]);
  //   exit(EXIT_FAILURE);
  // }

  //
  // Process.
  //

  // OPTIONAL: char *filename = argv[optind];
  // process input file ...

  double result;
  double abserr;

  int gsl_result = quad2d(xmin, epsabs, epsrel, limit, key, &result, &abserr);

  assert(gsl_result == 0);

  std::cerr << "QUAD2D ABSERR: " << std::scientific << abserr << std::endl;
  
  //
  // Output parameters and results.
  //

  const char *command_name = basename(argv[0]);

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
