#pragma once

#include <stdint.h>
#include "encode_def.h"

namespace CheckEncode
{

extern "C" EncodeType Check(const uint8_t* stream, size_t len);
  
};
