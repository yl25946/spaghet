#include "nnue.h"

#if defined(USE_AVX512) || defined(USE_AVX2)
#include <inmintrin.h>
#endif