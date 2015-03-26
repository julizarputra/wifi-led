#ifndef GLOBALS_H
#define GLOBALS_H
struct __attribute__((__packed__)) flashconfig {
  int programmed;
  char ssid[32];
  char password[64];
};
#endif //GLOBALS_H
