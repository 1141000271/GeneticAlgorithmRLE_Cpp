#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>
#include <string>
#include <limits>

#include "CsvDecompressor.h"

/* Default constructor. Assigns a 0 to columns_ */
CsvDecompressor::CsvDecompressor() : file_path_(NULL), columns_(0) {}

/* Constructor to initialize the path to a compressed folder */
CsvDecompressor::CsvDecompressor(std::string path_str) : columns_(0)
{
    file_path_ = boost::filesystem::path(path_str);
}
/* A self-explanatory set method using std::string as an argument */
void CsvDecompressor::set_file_path_str(std::string str)
{
    file_path_ = boost::filesystem::path(str);
}

/* A self-explanatory set method using boost::filesystem::path as an argument */
void CsvDecompressor::set_file_path(boost::filesystem::path path_in)
{
    file_path_ = path_in;
}

/* Self-explanatory set method for columns_ */
void CsvDecompressor::set_columns(unsigned int cols)
{
    columns_ = cols;
}

/* Self-explanatory get method for the folder path */
std::string CsvDecompressor::get_file_path_str()
{
    return file_path_.string();
}

/* Self-explanatory get method for the folder path */
boost::filesystem::path CsvDecompressor::get_file_path()
{
    return file_path_;
}

/* Returns the number of columns(compressed files in the dir) */
unsigned int CsvDecompressor::get_columns()
{
    return columns_;
}

/* Main utility of the class. First the path to the meta-data
 *  archive file is constructed and data. It is then read into
 *  the corresponding variables. Streams are instantiated to
 *  each compressed file representing a single column from the
 *  original file.
 *
 *  Having this, each file is iterated row-by-row and run lengths
 *  are decoded into a 32-bit unsigned integer. This value is
 *  then used to "unpack" the run by repeatedly writing the run
 *  value to an output CSV file.
 *
 *  Each separate decompressed column is actually merged in the
 *  output file and the original column-store table has been
 *  successfully reconstructed.
 */
void CsvDecompressor::Decompress()
{
    // Path to the folder that contains all compressed files
    boost::filesystem::path rootFolder;

    // Name of the file that will be decompressed
    boost::filesystem::path fileName;

    // Read meta-data from the archive
    std::ifstream archive(file_path_.string(), std::ios::binary);
    Archive archive_record;
    archive.read((char*)&archive_record, sizeof(archive_record));
    archive.close();
    rootFolder = path(std::string(archive_record.original_root_));
    fileName = path(std::string(archive_record.original_file_name_));
    columns_ = archive_record.columns_;

    std::cout<<rootFolder<<"\n"<<fileName<<"\n"<<columns_<<std::endl;

    boost::filesystem::path tmpDir = file_path_;
    tmpDir.remove_filename();
    tmpDir /= fileName;



    // Use a vector of pointers instead, because we can't copy fstream objects
    std::vector<std::ifstream*> streams;
    for (unsigned int i=0; i<columns_; i++) {
        std::string tmp(".csv_");
        std::stringstream strstr;
        strstr << i;
        tmp += strstr.str();
        tmp += ".csvcomp";
        boost::filesystem::path tmpFilePath = tmpDir;
        tmpFilePath.replace_extension(tmp);

        // Resolve the path for hidden files
        boost::filesystem::path hiddenFilePath;
        #ifdef  _WIN32
            hiddenFilePath = tmpFilePath;
        #else
            std::string posix_hide(".");
            posix_hide += tmpFilePath.filename().string();
            tmpFilePath.remove_filename();
            tmpFilePath /= posix_hide;
            hiddenFilePath = tmpFilePath;
        #endif

        /* Explicitly dynamically allocate memory so it remains available
         *  after leaving the scope of the for() loop. -- If objects were
         *  used instead of pointers, then they would have been valid within
         *  the for() scope ONLY.
         *  NOTE: If we used objects instead of pointers, they would have
         *  been valid within the for() scope ONLY, because they would have
         *  been declared within the scope.
         */
        std::ifstream* file = new std::ifstream(hiddenFilePath.string(), std::ios::in | std::ios::binary);
        streams.push_back(file);
    }

    // Create the output file
    boost::filesystem::path outPath = tmpDir;
    outPath.replace_extension("_decompressed.csv");
    std::ofstream output(outPath.string(), std::ios::out | std::ios::binary);

    // Structs for each row in a single column
    std::vector<RleDataStruct> rows;
    std::vector<bool> isCompressed(columns_);

    // Other needed variables
    unsigned int minVal;
    uint32_t runLength;
    uint8_t bytes[5];
    int nextPos;
    RleDataStruct data;
    bool isValid[columns_];
    unsigned int invalidCounter = 0;

    // Bool array which indicates whether a columns has been
    // completely decompressed and written to the output file
    for (unsigned int i=0; i<columns_; i++) {
        isValid[i] = true;
        isCompressed[i] = archive_record.is_worth_compressing_[i];
    }

    // For each compressed column stored in its own file...
    std::string line;
    minVal = std::numeric_limits<unsigned int>::max();
    for (unsigned int i=0; i<columns_; i++) {

            // Read and decode the first row
            if (std::getline(*streams[i], line)) {
                if(isCompressed[i]){
                    short len = line.length();
                    bytes[0] = line.at(0);
                    bytes[1] = len>=2 ? line.at(1) : 0;
                    bytes[2] = len>=3 ? line.at(2) : 0;
                    bytes[3] = len>=4 ? line.at(3) : 0;
                    bytes[4] = len>=5 ? line.at(4) : 0;
                    nextPos = __integer_encoding::VariableByte__Decode(bytes, &runLength);
                    data.run_length_ = runLength;
                    data.data_value_ = line.substr(nextPos, line.length()-nextPos);
                } else {
                    //the column is not compressed, so each run has a length of 1
                    runLength = 1;
                    data.run_length_ = runLength;
                    data.data_value_ = line;

                }
                if (runLength < minVal) {
                    minVal = runLength;
                }
                rows.push_back(data);
            }
    }

     // For each column again...
    while (invalidCounter < columns_) {

        size_t n = rows.size();
        while (minVal > 0) {
            for (size_t i=0; i<n; i++) {

                // If the column has not been finished with decompressing
                if (isValid[i]) {
                    // Write the data to the output file
                    output << rows[i].data_value_;
                }
                if (i < n-1) output << ',';
                else output << std::endl;
                rows[i].run_length_--;
            }
            --minVal;
        }

        // Repeat decoding step for the rest of the rows
        minVal = std::numeric_limits<unsigned int>::max();
        for (size_t i=0; i<n; i++) {

                if (rows[i].run_length_ == 0) {
                    if (std::getline(*streams[i], line)) {
                        if(isCompressed[i]){
                            short len = line.length();std::cout<<"READ!!!!!";
                            bytes[0] = line.at(0);
                            bytes[1] = len>=2 ? line.at(1) : 0;std::cout<<"----------> "<<line.length()<<std::endl;
                            bytes[2] = len>=3 ? line.at(2) : 0;
                            bytes[3] = len>=4 ? line.at(3) : 0;
                            bytes[4] = len>=5 ? line.at(4) : 0;

                            // Decode the byte array using Variable-Byte Decoding
                            nextPos = __integer_encoding::VariableByte__Decode(bytes, &runLength);
                            rows[i].run_length_ = runLength;
                            rows[i].data_value_ = line.substr(nextPos, line.length()-nextPos);


                        } else {
                            //again, the column is not compressed, so each run has a length of 1
                            runLength = 1;
                            // Set the flag to indicate that the end has been reached
                            rows[i].run_length_ = runLength;
                            // Increment the number of finished columns
                            rows[i].data_value_ = line;
                        }
                    } else {
                        // The last row has been decoded -- close the stream
                        streams[i]->close();
                        isValid[i] = false;
                        invalidCounter++;
                    }
                }
                if(rows[i].run_length_ < minVal) {
                    minVal = rows[i].run_length_;
                }

        }
    }
    output.close();

    // Delete the pointers and free the memory
    for (size_t i=0; i<streams.size(); i++) {
        delete streams[i];
    }
}
