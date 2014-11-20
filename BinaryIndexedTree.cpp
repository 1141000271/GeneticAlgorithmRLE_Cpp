#include "BinaryIndexedTree.h"

BinaryIndexedTree::BinaryIndexedTree() : treesize(1)
{
    tree = std::vector<RunInfo>(treesize);
    RunInfo firstElement;
    tree[0] = firstElement;
}

BinaryIndexedTree::BinaryIndexedTree(int size) : treesize(size)
{
    tree = std::vector<RunInfo>(treesize);
    RunInfo firstElement;
    tree[0] = firstElement;
}

std::vector<RunInfo> BinaryIndexedTree::get_tree()
{
    return tree;
}

size_t BinaryIndexedTree::get_treesize()
{
    return treesize;
}

void BinaryIndexedTree::resizeAdd(size_t size_to_add)
{
    tree.resize(treesize + size_to_add);
    treesize += size_to_add;
}

void BinaryIndexedTree::print_tree(int i)
{
    std::cout << "Tree " << i << " = ";
    for(int i=0; i<treesize; i++){
        std::cout <<"("<< tree[i].run_length_x_string_length << " " << tree[i].run_overhead << " " << tree[i].string_length << ") ---> ";
    }
    std::cout << "SIZE = " << treesize << std::endl;
    std::cout << std::endl;
}

void BinaryIndexedTree::add(int idx, RunInfo val)
{
    while(idx < treesize){
        tree[idx].run_length_x_string_length += val.run_length_x_string_length;
        tree[idx].string_length += val.string_length;
        tree[idx].run_overhead += val.run_overhead;
        idx += (idx & -idx);
    }
}
unsigned int BinaryIndexedTree::read(int idx, std::string command)
{
    if(command == "string_length") return read_string_length(idx);
    else if(command == "run_overhead") return read_run_overhead(idx);
    else if(command == "total_size") return read_total_size(idx);
    else return -1;
}

unsigned int BinaryIndexedTree::read_string_length(int idx)
{
    unsigned int sum = 0;
    //idx++;
    while (idx){
        sum += tree[idx].string_length;
        idx -= (idx & -idx);
    }
    return sum;
}

unsigned int BinaryIndexedTree::read_run_overhead(int idx)
{
    unsigned int sum = 0;
    //idx++;
    while (idx){
        sum += tree[idx].run_overhead;
        idx -= (idx & -idx);
    }
    return sum;
}

unsigned int BinaryIndexedTree::read_total_size(int idx)
{
    int sum = 0;
    //idx++;
    while (idx){
        sum += tree[idx].run_length_x_string_length;
        idx -= (idx & -idx);
    }
    return sum;
}
