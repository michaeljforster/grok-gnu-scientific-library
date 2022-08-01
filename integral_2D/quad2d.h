/*
 * quad2d.h
 */

#ifndef __QUAD2D_H__
#define __QUAD2D_H__

/*
 * Definition of an arbitrary function of two or more parameters to
 * integrate in two dimensions.
 */

struct quad2d_function_struct
{
  double (* function) (double x, double y, void * params);
  void * params;
};

typedef struct quad2d_function_struct quad2d_function;

/*
 * Computes the double integral of the function f(x, y) for x from
 * xmin to infinity using the QAGS algorithm and for y from ymin(x) to
 * ymax(x) using the QAG algorithm.
 *
 * The values of `epsabs` and `epsrel` determine absolute and relative
 * error limits for the estimate of the integrals.  The value of
 * `limit` determines the maximum number of subintervals and must not
 * exceed the allocated size of the `workspace`, which stores the
 * subintervals and their results. The value of `key` chooses the
 * integration rule.
 *
 * The function returns the final approximation, `result`.
 *
 * See the GSL Numerical Integration documentation for further
 * details,
 * https://www.gnu.org/software/gsl/doc/html/integration.html.
 *
 * Example:
 *
 * 1. The 2D function to integrate.
 *
 *    double f(double x, double y, double z)
 *    {
 *      return exp(-(x+y)) * z;
 *    }
 *
 * 2. Adapt the function for quad2d.
 *
 *    double f_2d(double x, double y, void * params)
 *    {
 *      double z = *(double *)params;
 *      return f(x, y, z);
 *    }
 *
 * 3. The ymin and ymax functions.
 *
 *    double ymin(double x)
 *    {
 *      return 0;
 *    }
 *
 *    double ymax(double x)
 *    {
 *      return x;
 *    }
 *
 * 4. Main.
 *
 *    int main ()
 *    {
 *      double epsabs = 5.0e-3;
 *      double epsrel = 5.0e-3;
 *      size_t limit = 1000;
 *      int key = GSL_INTEG_GAUSS31;
 *      double xmin = 1.0;
 *      double z = 42.1;
 *      quad2d_function FXY;
 *      FXY.function = &f_2d;
 *      FXY.params = &z;
 *      double result;
 *
 *      int status = quad2d(&FXY, xmin, &ymin, &ymax, epsabs, epsrel,
 *                          limit, key, &resultr);
 *
 *      std::cout << "STATUS: " << gsl_strerror(status) << std::endl;
 *      std::cout << "RESULT: " << std::scientific << result << std::endl;
 *      std::cout << "ABSERR: " << std::scientific << abserr << std::endl;
 *
 *      if (status != GSL_SUCCESS) {
 *        exit(EXIT_FAILURE);
 *      }
 *
 *      exit(EXIT_SUCCESS);
 *    }
 */

int quad2d(quad2d_function * fxy,
	   double xmin,
	   double (* ymin) (double x),
	   double (* ymax) (double x),
	   double epsabs,
	   double epsrel,
	   size_t limit,
	   int key,
	   double *result);

#endif /*  __QUAD2D_H__ */
