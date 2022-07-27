// Standard C headers
#include <assert.h>
#include <libgen.h> // basename()

// Standard C++ headers
#include <chrono>
#include <cmath>
#include <ctime>
#include <fstream>
#include <iomanip>
#include <iostream>

// Third-party C/C++ headers
#include <gsl/gsl_integration.h>

// Globals parameters, declared as constant expressions.
constexpr int key = GSL_INTEG_GAUSS31; // 3
constexpr size_t limit = 1000;
constexpr double epsabs = 5.0e-3;
constexpr double epsrel = 5.0e-3;

//
// The 2-ary function to be integrated, f(x, y).
//

double f(double x, double y)
{
  return exp(-(x+y));
}

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

double quad1d(double x, void *params)
{
  double result;	
  double abserr;

  // Option 1: f_y(y) = f(x, y)
  // xval = x;

  gsl_function integrand;
  integrand.function = &f_y;

  // Option 2: f_y(y) = f(x, y)
  integrand.params = &x;

  gsl_integration_workspace *workspace;
  workspace = gsl_integration_workspace_alloc(1000);
  
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
  
  // cout << "QUAD1D INTERVALS: " << workspace->size << std::endl;
  // cout << "QUAD1D ABSERR: " << std::scientific << abserr << std::endl;

  gsl_integration_workspace_free(workspace);

  return result;
}

//
// Integrate f(x, y) from x = xmin to x = infinity and y = ymin(x) to y = ymax(x)
//

double quad2d(double xmin)
{
  double result;	
  double abserr;	

  gsl_function integrand;
  integrand.function = &quad1d;

  gsl_integration_workspace *workspace;
  workspace = gsl_integration_workspace_alloc(1000);
  
  int gsl_result = gsl_integration_qagiu(&integrand,
					 xmin,
					 epsabs,
					 epsrel,
					 limit,
					 workspace,
					 &result,
					 &abserr);
  
  assert(gsl_result == 0);

  // cout << "QUAD2D INTERVALS: " << workspace->size << std::endl;
  // cout << "QUAD2D ABSERR: " << std::scientific << abserr << std::endl;
  
  gsl_integration_workspace_free(workspace);

  return result;
}

int main (int argc, char* argv[])
{
  //
  // Capture and validate input.
  //

  if (argc < 3) {
    std::cerr << "USAGE: " << argv[0] << " PRINT_HEADER(Y/N) XMIN" << std::endl;
    return 1;
  }

  std::string str_y ("y");
  std::string str_Y ("Y");
  std::string str_n ("n");
  std::string str_N ("N");

  char *command_name = basename(argv[0]);
  char *print_header = argv[1];
  double xmin = atof(argv[2]);
  
  if (str_y.compare(print_header) != 0 && str_Y.compare(print_header) != 0
      && str_n.compare(print_header) != 0 && str_N.compare(print_header) != 0) {
    std::cerr << "USAGE: " << argv[0] << " PRINT_HEADER(Y/N) XMIN" << std::endl;
    return 1;
  }
  
  //
  // Process.
  //
  
  double result = quad2d(xmin);

  //
  // Output parameters and results.
  //

  std::chrono::system_clock::time_point timestamp = std::chrono::system_clock::now();
  time_t timestamp_tt = std::chrono::system_clock::to_time_t(timestamp);
  std::tm timestamp_tm = *std::localtime(&timestamp_tt);
  
  if (str_y.compare(print_header) == 0 || str_Y.compare(print_header) == 0) {
    std::cout << std::left     // column alignment
	      << "timestamp" << '\t'
	      << "command" << '\t'
	      << "key" << '\t'
	      << "limit" << '\t'
	      << "epsabs" << '\t'
	      << "epsrel" << '\t'
	      << "xmin" << '\t'
	      << "result" << std::endl;
  }
  
  std::cout << std::left
	    << std::put_time(&timestamp_tm, "%FT%T%z") << '\t'  // ISO 8601 date/time format
	    << command_name << '\t'
	    << key << '\t'
	    << limit << '\t'
	    << epsabs << '\t'
	    << epsrel << '\t'
	    << xmin << '\t'
	    << std::scientific << result << std::endl;

  return 0;
}
