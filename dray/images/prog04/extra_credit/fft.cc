// To use this program, pass in a PPM file on stdin and it will send the
// Fourier transform to stdout. i.e.:
//
//   ./fft < prog04.ppm > prog04-fft.ppm
//
// Depending on the intensities in your image, you may need to adjust the
// value on line 82.

#include <math.h>
#include <complex>
#include <iostream>
#include <string>
#include <algorithm>

using namespace std;

typedef complex< double > comp;

static const double pi = 3.141592653589793;

void fft( comp *data, int size, int stride ) {
  for ( int index = 0; index < size; ++index ) {
    int reversed = 0;
    for ( int bit = 0; ( 1 << bit ) < size; ++bit )
      reversed = ( reversed << 1 ) + ( ( index >> bit ) & 1 );
    if (  index < reversed )
      swap( data[ index * stride ], data[ reversed * stride ] );
  }
  for ( int pass = 1; pass < size; pass *= 2 ) {
    double theta = -pi / pass;
    double sin_half_theta = sin( 0.5 * theta );
    comp recurrence( -2.0 * sin_half_theta * sin_half_theta, sin( theta ) );
    comp omega( 1.0, 0.0 );
    for ( int block = 0; block < pass; ++block ) {
      for ( int butterfly = block; butterfly < size; butterfly += pass * 2 ) {
        int top = butterfly * stride;
        int bottom = ( butterfly + pass ) * stride;
        comp shift( data[ bottom ] * omega );
        data[ bottom ] = data[ top ] - shift;
        data[ top ] += shift;
      }
      omega += omega * recurrence;
    }
  }
}

int main( int argc, char ** argv ) {
  string magic;
  int width, height, maximum;
  cin >> magic >> width >> height >> maximum;
  cin.get();
  comp *data = new comp[ width * height ];
  for ( int y = 0; y < height; ++y ) {
    for ( int x = 0; x < width; ++x ) {
      double luminance = cin.get() * 0.30;
      luminance += cin.get() * 0.59;
      luminance += cin.get() * 0.11;
      data[ y * width + x ] = comp( luminance, 0.0 );
    }
  }
  for ( int y = 0; y < height; ++y ) {
    cerr << y << "\r";
    fft( data + y * width, width, 1 );
  }
  for ( int x = 0; x < width; ++x ) {
    cerr << x << "    \r";
    fft( data + x, height, width );
  }
  cout << "P6 " << width << " " << height << " 255 ";
  double total = 0.0;
  int counted = 0;
  for ( int y = 0; y < height; ++y )
    for ( int x = 0; x < width; ++x )
      if ( ( x > 2 && x < width - 2 ) ||
           ( y > 2 && y < height - 2 ) ) {
        double value = abs( data[ y * width + x ] );
        if ( value > 0.0 ) {
          total += log( value );
          ++counted;
        }
      }
  double amplification = 20.0 / exp( total / counted );
  for ( int y = 0; y < height; ++y )
    for ( int x = 0; x < width; ++x ) {
      int index = ( ( ( y + height / 2 ) % height ) * width +
                    ( ( x + width / 2 ) % width ) );
      double value = min( 255.0, abs( data[ index ] ) * amplification );
      unsigned char grey = static_cast< unsigned char >( value );
      cout.put( grey );
      cout.put( grey );
      cout.put( grey );
    }
  return 0;
}
