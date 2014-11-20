#ifndef CSVDECOMPRESSOR_H_INCLUDED
#define CSVDECOMPRESSOR_H_INCLUDED

#include <boost/filesystem.hpp>

#include "RleDataStruct.h"
#include "Archive.h"


class CsvDecompressor
{
    public:
        /* Constructors */
        CsvDecompressor();
        CsvDecompressor(std::string);

        /* Setters and getters */
        void set_file_path_str(std::string);
        void set_file_path(boost::filesystem::path);
        void set_columns(unsigned int);
        std::string get_file_path_str();
        boost::filesystem::path get_file_path();
        unsigned int get_columns();

       /* The main functionality of this class. Decompresses and merges compressed
        * files stored in a single folder given the file path file_path_
        */
        void Decompress();

    private:
        // Path to the folder to be decompressed
        boost::filesystem::path file_path_;

        // The number of columns (separate files)
        unsigned int columns_;
};
#endif // CSVDECOMPRESSOR_H_INCLUDED
