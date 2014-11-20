#ifndef RLEDATASTRUCT_H_INCLUDED
#define RLEDATASTRUCT_H_INCLUDED

#include <string>

#include "VariableByteEncoding.h"

struct RleDataStruct
{
    unsigned int run_length_;
    std::string data_value_;
    RleDataStruct()
    {
        run_length_ = 0;
    }

};

std::ofstream& operator<<(std::ofstream&, const RleDataStruct&);

#endif // RLEDATASTRUCT_H_INCLUDED

