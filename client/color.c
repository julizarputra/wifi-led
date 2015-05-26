#include "color.h"
#define MIN(x,y,z) x<y?(x<z?x:z):(y<z?y:z)
#define MAX(x,y,z) x>y?(x>z?x:z):(y>z?y:z)

void RGBtoHSV( float r, float g, float b, float *h, float *s, float *v )
{
  float min, max, delta;

  min = MIN( r, g, b );
  max = MAX( r, g, b );
  *v = max;       // v

  delta = max - min;

  if( max != 0 )
    *s = delta / max;   // s
  else {
    // r = g = b = 0    // s = 0, v is undefined
    *s = 0;
    *h = -1;
    return;
  }

  if( r == max )
    *h = ( g - b ) / delta;   // between yellow & magenta
  else if( g == max )
    *h = 2 + ( b - r ) / delta; // between cyan & yellow
  else
    *h = 4 + ( r - g ) / delta; // between magenta & cyan

  *h *= 60;       // degrees
  if( *h < 0 )
    *h += 360;

}

void HSVtoRGB( float *r, float *g, float *b, float h, float s, float v )
{
  int i;
  float f, p, q, t;

  if( s == 0 ) {
    // achromatic (grey)
    *r = *g = *b = v;
    return;
  }

  h /= 60;      // sector 0 to 5
  i = (int)h;
  f = h - i;      // factorial part of h
  p = v * ( 1 - s );
  q = v * ( 1 - s * f );
  t = v * ( 1 - s * ( 1 - f ) );

  switch( i ) {
    case 0:
      *r = v;
      *g = t;
      *b = p;
      break;
    case 1:
      *r = q;
      *g = v;
      *b = p;
      break;
    case 2:
      *r = p;
      *g = v;
      *b = t;
      break;
    case 3:
      *r = p;
      *g = q;
      *b = v;
      break;
    case 4:
      *r = t;
      *g = p;
      *b = v;
      break;
    default:    // case 5:
      *r = v;
      *g = p;
      *b = q;
      break;
  }

}

void set_hsv(led *strip,int index,float h,float s,float v) {
  float r,g,b;
  HSVtoRGB(&r,&g,&b,h,s,v);
  strip[index].r=(char)(r*255);
  strip[index].g=(char)(g*255);
  strip[index].b=(char)(b*255);
}
