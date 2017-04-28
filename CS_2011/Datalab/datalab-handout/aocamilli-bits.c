/* 
 * CS:APP Data Lab 
 * 
 * Adam Camilli
 * aocamilli 
 *
 * bits.c - Source file with your solutions to the Lab.
 *          This is the file you will hand in to your instructor.
 *
 * WARNING: Do not include the <stdio.h> header; it confuses the dlc
 * compiler. You can still use printf for debugging without including
 * <stdio.h>, although you might get a compiler warning. In general,
 * it's not good practice to ignore compiler warnings, but in this
 * case it's OK.  
 */

#if 0
/*
 * Instructions to Students:
 *
 * STEP 1: Read the following instructions carefully.
 */

You will provide your solution to the Data Lab by
editing the collection of functions in this source file.

INTEGER CODING RULES:
 
  Replace the "return" statement in each function with one
  or more lines of C code that implements the function. Your code 
  must conform to the following style:

  int Funct(arg1, arg2, ...) {
      /* brief description of how your implementation works */
      int var1 = Expr1;
      ...
      int varM = ExprM;

      varJ = ExprJ;
      ...
      varN = ExprN;
      return ExprR;
  }

  Each "Expr" is an expression using ONLY the following:
  1. Integer constants 0 through 255 (0xFF), inclusive. You are
      not allowed to use big constants such as 0xffffffff.
  2. Function arguments and local variables (no global variables).
  3. Unary integer operations ! ~
  4. Binary integer operations & ^ | + << >>
    
  Some of the problems restrict the set of allowed operators even further.
  Each "Expr" may consist of multiple operators. You are not restricted to
  one operator per line.

  You are expressly forbidden to:
  1. Use any control constructs such as if, do, while, for, switch, etc.
  2. Define or use any macros.
  3. Define any additional functions in this file.
  4. Call any functions.
  5. Use any other operations, such as &&, ||, -, or ?:
  6. Use any form of casting.
  7. Use any data type other than int.  This implies that you
     cannot use arrays, structs, or unions.

 
  You may assume that your machine:
  1. Uses 2s complement, 32-bit representations of integers.
  2. Performs right shifts arithmetically.
  3. Has unpredictable behavior when shifting an integer by more
     than the word size.

EXAMPLES OF ACCEPTABLE CODING STYLE:
  /*
   * pow2plus1 - returns 2^x + 1, where 0 <= x <= 31
   */
  int pow2plus1(int x) {
     /* exploit ability of shifts to compute powers of 2 */
     return (1 << x) + 1;
  }

  /*
   * pow2plus4 - returns 2^x + 4, where 0 <= x <= 31
   */
  int pow2plus4(int x) {
    /* exploit ability of shifts to compute powers of 2 */
  int result = (1 << x);
     result += 4;
     return result;
  }

FLOATING POINT CODING RULES

For the problems that require you to implent floating-point operations,
the coding rules are less strict.  You are allowed to use looping and
conditional control.  You are allowed to use both ints and unsigneds.
You can use arbitrary integer and unsigned constants.

You are expressly forbidden to:
  1. Define or use any macros.
  2. Define any additional functions in this file.
  3. Call any functions.
  4. Use any form of casting.
  5. Use any data type other than int or unsigned.  This means that you
     cannot use arrays, structs, or unions.
  6. Use any floating point data types, operations, or constants.


NOTES:
  1. Use the dlc (data lab checker) compiler (described in the handout) to 
     check the legality of your solutions.
  2. Each function has a maximum number of operators (! ~ & ^ | + << >>)
     that you are allowed to use for your implementation of the function. 
     The max operator count is checked by dlc. Note that '=' is not 
     counted; you may use as many of these as you want without penalty.
  3. Use the btest test harness to check your functions for correctness.
  4. Use the BDD checker to formally verify your functions
  5. The maximum number of ops for each function is given in the
     header comment for each function. If there are any inconsistencies 
     between the maximum ops in the writeup and in this file, consider
     this file the authoritative source.			      
/*
 * STEP 2: Modify the following functions according the coding rules.
 * 
 *   IMPORTANT. TO AVOID GRADING SURPRISES:
 *   1. Use the dlc compiler to check that your solutions conform
 *      to the coding rules.
 *   2. Use the BDD checker to formally verify that your solutions produce 
 *      the correct answers.
 */
#endif		
/* Copyright (C) 1991-2016 Free Software Foundation, Inc.
   This file is part of the GNU C Library.

   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   The GNU C Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with the GNU C Library; if not, see
   <http://www.gnu.org/licenses/>.  */
/* This header is separate from features.h so that the compiler can
   include it implicitly at the start of every compilation.  It must
   not itself include <features.h> or any other header that includes
   <features.h> because the implicit include comes before any feature
   test macros that may be defined in a source file before it first
   explicitly includes a system header.  GCC knows the name of this
   header in order to preinclude it.  */
/* glibc's intent is to support the IEC 559 math functionality, real
   and complex.  If the GCC (4.9 and later) predefined macros
   specifying compiler intent are available, use them to determine
   whether the overall intent is to support these features; otherwise,
   presume an older compiler has intent to support these features and
   define these macros by default.  */
/* wchar_t uses Unicode 8.0.0.  Version 8.0 of the Unicode Standard is
   synchronized with ISO/IEC 10646:2014, plus Amendment 1 (published
   2015-05-15).  */
/* We do not support C11 <threads.h>.  */
		
/* 
 * oddBits - return word with all odd-numbered bits set to 1
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 8
 *   Rating: 2
 */

int oddBits(void) {
  // On 32-bit machine word size is 32 bits, 0xAAAAAAAA = 1010...10
  // Unfortunately 0xAAAAAAAA is too big, so get it using shifts.
  int ans = 0xAA;
  int word = ans | ans << 8;
  return word | word << 16;
}

/*
 * isTmin - returns 1 if x is the minimum, two's complement number,
 *     and 0 otherwise 
 *   Legal ops: ! ~ & ^ | +
 *   Max ops: 10
 *   Rating: 1
 */
int isTmin(int x) {
  /* x != 0 AND 2*(~x+1) == 0
   *
   * Essentially we are checking if x == 100...0 [31 0s] since the minimum
   * two's complement negated is simply itself. This also applies to 0, so
   * check it is not 0 with ~(!x)
   */
  return ~(!x) & !(~x + ~x + 2);
}
/* 
 * bitXor - x^y using only ~ and & 
 *   Example: bitXor(4, 5) = 1
 *   Legal ops: ~ &
 *   Max ops: 14
 *   Rating: 1
 */
int bitXor(int x, int y) {
  // Simply follows both rules of XOR operation:
  // 1) ~(x&y) [One must be false and...]
  // 2) ~(~x & ~y) [One must be true].
  return ~(x&y) & ~(~x & ~y);
}
/* 
 * conditional - same as x ? y : z 
 *   Example: conditional(2,4,5) = 4
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 16
 *   Rating: 3
 */
int conditional(int x, int y, int z) {
  int a = !!x; // Set x to a simple boolean value 0 or 1.
  int b = ~a + 1; // Now negate to get 000..0 if x or 111...1 otherwise
  // Now we have the following masks if !!x = 1:
  //    b = 111...1 , ~b = 000...0
  // Or if !!x = 0;
  //    b = 000...0 , ~b = 111...1
  // So the expression below will result in either y if x or z otherwise, since
  // one is will be anded with 111...1 and so unchanged, while the other is anded
  // with 000...0 and therefore set to 0, giving us either (y | 0) or (0 | z)
  return (b & y) | (~b & z);
}
/* 
 * greatestBitPos - return a mask that marks the position of the
 *               most significant 1 bit. If x == 0, return 0
 *   Example: greatestBitPos(96) = 0x40
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 70
 *   Rating: 4 
 */
int greatestBitPos(int x) {
  // Set all digits after most significant to 1, assuming 32-bit integers
  x |= x >> 1;
  x |= x >> 2;
  x |= x >> 4;
  x |= x >> 8;
  x |= x >> 16;
  
  // Now simply and it with its inverse, or with 0x80000000 if negative,
  // since negative numbers have parity bit as leftmost bit
  x &= ((~x >> 1)^(1 << 31));
  return x;
}
/* 
 * divpwr2 - Compute x/(2^n), for 0 <= n <= 30
 *  Round toward zero
 *   Examples: divpwr2(15,1) = 7, divpwr2(-33,4) = -2
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 15
 *   Rating: 2
 */
int divpwr2(int x, int n) {
  // First determine whether x is negative or positive: x >> 31
  // Then subtract 1 to represent 2^(n-1): ((1 << n) + ~0) [~0 == -1, - is forbidden]
  // Now and them together to get 000...0 or 00...11...1 [n 1's]
  // x can be added to this result to get the perfect power 2^n
  // Finally, simply divide perfect power by n using shift: [result] >> n
  return (x + ((x >> 31) & ((1 << n) + ~0))) >> n;
}

/* 
 * isNonNegative - return 1 if x >= 0, return 0 otherwise 
 *   Example: isNonNegative(-1) = 0.  isNonNegative(0) = 1.
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 6
 *   Rating: 3
 */
int isNonNegative(int x) {
  // Simply check if most significant bit is equal to 1, if so x is negative.
  // Since we are assuming 32-bit integers, we right-shift by 31 and check result.
  return !(x >> 31);
}

/*
 * satMul2 - multiplies by 2, saturating to Tmin or Tmax if overflow
 *   Examples: satMul2(0x30000000) = 0x60000000
 *             satMul2(0x40000000) = 0x7FFFFFFF (saturate to TMax)
 *             satMul2(0x60000000) = 0x80000000 (saturate to TMin)
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 20
 *   Rating: 3
 */
int satMul2(int x) {
  int sign_fill = x >> 31; // Find parity bit and fill whole integer with it
  int msb_fill = (x << 1) >> 31; // Isolate MSB of x and fill whole integer with it
  x <<= 1; // Perform multiplication by 2
  
  // Right hand side of below expression is 0 (and x*2 is valid) unless msb != sign,
  // which if true implies result is too large/small and so should be saturated.
  // This case results in the expression
  // x^(x^msb_fill^(1<<31)) = (msb_fill ^ (1 << 31)) = saturation value 
  x ^= (sign_fill ^ msb_fill) & (x ^ msb_fill ^ (1 << 31));
  return x;
}

/* 
 * isLess - if x < y  then return 1, else return 0 
 *   Example: isLess(4,5) = 1.
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 24
 *   Rating: 3
 */
int isLess(int x, int y) {
  int x_neg_y_pos = (x >> 31) & ((!(y >> 31)) | !y); // If x < 0 and y >= 0
  int x_pos_y_neg = ((!(x >> 31) | !x)) & (y >> 31); // If x >= 0 and y < 0
  int diff = x + (~y + 1); // x - y
  // If x < 0 and y >= 0 just return 1,
  // Else return x - y < 0, ignoring case (x >= 0 & y < 0) where 0 is the obvious result.
  // This way we can avoid any possible over or underflow
  return (x_neg_y_pos) | ((diff >> 31) & !(x_pos_y_neg));
}

/* 
 * isAsciiDigit - return 1 if 0x30 <= x <= 0x39 (ASCII codes for characters '0' to '9')
 *   Example: isAsciiDigit(0x35) = 1.
 *            isAsciiDigit(0x3a) = 0.
 *            isAsciiDigit(0x05) = 0.
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 15
 *   Rating: 3
 */
int isAsciiDigit(int x) {
  int lower = 0x30;
  int upper = 0x3a;
  int lower_diff = x + (~lower + 1);  // Should be positive or 0
  int upper_diff = x + (~upper + 1);  // Should be negative
  return (upper_diff >> 31) & (!(lower_diff >> 31));
}

/*
 * trueThreeFourths - multiplies by 3/4 rounding toward 0,
 *   avoiding errors due to overflow
 *   Examples: trueThreeFourths(11) = 8
 *             trueThreeFourths(-9) = -6
 *             trueThreeFourths(1073741824) = 805306368 (no overflow)
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 20
 *   Rating: 4
 */
int trueThreeFourths(int x) {
  // Crux of this problem is that right shifts of negative vals are logical,
  // so they will round to neg. infinity instead of 0, resulting in non-intuitive
  // off by something errors. 

  // When we divide, first two bits are lost, so preserve them.
  int lost_bits = x & 3;
  int mod = (x >> 31) & (x & 3); // If x is negative, adjust by x % 4

  x >>= 2; // Divide by 4
  x += x << 1; // Multiply by 3
  
  // Add back in lost bits + adjustment value if x is negative
  lost_bits = ((lost_bits << 1) + lost_bits + mod) >> 2; 
  return x + lost_bits;
}

/*
 * ilog2 - return floor(log base 2 of x), where x > 0
 *   Example: ilog2(16) = 4
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 90
 *   Rating: 4
 */
int ilog2(int x) {
  int i, j, k, l, m; // Our masks
  // Set all digits after most significant to 1, assuming 32-bit integers
  // x is now 1111...11
  x |= (x >> 1);
  x |= (x >> 2);
  x |= (x >> 4);
  x |= (x >> 8);
  x |= (x >> 16);
  
  // Set i to 0x55555555, which clears all even bits (101010...)
  i = 0x55 | (0x55 << 8);
  i |= (i << 16);

  // Set j to 0x33333333, which clears every other 2 bits (1100...)
  j = 0x33 | (0x33 << 8);
  j |= (j << 16);

  // Set k to 0x0f0f0f0f, which clears every other 4 bits (11110000...)
  k = 0x0f | (0x0f << 8);
  k |= (k << 16);

  // Set l to 0x00ff00ff, which clears every other 8 bits (use your imagination)
  l = 0xff | (0xff << 16);

  // Set m to 0x0000ffff, which sets 16 bits (1111...1)
  m = 0xff | (0xff << 8);

  x = (x & i) + ((x >> 1) & i);
  x = (x & j) + ((x >> 2) & j);
  x = (x & k) + ((x >> 4) & k);
  x = (x & l) + ((x >> 8) & l);
  x = (x & m) + ((x >> 16) & m);
  x += ~0; // x - 1, - is forbidden
  return x; 
}

/* 
 * float_neg - Return bit-level equivalent of expression -f for
 *   floating point argument f.
 *   Both the argument and result are passed as unsigned int's, but
 *   they are to be interpreted as the bit-level representations of
 *   single-precision floating point values.
 *   When argument is NaN, return argument.
 *   Legal ops: Any integer/unsigned operations incl. ||, &&. also if, while
 *   Max ops: 10
 *   Rating: 2
 */
unsigned float_neg(unsigned uf) {
  int sig_mask = (1 << 23) + ~0; // Mask for the significand since 0x7FFFFF is too big
  int is_nan = 0xFF << 23; // Clears non-exponent bits
  int significand = sig_mask & uf; // Clears non-significand/non-mantissa bits

  // Return argument if exp bits are all 1 and significand != 0
  // a.k.a. the binary32 conditions for NaN
  if((is_nan & uf) == is_nan && significand)
    return uf;

  // Otherwise just toggle sign bit
  return uf ^ (1 << 31);
}

/* 
 * float_i2f - Return bit-level equivalent of expression (float) x
 *   Result is returned as unsigned int, but
 *   it is to be interpreted as the bit-level representation of a
 *   single-precision floating point values.
 *   Legal ops: Any integer/unsigned operations incl. ||, &&. also if, while
 *   Max ops: 30
 *   Rating: 4
 */
unsigned float_i2f(int x) {
  /* When converting from integer to float:
   * 1) Significand/Mantissa: Starting from left, sum 1/(2^n) where n is digit number (0-22) of a true bit.
   * 2) Exponent: Starting from right, sum 2^n where n is digit number (0-8) of a true bit, then subtract
   *              the bias for single-precision floats which is 2^7 - 1 = 127
   * 3) Sign: Well, it's either positive or negative as before, just toggle it.
   * 
   * Due to how significand/mantissa works, only 23/24 bits are available for precision. This
   * means that if x contains >23 non-sign bits there will be a loss of precision where the last few bits 
   * are truncated so x can fit into significand.
   * 
   * Finally NaN must be handled as before.
   */
  int sig; // For some reason dlc yells at me if I declare it below
  int exp = 158; // Bias + Width = 127 + 31
  int mask = 1 << 31; // TMin, or 100...0
  int sign = x & mask;

  // Handle obvious cases immediately
  if (x == mask) { // If x is TMin
    // If x is TMin, transform to floating-point representation of TMin,
    // which is 0xCF000000, or an empty significand and exponent of 158
    return mask | (exp << 23);
  } if (!x) // If 0 return 0
    return 0;

  if (sign) // If sign bit is set, unset it
    x = ~x + 1;
  while (!(x & mask)) { // Until we get to sign bit, or leftmost bit, 
    x <<= 1; // Multiply by 2
    exp += ~0; // Subtract from our exponent
  }

  // Take all but the sign bit and divide by 8 to obtain significand
  sig = (x & (~mask)) >> 8;
  // If exponent has leading 1 and rest of exp and significand are not 0
  if ((x & 0x80) && ((x & 0x7F) > 0 || sig & 1))
    sig += 1; // Adjust significand by the implicit base, 1

  // Finally, simply return the full float representation of
  // 1 sign bit, 8 exponent bits, and 23 significand bits.
  return sign + (exp << 23) + sig;
}

/* 
 * float_twice - Return bit-level equivalent of expression 2*f for
 *   floating point argument f.
 *   Both the argument and result are passed as unsigned int's, but
 *   they are to be interpreted as the bit-level representation of
 *   single-precision floating point values.
 *   When argument is NaN, return argument
 *   Legal ops: Any integer/unsigned operations incl. ||, &&. also if, while
 *   Max ops: 30
 *   Rating: 4
 */
unsigned float_twice(unsigned uf) {
  /* This problem is concerned with the exponent component of binary32
   * representation, since it is used to obtain a power of 2 which multiplies
   * the significand.
   *
   * As such, we must deal with 2 cases:
   * 1) Multiplication that doesn't result in overflow with the significand, in which case 
   *    we simply add to the exponent to represent multiplication by 2
   * 2) Multiplication where significand will overflow, in which case we must truncate the rightmost
   *    bits of significand to ensure it remains 23 bits long
   */

  // Obtain sign, exponent, and significand using appropriate shifts
  int sign = uf & (1 << 31);
  int exp = (((uf << 1) >> 24) << 23); 
  int sig = (uf << 9) >> 9;

  // Take care of exceptional cases
  if (exp == 0x7F800000) { // Normal NaN procedure, just return uf
      return uf; 
  } else if (!exp) {
      return ((uf << 1) | sign); // If exponent is 0, shift left by 1 (significand is multiplied by 1 and so unchanged)
                                 // and set sign bit accordingly
  }

  // Otherwise simply add to exponent to multiply by 2
  exp = (exp + (1 << 23)); // Add one to exponent within the whole float
  return(sign | exp | sig); // Simply add the components together for result.
}
