//
// some macros to help with bit banging. Use with care they are not
// extensively tested
// See http://www.nongnu.org/avr-libc/user-manual/inline_asm.html
// jnc 19 April 2008
//
#define SET(port, bit) asm volatile("sbi %[p], %[b]"::[b] "I" (bit), [p] "I" (port))
#define CLEAR(port, bit) asm volatile("cbi %[p], %[b]"::[b] "I" (bit), [p] "I" (port))
// register addressed, dont use PORTA etc.
#define rA 0x1B
#define rB 0x18
#define rC 0x15
#define rD 0x12

