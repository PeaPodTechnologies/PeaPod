#ifndef CSOS_TYPES_H_
#define CSOS_TYPES_H_

#include <Arduino.h>

// Cross-compatible data types
typedef enum {
  CSOS_NULL =   0x0, // INVALID - failsafe args
  CSOS_BOOL =   0x2, // 0 | !0
  CSOS_UINT8 =  0x4, // 0-255
  CSOS_INT8 =   0x8, // -128-127
  CSOS_UINT16 = 0x10,// etc...
  CSOS_INT16 =  0x20,
  CSOS_UINT32 = 0x40,// May be a timestamp in ms
  CSOS_INT32 =  0x80,
  CSOS_FLOAT =  0x200, // 16-bit IEEE 754 floating point
  CSOS_DOUBLE = 0x400, // 32-bit IEEE 754 floating point
  CSOS_SCHAR =  0x800, // A character from a null-terminated string (quit if '\0')
} csos_types_t;

// Pointer types (treated like an array, with default a single element)
typedef enum {
  CSOS_NULLP =    (CSOS_NULL    | 0x1), // NOP - void* nullptr, "no args" ergo LAST args used
  CSOS_BOOLP =    (CSOS_BOOL    | 0x1),
  CSOS_UINT8P =   (CSOS_UINT8   | 0x1),
  CSOS_INT8P =    (CSOS_INT8    | 0x1),
  CSOS_UINT16P =  (CSOS_UINT16  | 0x1),
  CSOS_INT16P =   (CSOS_INT16   | 0x1),
  CSOS_UINT32P =  (CSOS_UINT32  | 0x1),
  CSOS_INT32P =   (CSOS_INT32   | 0x1),
  CSOS_FLOATP =   (CSOS_FLOAT   | 0x1),
  CSOS_DOUBLEP =  (CSOS_DOUBLE  | 0x1),
  CSOS_STRING =   (CSOS_SCHAR   | 0x1), // A pointer to a character from a null-terminated string
} csos_ptypes_t;

// Cross-compatible data union - a single 32-bit value shared between all types
typedef union {
  nullptr_t null;
  bool b;
  uint8_t u8;
  int8_t i8;
  uint16_t u16;
  int16_t i16;
  uint32_t u32;
  int32_t i32;
  float f;
  double d;
  char c;
} csos_data_t;

// Cross-compatible pointer union - a single 16-bit pointer shared between all types 
typedef union {
  void* null;
  const bool* b;
  const uint8_t* u8;
  const int8_t* i8;
  const uint16_t* u16;
  const int16_t* i16;
  const uint32_t* u32;
  const int32_t* i32;
  const float* f;
  const double* d;
  const char* c;
} csos_pdata_t;

#endif