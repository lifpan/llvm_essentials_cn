#include <arm_neon.h>

unsigned hadd(uint32x4_t a) {
    return a[0] + a[1] + a[2] + a[3];
}
