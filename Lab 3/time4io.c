#include <stdint.h>
#include <pic32mx.h>
#include "mipslab.h"


int getsw (void) {
  return ((PORTD & 0xF00) >> 8);
}

int getbtns (void) {

return ((PORTD & 0xE0) >> 5);

}
