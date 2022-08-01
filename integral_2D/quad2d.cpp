/*
 * quad2d.cpp
 */

#include <assert.h>
#include <gsl/gsl_errno.h> // gsl_strerror(), GSL_SUCCESS, etc.
#include <gsl/gsl_integration.h>
#include "quad2d.h"

/*
 * Definition of a structure type and function to call f(x, y, p).
 */

struct fy_params_struct
{
  quad2d_function * fxy;
  double x;
};

typedef struct fy_params_struct fy_params;

double fy(double y, void * params)
{
  fy_params * fy_p = (fy_params *)params;
  quad2d_function * fxy = (fy_p->fxy);
  double x = (fy_p->x);
  
  return (*(fxy->function))(x, y, ((fxy)->params));
}

/*
 * Definition of a structure type and function to integrate fy(y, p).
 */

struct fx_params_struct
{
  quad2d_function * fxy;
  double (* ymin) (double x);
  double (* ymax) (double x);
  double epsabs;
  double epsrel;
  size_t limit;
  int key;
};

typedef struct fx_params_struct fx_params;

double fx(double x, void * params)
{
  fx_params * fx_p = (fx_params *)params;
  quad2d_function * fxy = (fx_p->fxy);
  double (* ymin) (double x) = (fx_p->ymin);
  double (* ymax) (double x) = (fx_p->ymax);
  double epsabs = (fx_p->epsabs);
  double epsrel = (fx_p->epsrel);
  size_t limit = (fx_p->limit);
  int key = (fx_p->key);
  
  double result;	
  double abserr;

  fy_params fy_p = { fxy, x };
  
  gsl_function FY;
  FY.function = fy;
  FY.params = &fy_p;

  /* Although the documentation states that a workspace can be reused,
     allocate a new one each time. Ensure that the workspace size is
     at least as large as limit.. */
  size_t workspace_size = limit;
  gsl_integration_workspace *workspace = gsl_integration_workspace_alloc(workspace_size);
  
  int status = gsl_integration_qag(&FY,
				   (*ymin)(x),
				   (*ymax)(x),
				   epsabs,
				   epsrel,
				   limit,
				   key,
				   workspace,
				   &result,
				   &abserr);

  gsl_integration_workspace_free(workspace);

  /* Abort if the inner integration fails. */
  assert(status == GSL_SUCCESS);

  /* Ignore the estimate of the absolute error, abserr, of the inner
     integration. */

  return result;
}

/*
 * Definition of quad2d().
 */

int quad2d(quad2d_function * fxy,
	   double xmin,
	   double (* ymin) (double x),
	   double (* ymax) (double x),
	   double epsabs,
	   double epsrel,
	   size_t limit,
	   int key,
	   double *result)
{
  double abserr;
  
  fx_params fx_p = { fxy, ymin, ymax, epsabs, epsrel, limit, key };

  gsl_function FX;
  FX.function = &fx;
  FX.params = &fx_p;

  /* Although the documentation states that a workspace can be reused,
     allocate a new one each time. Ensure that the workspace size is
     at least as large as limit.. */
  size_t workspace_size = limit;
  gsl_integration_workspace *workspace = gsl_integration_workspace_alloc(workspace_size);
  
  int status = gsl_integration_qagiu(&FX,
				     xmin,
				     epsabs,
				     epsrel,
				     limit,
				     workspace,
				     result,
				     &abserr);

  gsl_integration_workspace_free(workspace);

  /* Ignore the estimate of the absolute error, abserr, of the outer
     integration. */

  return status;
}
