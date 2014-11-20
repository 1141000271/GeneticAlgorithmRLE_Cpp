#ifndef QUEUE1MB_H_INCLUDED
#define QUEUE1MB_H_INCLUDED

#include <queue>
#include <stdint.h>

#include "RunInfo.h"

class Queue1MB
{
    private:
        std::queue<RunInfo> q;
        size_t sz_;
        size_t num_el_;
    public:
        Queue1MB(void);
        std::queue<RunInfo>* get_queue();
        size_t get_size();
        size_t get_num_el();
        void print();
        bool enqueue(RunInfo);
        RunInfo pop();

};

#endif // QUEUE1MB_H_INCLUDED
