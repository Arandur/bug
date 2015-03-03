#include <stdio.h>
#include <stdint.h>

/**
 * Compile with:
 *
 * `g++ -std=c++14 -Os`
 */

#ifndef vxCPLSEED
  // If you don't specify the seed for algorithms, the time when compilation
  // started will be used, seed actually changes the results of algorithms...
  #define vxCPLSEED ( \
                      (__TIME__[7] - '0') *     1 + \
                      (__TIME__[6] - '0') *    10 + \
                      (__TIME__[4] - '0') *    60 + \
                      (__TIME__[3] - '0') *   600 + \
                      (__TIME__[1] - '0') *  3600 + \
                      (__TIME__[0] - '0') * 36000   \
                    )
#endif

// The constantify template is used to make sure that the result of constexpr
// function will be computed at compile-time instead of run-time
template <uint32_t Const> 
struct vxCplConstantify { 

  enum { Value = Const }; 
};

// Compile-time mod of a linear congruential pseudorandom number generator,
// the actual algorithm was taken from "Numerical Recipes" book
constexpr auto vxCplRandom(uint32_t Id) ->
  uint32_t { 

  return (1013904223 + 1664525 * 
          ((Id > 0) ? 
            (vxCplRandom(Id - 1)) : 
            (vxCPLSEED))) & 
          0xFFFFFFFF; 
}

// Compile-time random macros, can be used to randomize execution  
// path for separate builds, or compile-time trash code generation
#define vxRANDOM(Min, Max)  (Min + (vxRAND() % (Max - Min + 1)))
#define vxRAND()            (vxCplConstantify<vxCplRandom(__COUNTER__ + 1)>::Value)

// Compile-time generator for list of indexes (0, 1, 2, ...)
template <uint32_t...> 
struct vxCplIndexList {};

template <typename  IndexList, uint32_t Right> 
struct vxCplAppend;

template <uint32_t... Left,   uint32_t Right> 
struct vxCplAppend<vxCplIndexList<Left...>, Right> { 

  typedef vxCplIndexList<Left..., Right> Result; 
};

template <uint32_t N> 
struct vxCplIndexes { 

  typedef 
    typename vxCplAppend<typename vxCplIndexes<N - 1>::Result, N - 1>::Result 
    Result; 

};

template <> 
struct vxCplIndexes<0> { 

  typedef vxCplIndexList<> Result; 
};

// Compile-time string encryption of a single character
const char vxCplEncryptCharKey = vxRANDOM(0, 0xFF);
constexpr char vxCplEncryptChar(const char Ch, uint32_t Idx) { 

  return Ch ^ (vxCplEncryptCharKey + Idx); 
}

// Compile-time string encryption class
template <typename IndexList> 
struct vxCplEncryptedString;

template <uint32_t... Idx>  
struct vxCplEncryptedString<vxCplIndexList<Idx...>> {

  char Value[sizeof...(Idx) + 1]; // Buffer for a string

  // Compile-time constructor
  constexpr inline vxCplEncryptedString(const char* const Str)  
    : Value{vxCplEncryptChar(Str[Idx], Idx)...} {}

  // Run-time decryption
  char* decrypt() {

    for(uint32_t t = 0; t < sizeof...(Idx); ++t) { 

      this->Value[t] ^= (vxCplEncryptCharKey + t); 
    }

    this->Value[sizeof...(Idx)] = '\0'; 

    return this->Value;
  }
};

// Compile-time string encryption macro
#define vxENCRYPT(Str) (vxCplEncryptedString<vxCplIndexes<sizeof(Str) - 1>::Result>(Str).decrypt())
