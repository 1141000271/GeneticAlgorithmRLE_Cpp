#ifndef CSVCOMPRESSOR_H_INCLUDED
#define CSVCOMPRESSOR_H_INCLUDED

#include "CsvSorter.h"

class CsvCompressor
{
    public:
        /* Constructors */
        CsvCompressor();
        CsvCompressor(CsvSorter*);

        /* Setters and getters */
        void set_csv_sorter(CsvSorter*);
        CsvSorter* get_csv_sorter();

        /* Compress a single column. Later used in a single thread by CompressAllColumns().
         * Parameters: column index
         */
        void CompressColumn(int);

        /* Compress all columns in the associated CsvSorter object using a single thread
         * to compress each column.
         */
        void CompressAllColumns();

    private:
        CsvSorter* csv_sorter_;

};

#endif // CSVCOMPRESSOR_H_INCLUDED

