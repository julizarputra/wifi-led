typedef struct __attribute__((__packed__)) {
  char g;
  char r;
  char b;
} led;
void RGBtoHSV( float r, float g, float b, float *h, float *s, float *v );
void HSVtoRGB( float *r, float *g, float *b, float h, float s, float v );
void set_hsv(led *strip,int index,float h,float s,float v);

