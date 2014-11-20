#include <fstream>
#include "RleDataStruct.h"

std::ofstream& operator<<(std::ofstream& ofs, const RleDataStruct& rhs){
    uint8_t bytes[5];
    bytes[0] = bytes[1] = bytes[2] = bytes[3] = bytes[4] = 0;
    uint32_t num = (uint32_t)rhs.run_length_;
    __integer_encoding::VariableByte__Encode(&num, bytes);
    if (bytes[0] != 0) ofs.write(reinterpret_cast<const char*>(&(bytes[0])), sizeof(bytes[0]));
    if (bytes[1] != 0) ofs.write(reinterpret_cast<const char*>(&(bytes[1])), sizeof(bytes[1]));
    if (bytes[2] != 0) ofs.write(reinterpret_cast<const char*>(&(bytes[2])), sizeof(bytes[2]));
    if (bytes[3] != 0) ofs.write(reinterpret_cast<const char*>(&(bytes[3])), sizeof(bytes[3]));
    if (bytes[4] != 0) ofs.write(reinterpret_cast<const char*>(&(bytes[4])), sizeof(bytes[4]));
    ofs.write(rhs.data_value_.c_str(),rhs.data_value_.length());
    return ofs;
}
