#ifndef ARCHIVE_H_INCLUDED
#define ARCHIVE_H_INCLUDED

#include <boost/filesystem.hpp>

using namespace boost::filesystem;

struct Archive
{
    char original_file_name_[100];
    char original_root_[100];
    size_t columns_;
    bool is_worth_compressing_[100];
    uintmax_t total_compressed_size_;

    Archive();
    Archive(path, path, size_t, std::vector<bool>, uintmax_t);
    path GenerateArchivePathAndDirectory();
};

#endif // ARCHIVE_H_INCLUDED
