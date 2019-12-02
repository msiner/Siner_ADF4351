/*
The binary GCD code was taken from Wikipedia
https://en.wikipedia.org/wiki/Binary_GCD_algorithm

This version and the original are available under the
Creative Commons Attribution-ShareAlike License
https://creativecommons.org/licenses/by-sa/3.0/

Changes from original:
- Use uint32_t instead if unsigned int
- Modify code formatting and use curly braces for single
line conditional statements
*/

#ifndef BINARY_GCD_H
#define BINARY_GCD_H


#include <stdint.h>


static uint32_t binary_gcd(uint32_t u, uint32_t v)
{
  uint32_t shift;

  /* GCD(0,v) == v; GCD(u,0) == u, GCD(0,0) == 0 */
  if (u == 0) {
    return v;
  }
  if (v == 0) {
    return u;
  }
 
  /* Let shift := lg K, where K is the greatest power of 2
        dividing both u and v. */
  for (shift = 0; ((u | v) & 1) == 0; ++shift) {
    u >>= 1;
    v >>= 1;
  }
 
  while ((u & 1) == 0) {
    u >>= 1;
  }
 
  /* From here on, u is always odd. */
  uint32_t temp = 0;
  do {
    /* remove all factors of 2 in v -- they are not common */
    /*   note: v is not zero, so while will terminate */
    while ((v & 1) == 0) {  /* Loop X */
      v >>= 1;
    }

    /* Now u and v are both odd. Swap if necessary so u <= v,
    then set v = v - u (which is even). For bignums, the
    swapping is just pointer movement, and the subtraction
    can be done in-place. */
    if (u > v) {
      temp = v;
      v = u;
      u = temp; // Swap u and v.
    }
       
    v = v - u; // Here v >= u.
  } while (v != 0);

  /* restore common factors of 2 */
  return u << shift;
}

#endif // BINARY_GCD_H