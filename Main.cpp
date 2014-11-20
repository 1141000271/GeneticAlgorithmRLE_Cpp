#include <iostream>
#include <cstring>
#include <sstream>
#include <cstdlib>
#include <vector>
#include <chrono>

#include "CsvCompressor.h"
#include "CsvDecompressor.h"
#include "Global.h"


using namespace boost::filesystem;
using namespace std;
using namespace std::chrono;

void write_archive_records(Archive* archive)
{
    path archivePath = archive->GenerateArchivePathAndDirectory();
    archivePath /= "metadata.data";

    std::ofstream archive_stream(archivePath.string(), std::ios::binary);

    archive_stream.write((char*) archive, sizeof(*archive));
    archive_stream.close();
}

void hide_file(path p)
{
#ifdef _WIN32
    std::string command("attrib +h ");
    command += p.string();
    std::system(command.c_str());
#else
    std::string p_str(".");
    p_str += p.filename().string();
    path p_new(p);
    p_new.remove_filename();
    p_new /= p_str;
    rename(p, p_new);
#endif
}

void hide_compressed_files(Archive* archive)
{
    path fullFilePath = archive->original_root_;
    fullFilePath /= archive->original_file_name_;
    for (size_t i=0; i<archive->columns_; i++) {
        std::string tmpstr(".csv_");
        std::stringstream intToStr;
        intToStr << i;
        tmpstr += intToStr.str();
        tmpstr += ".csvcomp";

        path from = fullFilePath;
        from.replace_extension(tmpstr);
        path to = archive->GenerateArchivePathAndDirectory();
        to /= archive->original_file_name_;
        to.replace_extension(tmpstr);
        rename(from, to);

        hide_file(to);
    }
}



int main(int argc, char* argv[])
{

    if(argc < 3){
        std::cerr << "Usage:" << argv[0] << " [-c|-d] " << "FILE_NAME" << std::endl;
        return 1;
    }

    for(int i=0; i<argc-1; i++){
        if(!strcmp(argv[i], "-delimiter")){
            delimiter = std::string(argv[i+1]);
        }
    }

    // Compress
    if (!strcmp(argv[1], "-c")) {
        path file_path(argv[2]);
        if (!exists(file_path)) {
            std::cerr << "Error: Input file does not exist!" << std::endl;
            return 1;
        }
        int gares_idx = -1;
        int pop_size, number_generations;
        for(int i = 0; i < argc; i++) {
            if(!strcmp(argv[i], "-gares")) gares_idx = i;
            if(!strcmp(argv[i], "-popsize")) pop_size = atoi(argv[i + 1]);
            if(!strcmp(argv[i], "-numgen")) number_generations = atoi(argv[i + 1]);
        }



        streampos begin, end;
        ifstream is(file_path.string().c_str(), ios::binary);
        begin = is.tellg();
        is.seekg(0, ios::end);
        end = is.tellg();
        is.close();

        high_resolution_clock::time_point t1 = high_resolution_clock::now();
        std::cout<< "All data read." << std::endl;
        // Read, analyze and sort the data
        CsvSorter* sorter = new CsvSorter(file_path.string());
        sorter -> CountRows();
        sorter -> ReadAndAnalyzeData(file_path.string(), pop_size, number_generations);
        sorter -> SortData();
        std::cout<< "All data sorted." << std::endl;
        // Run a RLE compression on the data
        CsvCompressor compressor(sorter);
        compressor.CompressAllColumns();


        high_resolution_clock::time_point t2 = high_resolution_clock::now();

        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count();

        // Organize output data
        path file_name = file_path.filename();
        path root_dir = file_path.remove_filename();
        size_t n_columns = sorter -> get_columns();
        uintmax_t compressed_size = sorter -> get_compressed_size();
        cout << "Filename: " << file_name << endl;
        cout << "Initial size: " << (end - begin) << endl;
        cout << "Compressed size: " << compressed_size << endl;
        cout << "Execution time: " << duration << "microseconds" << endl;

        Archive* archive = new Archive(root_dir, file_name, n_columns, IS_WORTH_COMPRESSING, compressed_size);
        hide_compressed_files(archive);
        write_archive_records(archive);

    } else if (!strcmp(argv[1], "-d")) {

        // Decompress
        path decompPath(argv[2]);
        if (!exists(decompPath)) {
            std::cerr << "Error: File does not exist!" << std::endl;
            return 1;
        }

        // Decompress the file
        CsvDecompressor decompressor(decompPath.string());
        decompressor.Decompress();
        std::cout << "File successfully decompressed! "<<std::endl;
        std::cout << "You can find it in the same location as the compressed file you have selected." << std::endl;
    }


    return 0;
}
