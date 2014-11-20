#ifndef VARIABLEBYTEENCODING_H_INCLUDED
#define VARIABLEBYTEENCODING_H_INCLUDED
#include <stdint.h>

namespace __integer_encoding {
    void VariableByte__Encode(uint32_t*, uint8_t*);
    int VariableByte__Decode(uint8_t*, uint32_t*);
}
#endif // VARIABLEBYTEENCODING_H_INCLUDED
