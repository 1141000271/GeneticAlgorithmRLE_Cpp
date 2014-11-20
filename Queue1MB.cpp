#include "Queue1MB.h"
#include <iostream>

Queue1MB::Queue1MB(void)
{
    q = std::queue<RunInfo>();
    sz_ = 0;
    num_el_ = 0;
}
std::queue<RunInfo>* Queue1MB::get_queue()
{
    return &q;
}

size_t Queue1MB::get_size()
{
    return sz_;
}

size_t Queue1MB::get_num_el()
{
    return num_el_;
}

void Queue1MB::print()
{
    std::queue<RunInfo> q2;
    q2 = q;
    std::cout << "QUEUE : ";
    while(!q2.empty())
    {
        RunInfo x = q2.front();
        std:: cout <<"(" << x.run_length_x_string_length << " " << x.run_overhead << " " << x.string_length << ") | ";
        q2.pop();
    }
    std::cout<< std::endl;
}

bool Queue1MB::enqueue(RunInfo val)
{
    if(sz_ <= 1048576-sizeof(RunInfo)){     // 1MB - size of a RunInfo struct
        sz_ += sizeof(RunInfo);
        num_el_++;
        q.push(val);
        return true;
    }
    return false;
}

RunInfo Queue1MB::pop()
{
    if(q.size() > 0){
        sz_ -= sizeof(unsigned int);
        num_el_--;
        RunInfo front = q.front();
        q.pop();
        return front;
    }
    RunInfo err;
    return err;
}
