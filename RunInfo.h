#ifndef RUNINFO_H_INCLUDED
#define RUNINFO_H_INCLUDED
#include <iostream>

struct RunInfo
{
    uint32_t run_length_x_string_length;
    uint16_t string_length;
    uint8_t run_overhead;

    RunInfo()
    {
        run_length_x_string_length = string_length = run_overhead = 0;
    }

    RunInfo(const RunInfo& other)
    {
        run_length_x_string_length = other.run_length_x_string_length;
        string_length = other.string_length;
        run_overhead = other.run_overhead;
    }

    void print()
    {
        std:: cout << "(" << run_length_x_string_length << " " << run_overhead << " " << string_length << ")";
    }
};

#endif // RUNINFO_H_INCLUDED
