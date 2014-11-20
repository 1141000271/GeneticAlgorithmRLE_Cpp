
#include "windows.h"

#include <boost/thread/thread.hpp>
#include <boost/detail/interlocked.hpp>

#include "CsvCompressor.h"

/* Default constructor. Does nothing... */
CsvCompressor::CsvCompressor() : csv_sorter_(NULL) { };

/* A simple constructor to assign an address to the pointer member */
CsvCompressor::CsvCompressor(CsvSorter* csv_sorter) : csv_sorter_(csv_sorter) {};

/* Sets the CsvSorter pointer to point to an object */
void CsvCompressor::set_csv_sorter(CsvSorter* csv_sorter)
{
    csv_sorter_ = csv_sorter;
}

/* Returns a pointer to the CsvSorter object member */
CsvSorter* CsvCompressor::get_csv_sorter()
{
    return csv_sorter_;
}

/* The column is compressed by RLE and this function
 *  is executed in its own separate thread.
 */
void CsvCompressor::CompressColumn(int column_index)
{
    csv_sorter_->ColumnRle(column_index);
}

/* This function compresses the whole CSV file in a parallel
 *  manner.  Namely, we create a thread for each column and
 *  each executes the above CompressColumn() function. Threads
 *  for each column are joined together in a group and then
 *  concurrently started. This is a speed-up, so the compression
 *  of the csv file is very fast.
 */
void CsvCompressor::CompressAllColumns()
{
    size_t columns = csv_sorter_->get_columns();
    boost::thread_group group;

    for (int i=0; i<columns; i++) {
        group.create_thread(boost::bind(&CsvCompressor::CompressColumn, this, i));
    }
    group.join_all();
}
