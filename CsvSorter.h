#ifndef CSVSORTER_H_INCLUDED
#define CSVSORTER_H_INCLUDED

#include "windows.h"
#include <vector>
#include <algorithm>
#include <hash_set>
#include <string>
#include <cstring>
#include <cstdlib>
#include <stdio.h>

#include <boost/filesystem.hpp>
#include <boost/thread/thread.hpp>
#include <boost/detail/interlocked.hpp>

#include "CsvIterator.h"
#include "RleDataStruct.h"
#include "Queue1MB.h"
#include "BinaryIndexedTree.h"
#include "Archive.h"

extern std::vector<bool> IS_WORTH_COMPRESSING;

/* Struct for holding meta-data */
struct Metadata
{
    std::vector<unsigned int> runs;
    std::vector<unsigned int> overhead;
    std::vector<unsigned int> sizes;
};

class CsvSorter
{
    public:
        /* Constructors */
        CsvSorter();
        CsvSorter(std::string);

        /* Destructor */
        ~CsvSorter();

        /* Setters and getters */
        size_t get_columns();
        size_t get_rows();
        std::vector<__gnu_cxx::hash_set<std::string, std::hash<std::string> > >& get_distinct_values();
        std::string get_file_name();
        uintmax_t get_compressed_size();
        void CountRows();

       /* Analysis of the data stored in all_rows_.
        * More detailed description in the source file...
        */
        void ReadAndAnalyzeData(std::string, int, int);
        std::string exec(const char*);

       /* Performs a Quick-Sort on the table (all_rows_)
        * by sorting all columns in a specific sort order
        * described by sorting_order_ and sorting_direction_.
        */
        void SortData();

       /* Applies RLE compression on a single column.
        * Parameters: column index
        */
        void ColumnRle(int);

        /* Threshold-based compression decision */
        void IsWorthCompressing(int);

       /* A special struct used as a comparator between two rows.
        * It is used during Quick-Sort over the table in the
        * specified order and direction for each column.
        */
        struct SortRowsStruct
        {
            CsvSorter* r;
            SortRowsStruct(CsvSorter* rr) : r(rr) { };

            bool operator() (CsvRow a, CsvRow b)
            {
                bool res = true;
                for(unsigned int i=0; i<a.size(); i++) {
                    int direction = r->sorting_direction_[i];
                    int col = r->sorting_order_[i];
                    if(direction == 1) {
                        if(a[col] < b[col])
                            return true;
                        else if(a[col] > b[col])
                            return false;
                        else if(a[col] == b[col] && i==a.size()-1)
                            return false;
                    } else if(direction == 2){
                        if(a[col] > b[col])
                            return true;
                        else if(a[col] < b[col])
                            return false;
                        else if(a[col] == b[col] && i == a.size()-1)
                           return false;
                     } else if(direction == 0 && i==a.size()-1){
                            return false;
                     }

                }
                return res;
            }
        };

       /* A special struct for calculating the column order for
        * sorting the table using the heuristic described in
        * the paper.
        */
        struct SortingOrderStruct
        {
            CsvSorter* r;
            SortingOrderStruct(CsvSorter* rr) : r(rr) { };

            bool operator() (int i, int j)
            {
                return (r->distinct_values_[i].size() < r->distinct_values_[j].size());
            }
        };



    private:
        // The name of the CSV file
        std::string file_name_;

        // Number of rows and columns
        size_t rows_;
        size_t columns_;

        // Order and direction in which the columns will be sorted
        std::vector<int> sorting_order_;
        std::vector<int> sorting_direction_;

        // A vector of hash-sets for counting the distinct values in O(N)
        std::vector<__gnu_cxx::hash_set<std::string, std::hash<std::string> > > distinct_values_;

        // Holder for the CSV data orgainzed in rows
        std::vector<CsvRow> all_rows_;

        // Size after compression to be calculated
        uintmax_t compressed_size_;

};

#endif // CSVSORTER_H_INCLUDED
