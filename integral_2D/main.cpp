//libraries and namespaces
#include <assert.h>
#include <cmath>
#include <fstream>
#include <gsl/gsl_integration.h>
#include <iomanip>
#include <iostream>

using namespace std;

//
// Globals to be set in main().
//

int key = 0;
size_t limit = 0;
double epsabs = 0.0;
double epsrel = 0.0;

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
// One work-around for this limitation is to declare a global variable
// and wrapper function referencing the global as a free variable.
//

/*
  double xval = 1;

  double f_y(double y, void *params) 
  {
  return f(xval, y);
  }
*/

// Another way is to pass the x value via the "params" parameter.
// See integrand.params = &x; in quad1d().
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

  // f_y(y) = f(x, y)
  // xval = x;

  gsl_function integrand;
  integrand.function = &f_y;

  // f_y(y) = f(x, y)
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
  
  // cout << "QUAD1D INTERVALS: " << workspace->size << endl;
  // cout << "QUAD1D ABSERR: " << std::scientific << abserr << endl;

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

  // cout << "QUAD2D INTERVALS: " << workspace->size << endl;
  // cout << "QUAD2D ABSERR: " << std::scientific << abserr << endl;
  
  gsl_integration_workspace_free(workspace);

  return result;
}

int main (int argc, char* argv[])
{
  // Set global parameters.
  key = GSL_INTEG_GAUSS31; // 3
  limit = 1000;
  epsabs = 5.0e-3;
  epsrel = 5.0e-3;

  // Get xmin from the command line or by pompting for entry.
  double xmin = 0.0;

  if (argc == 2) {
    xmin = atof(argv[1]);
  } else {
    cout << "Enter XMIN: " << endl;
    cin >> xmin;
  }

  double result = quad2d(xmin);

  cout << "RESULT: " << std::scientific << result << endl;

  return 0;
}
