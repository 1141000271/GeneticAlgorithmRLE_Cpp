#include "Archive.h"

Archive::Archive()
{
    columns_ = 0;
    total_compressed_size_ = 0;
}

Archive::Archive(path orig_root, path orig_name, size_t cols,
                  std::vector<bool> is_worth_compressing, uintmax_t total)
{
    strcpy(original_root_, orig_root.string().c_str());
    strcpy(original_file_name_, orig_name.string().c_str());
    columns_ = cols;
    for(int i=0; i<columns_; i++){
        is_worth_compressing_[i] = is_worth_compressing[i];
    }
    total_compressed_size_ = total;
}

path Archive::GenerateArchivePathAndDirectory()
{
    path archivePath = path(std::string(original_root_));
    archivePath /= original_file_name_;
    archivePath.replace_extension("");

    if (!exists(archivePath)) {
        create_directory(archivePath);
    }

    return archivePath;
}
