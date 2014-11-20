#ifndef BINARYINDEXEDTREE_H_INCLUDED
#define BINARYINDEXEDTREE_H_INCLUDED

#include <iostream>
#include <string>
#include <vector>
#include <stdint.h>

#include "RunInfo.h"

class BinaryIndexedTree
{
    private:
        std::vector<RunInfo> tree;
        size_t treesize;
    public:
        BinaryIndexedTree();
        BinaryIndexedTree(int);
        std::vector<RunInfo> get_tree();
        size_t get_treesize();
        void resizeAdd(size_t);
        void add(int, RunInfo);
        void print_tree(int);
        unsigned int read(int, std::string);
        unsigned int read_string_length(int);
        unsigned int read_run_overhead(int);
        unsigned int read_total_size(int);
};

#endif // BINARYINDEXEDTREE_H_INCLUDED
