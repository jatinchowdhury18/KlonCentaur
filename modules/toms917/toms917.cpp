# include <cfenv>
# include <cfloat>
# include <cmath>
# include <complex>
# include <cstdlib>
# include <ctime>
# include <iomanip>
# include <iostream>

using namespace std;
//
//  DBL_EPSILON, provided by <cfloat>, is twice the machine epsilon for
//  double precision arithmetic.
//
# define TWOITERTOL DBL_EPSILON

# include "toms917.hpp"

//****************************************************************************80

complex <double> wrightomega ( complex <double> z )

//****************************************************************************80
//
//  Purpose:
//
//    WRIGHTOMEGA is the simple routine for evaluating the Wright Omega function. 
//
//  Discussion:
//
//    This function is called by:
//
//      w = wrightomega ( z )
//
//    This function makes a call to the more powerful wrightomega_ext() function.
//
//  Modified:
//
//    14 May 2016
//
//  Author:
//
//    Piers Lawrence, Robert Corless, David Jeffrey
//
//  Reference:
//
//    Piers Lawrence, Robert Corless, David Jeffrey,
//    Algorithm 917: Complex Double-Precision Evaluation of the Wright Omega 
//    Function,
//    ACM Transactions on Mathematical Software,
//    Volume 38, Number 3, Article 20, April 2012, 17 pages.
//
//  Parameters:
//
//    Input, complex <double> Z, the argument.
//
//    Output, complex <double> WRIGHTOMEGA, the value of the Wright Omega
//    function of Z.
//
{
  complex <double> cond;
  complex <double> e;
  complex <double> r;
  complex <double> w;

  wrightomega_ext ( z, w, e, r, cond );

  return w;
}
//****************************************************************************80

int wrightomega_ext ( complex <double> z, complex <double> &w, 
  complex <double> &e, complex <double> &r, complex <double> &cond )

//****************************************************************************80
//
//  Purpose:
//
//    WRIGHTOMEGA_EXT computes the Wright Omega function with extra information.
//
//  Discussion:
//
//    WRIGHTOMEGA_EXT is the extended routine for evaluating the Wright
//    Omega function with the option of extracting the last update step,
//    the penultimate residual and the condition number estimate.
//
//  Modified:
//
//    14 May 2016
//
//  Author:
//
//    Piers Lawrence, Robert Corless, David Jeffrey
//
//  Reference:
//
//    Piers Lawrence, Robert Corless, David Jeffrey,
//    Algorithm 917: Complex Double-Precision Evaluation of the Wright Omega 
//    Function,
//    ACM Transactions on Mathematical Software,
//    Volume 38, Number 3, Article 20, April 2012, 17 pages.
//
//  Parameters:
//
//    Input, complex <double> Z, value at which to evaluate Wrightomega().
//
//    Output, complex <double> &W, the value of Wrightomega(z).
//
//    Output, complex <double> &E, the last update step in the iterative scheme.
//
//    Output, complex <double> &R, the penultimate residual,
//    r_k = z - w_k - log(w_k)
//
//    Output, complex <double> &COND, the condition number estimate. 
//
//    Output, int WRIGHTOMEGA_EXT, error flag;
//    0, successful computation.
//    nonzero, the computation failed.        
//
{
  double near;
  double pi = M_PI;
  complex <double> pz;
  double s = 1.0;
  complex <double> t;
  complex <double> wp1;
  double x;
  double y;
  double ympi;
  double yppi;
// 
//  Extract real and imaginary parts of Z. 
//
  x = real ( z );
  y = imag ( z );
// 
//  Compute if we are near the branch cuts.
//
  ympi = y - pi;
  yppi = y + pi;
  near = 0.01;
// 
//  Test for floating point exceptions:
//

//
//  NaN output for NaN input.
//
  if ( isnan ( x ) || isnan ( y ) )
  {
    w = complex <double> ( ( 0.0 / 0.0 ), ( 0.0 / 0.0 ) );
    e = complex <double> ( 0.0, 0.0 );
    r = complex <double> ( 0.0, 0.0 );
    return 0;
  }
//
//  Signed zeros between branches.
//
  else if ( isinf ( x ) && ( x < 0.0 ) && ( - pi < y ) && ( y <= pi ) )
  {
    if ( fabs ( y ) <= pi / 2.0 )
    {
      w = + 0.0;
    }
    else
    {
      w = - 0.0;
    }
      
    if ( 0.0 <= y )
    {
      w = w + complex <double> ( 0.0, 0.0 );
    }
    else
    {
      w = w + complex <double> ( 0.0, - 1.0 * 0.0 );
    }

    e = complex <double> ( 0.0, 0.0 );
    r = complex <double> ( 0.0, 0.0 );
    return 0;
  }
//
//  Asymptotic for large z.
//
  else if ( isinf ( x ) || isinf ( y ) )
  {
    w = complex <double> ( x, y );
    e = complex <double> ( 0.0, 0.0 );
    r = complex <double> ( 0.0, 0.0 );
    return 0;
  }
//
//  Test if exactly on the singular points.
//
  if ( ( x == - 1.0 ) && ( fabs ( y ) == pi ) )
  {
    w = complex <double> ( - 1.0, 0.0 );
    e = complex <double> ( 0.0, 0.0 );
    r = complex <double> ( 0.0, 0.0 );
    return 0;
  }
// 
//  Choose approximation based on region.
//

//
//  Region 1: upper branch point.
//  Series about z=-1+Pi*I.
//
  if ( ( - 2.0 < x && x <= 1.0 && 1.0 < y && y < 2.0 * pi ) )
  {
    pz = conj ( sqrt ( conj ( 2.0 * ( z + complex <double> ( 1.0, - pi ) ) ) ) );

    w = - 1.0 
      + ( complex <double> ( 0.0, 1.0 )
      + ( 1.0 / 3.0 
      + ( - 1.0 / 36.0 * complex <double> ( 0.0, 1.0 ) 
      + ( 1.0 / 270.0 + 1.0 / 4320.0 * complex <double> ( 0.0, 1.0 ) * pz ) 
      * pz ) * pz ) * pz ) * pz;
  }
//
//  Region 2: lower branch point.
//  Series about z=-1-Pi*I.
//
  else if ( ( - 2.0 < x && x <= 1.0 && - 2.0 * pi < y && y <- 1.0 ) )
  {
    pz = conj ( sqrt ( conj ( 2.0 * ( z + 1.0 + complex <double> ( 0.0, pi ) ) ) ) );

    w = - 1.0 
      + ( - complex <double> ( 0.0, 1.0 ) + ( 1.0 / 3.0 
      + ( 1.0 / 36.0 * complex <double> ( 0.0, 1.0 )
      + ( 1.0 / 270.0 - 1.0 / 4320.0 * complex <double> ( 0.0, 1.0 ) * pz ) 
      * pz ) * pz ) * pz ) * pz;
  }
//
//  Region 3: between branch cuts.
//  Series: About -infinity.
//
  else if ( x <= - 2.0 && - pi < y && y <= pi )
  {
    pz = exp ( z );
    w = ( 1.0 
      + ( - 1.0 
      + ( 3.0 / 2.0 
      + ( - 8.0 / 3.0 
      + 125.0 / 24.0 * pz ) * pz ) * pz ) * pz ) * pz;
  }
//
//  Region 4: Mushroom.
//  Series about z=1.
//
  else if ( ( ( - 2.0 < x ) && ( x <= 1.0 ) && ( - 1.0 <= y ) && ( y <= 1.0 ) )
    || ( ( - 2.0 < x ) && ( x - 1.0 ) * ( x - 1.0 ) + y * y <= pi * pi ) )
  {
    pz = z - 1.0;
    w = 1.0 / 2.0 + 1.0 / 2.0 * z 
      + ( 1.0 / 16.0 
      + ( - 1.0 / 192.0 
      + ( - 1.0 / 3072.0 + 13.0 / 61440.0 * pz ) * pz ) * pz ) * pz * pz;
  }
//
//  Region 5: Top wing.
//  Negative log series.
//
  else if ( x <= - 1.05 && pi < y && y - pi <= - 0.75 * ( x + 1.0 ) )
  {
    t = z - complex <double> ( 0.0, pi );
    pz = log ( - t );
    w = ( ( 1.0 + ( - 3.0 / 2.0 + 1.0 / 3.0 * pz ) * pz ) * pz 
      + ( ( -1.0 + 1.0 / 2.0 * pz ) * pz + ( pz + ( - pz + t ) * t ) * t ) * t ) 
      / ( t * t * t );
  }
//
//  Region 6: Bottom wing.
//  Negative log series.
//
  else if ( x <= - 1.05 && 0.75 * ( x + 1.0 ) < y + pi && y + pi <= 0.0 )
  {
    t = z + complex <double> ( 0.0, pi );
    pz = log ( - t );
    w = ( ( 1.0 + ( - 3.0 / 2.0 + 1.0 / 3.0 * pz ) * pz ) * pz
      + ( ( - 1.0 + 1.0 / 2.0 * pz ) * pz + ( pz + ( - pz + t ) * t ) * t ) * t ) 
      / ( t * t * t );
  }
//
//  Region 7: Everywhere else.
//  Series solution about infinity.
//
  else
  {
    pz = log ( z );
    w = ( ( 1.0 + ( - 3.0 / 2.0 + 1.0 / 3.0 * pz ) * pz ) * pz 
      + ( ( - 1.0 + 1.0 / 2.0 * pz ) * pz + ( pz + ( - pz + z ) * z ) * z ) * z ) 
      / ( z * z * z );
  }
//
//  Regularize if near branch cuts.
///
  if ( x <= - 1.0 + near && ( fabs ( ympi ) <= near || fabs ( yppi ) <= near ) ) 
  { 
    s = - 1.0;
    if ( fabs ( ympi ) <= near )
    {
//
//  Recompute ympi with directed rounding.
//
      fesetround ( FE_UPWARD );
      ympi = y - pi;
          
      if ( ympi <= 0.0 )
      {
        fesetround ( FE_DOWNWARD );
        ympi = y - pi;
      }
         
      z = complex <double> ( x, ympi ); 
// 
//  Return rounding to default.
//
      fesetround ( FE_TONEAREST );
    }
    else
    {
//
//  Recompute yppi with directed rounding.
//
      fesetround ( FE_UPWARD );
      yppi = y + pi;
          
      if ( yppi <= 0.0 )
      {
        fesetround ( FE_DOWNWARD );
        yppi = y + pi;
      }

      z = complex <double> ( x, yppi );
// 
//  Return rounding to default.
//
      fesetround ( FE_TONEAREST );
    }
  }
//
//  Iteration one.
//
  w = s * w;
  r = z - s * w - log ( w );
  wp1 = s * w + 1.0;
  e = r / wp1 * ( 2.0 * wp1 * ( wp1 + 2.0 / 3.0 * r ) - r )
    / ( 2.0 * wp1 * ( wp1 + 2.0 / 3.0 * r ) - 2.0 * r );
  w = w * ( 1.0 + e );
//
//  Iteration two.
//
  if ( abs ( ( 2.0 * w * w - 8.0 * w - 1.0 ) * pow ( abs ( r ), 4.0 ) ) 
    >= TWOITERTOL * 72.0 * pow ( abs ( wp1 ), 6.0 ) )
  {
    r = z - s * w - log ( w );
    wp1 = s * w + 1.0;
    e = r / wp1 * ( 2.0 * wp1 * ( wp1 + 2.0 / 3.0 * r ) - r ) 
      / ( 2.0 * wp1 * ( wp1 + 2.0 / 3.0 * r ) - 2.0 * r );
    w = w * ( 1.0 + e );
  }
//
//  Undo regularization.
//
  w = s * w;
//
//  Provide condition number estimate.
//
  cond = z / ( 1.0 + w );
      
  return 0;
}
//****************************************************************************80

void timestamp ( )

//****************************************************************************80
//
//  Purpose:
//
//    TIMESTAMP prints the current YMDHMS date as a time stamp.
//
//  Example:
//
//    31 May 2001 09:45:54 AM
//
//  Licensing:
//
//    This code is distributed under the GNU LGPL license. 
//
//  Modified:
//
//    08 July 2009
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    None
//
{
# define TIME_SIZE 40

  static char time_buffer[TIME_SIZE];
  const struct std::tm *tm_ptr;
  std::time_t now;

  now = std::time ( NULL );
  tm_ptr = std::localtime ( &now );

  std::strftime ( time_buffer, TIME_SIZE, "%d %B %Y %I:%M:%S %p", tm_ptr );

  std::cout << time_buffer << "\n";

  return;
# undef TIME_SIZE
}
