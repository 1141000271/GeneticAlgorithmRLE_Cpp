#ifndef ARCHIVE_H_INCLUDED
#define ARCHIVE_H_INCLUDED

#include <boost/filesystem.hpp>
#include "CsvSorter.h"

using namespace boost::filesystem;

struct Archive
{
    path original_file_name_;
    path original_root_;
    size_t columns_;
    std::vector<bool> is_worth_compressing_;
    uintmax_t total_compressed_size_;

    Archive();
    Archive(path, path, size_t, std::vector<bool>, uintmax_t);
    path GenerateArchivePathAndDirectory();
};

Archive::Archive()
{

}

Archive::Archive(path orig_root, path orig_name, size_t cols,
                  std::vector<bool> is_worth_compressing, uintmax_t total)
{
    original_root_ = orig_root;
    original_file_name_ = orig_name;
    columns_ = cols;
    is_worth_compressing_ = std::vector<bool>(is_worth_compressing);
    total_compressed_size_ = total;
}

path Archive::GenerateArchivePathAndDirectory()
{
    path archivePath = original_root_;
    archivePath /= original_file_name_;
    archivePath.replace_extension("");

    if (!exists(archivePath)) {
        create_directory(archivePath);
    }

    return archivePath;
}

#endif // ARCHIVE_H_INCLUDED
