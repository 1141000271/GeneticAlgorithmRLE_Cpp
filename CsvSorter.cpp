#include "CsvSorter.h"

std::vector<bool> IS_WORTH_COMPRESSING;

/* Default constructor.  Initializes everything to zero. */
CsvSorter::CsvSorter() : rows_(0), columns_(0), compressed_size_(0){ }

/* Constructor -- additionally assign a CSV filename */
CsvSorter::CsvSorter(std::string file_name)
{
    file_name_ = file_name;
    rows_ = 0;
    columns_ = 0;
    compressed_size_ = 0;

}

/* Default destructor. Does nothing... */
CsvSorter::~CsvSorter()
{
}

/* Self-explanatory getter */
size_t CsvSorter::get_columns()
{
    return columns_;
}

size_t CsvSorter::get_rows()
{
    return rows_;
}

std::vector<__gnu_cxx::hash_set<std::string, std::hash<std::string> > >& CsvSorter::get_distinct_values()
{
    return distinct_values_;
}

/* Self-explanatory getter */
std::string CsvSorter::get_file_name()
{
    return file_name_;
}

/* Self-explanatory getter */
uintmax_t CsvSorter::get_compressed_size()
{
    return compressed_size_;
}

/* Counts the rows in the provided CSV file member. Reads the file
 * line by line and increments a row counter. Very simple.
 */
void CsvSorter::CountRows()
{
    std::ifstream file(file_name_);
    std::string line;

    // While there are still lines to be reads
    while(getline(file, line))
    {
        // Increment the counter
        rows_++;
    }
    file.close();
}

std::string CsvSorter::exec(const char* cmd)
{
    FILE* pipe = popen(cmd, "r");
    if(!pipe) return "ERROR";
    char buffer[128];
    std::string result = "";
    while(!feof(pipe)) {
        if(fgets(buffer, 128, pipe) != NULL) {
            result += buffer;
        }
    }
    pclose(pipe);
    return result;
}

/*  Reads the whole CSV file and then analyzes the data in the
 *  following manner:
 *
 *  1. First, each table cell is added into a hash-set that
 *  helps us count the number of distinct value in each column,
 *  i.e we caluclate cardinalities for each column.
 *
 *  2. Then, according to our heuristic approach in generating
 *  the solution -- columns are ordered with respect to their
 *  cardinality, starting from the lowest-cardinality column.
 *  This information is stored in the sorting_order_ vector.
 *
 *  3. Each column is then set to be sorted in ascending order
 *  which is denoted by "1". This information is stored in
 *  the sorting_direction_ vector.
 */
void CsvSorter::ReadAndAnalyzeData(std::string filename, int pop_size, int number_generations)
{

    // Assign data to a vector of CsvRow objects
    all_rows_ = std::vector<CsvRow>(rows_);

    // Instantiate a stream to the input CSV file
    std::ifstream file(file_name_);

    // We keep a hash-set for each column, so we have a vector
    int k=0;
    for (CsvIterator it(file); it != CsvIterator(); ++it) {

        // If it happens to be the first column...
        if(columns_ == 0) {

            // Update the number of columns
            columns_ = (*it).size();

            // Create a new hssh-set for each column and push it to a vector container
            for (unsigned int i=0; i<columns_; i++) {
                distinct_values_.push_back(__gnu_cxx::hash_set<std::string, std::hash<std::string> >());
            }
        }

        // Insert each cell to its column-wise hash-set
        // Note that duplicates are not really inserted!
        for (unsigned int i=0; i<columns_; i++) {
            distinct_values_[i].insert((*it)[i]);
        }

        // Insert the current row in the data container
        all_rows_[k]=(*it);
        k++;
    }
    std::cout<< "All data read." << std::endl;
    std::string command = "CsvCompression2_Threshold_GA.exe ";
    std::string filenamearg = "-filename " + filename + " ";
    std::string ga_args = "-popsize " + std::to_string(pop_size)    + " -numgen " + std::to_string(number_generations) + " ";
    std::string cardinalityarg = "-cardinality ";
    std::string rowsarg = "-rows ";
    std::string columnsarg = "-columns ";
    command += filenamearg;
    command += ga_args;
    columnsarg += std::to_string(columns_);
    columnsarg += " ";
    rowsarg += std::to_string(rows_);
    rowsarg += " ";
    command += rowsarg;
    command += columnsarg;
    for(int i = 0; i < distinct_values_.size(); i++) {
            cardinalityarg += std::to_string(distinct_values_[i].size());
            cardinalityarg += " ";
    }
    command += cardinalityarg;
    std::cout << command << std::endl;
    std::vector<int> ga_results;
    std::string result = exec(command.c_str());
    std::stringstream str_parser(result);
    int val;
    while(str_parser >> val) {
        ga_results.push_back(val);
        if(str_parser.peek() == ',' || str_parser.peek() == ' ')
            str_parser.ignore();
    }
    std::cout << "RESULT: " << result<< std::endl;
    for(int i = 0; i < ga_results.size(); i++)
        std::cout << "GaRes: " + ga_results[i] << std::endl;
    // Create the sorting order and direction vectors
    if(ga_results.size() == 2 * columns_) {
        int j=0;
        for(size_t i=0; i<columns_; i++){
            sorting_order_.push_back(ga_results[j]);
            sorting_direction_.push_back(ga_results[j+1]);
            j+=2;
        }
    } else {
        for(size_t i=0; i<columns_; i++){
            sorting_order_.push_back(i);
            sorting_direction_.push_back(0);
        }
    }

    std::cout<<"Sorting order: ";
    for(size_t i=0; i<columns_; i++){
        std::cout << sorting_order_[i] << " ";
    }
    std::cout<<std::endl<<"Sorting direction: ";
    for(size_t i=0; i<columns_; i++){
        std::cout << sorting_direction_[i] << " ";
    }
    std::cout<<std::endl;

    file.close();
}

/* This function performs a Quick-Sort using std::sort()
 *  according to the order in which the columns are to be
 *  sorted. As stated before, we first sort the rows by the
 *  lowest cardinality column and then "recursively" we sort
 *  by the next smallest cardinality column,
 *  The SortRowsStruct is used to achieve this.
 *  It also makes the threshold-based decision on compression.
 */
void CsvSorter::SortData()
{
    SortRowsStruct s(this);

    std::sort(all_rows_.begin(), all_rows_.end(), s);

    // Compression threshold decision
    IS_WORTH_COMPRESSING.resize(columns_);
    boost::thread_group group;
    for (size_t i=0; i<columns_; ++i) {
        group.create_thread(boost::bind(&CsvSorter::IsWorthCompressing, this, i));
    }
    group.join_all();
}


/*  Performs RLE (Run-Length Encoding) compression over the
 *  column that was passed as an argument into the functions.
 *
 *  We used the optimized storage format described in our
 *  paper, i.e "lengthValue".
 *
 *  Length is initially a 32-bit unsigned integer, but is then
 *  encoded using Variable-Byte Encoding which keeps only enough
 *  bytes (1,2,3 or 4) to represent the value, instead of keeping
 *  4 bytes in all cases.
 *
 *  Regarding RLE, each value is saved and then each successive
 *  cell in the same column is compared to it. If they are equal
 *  then the current run length is incremented by 1, else the run
 *  has finished and it is encoded using VBE. Together with the
 *  current run value, it is written to the output file for the
 *  corresponding column.
 *
 *  NOTE: This function is used in CsvCompressor::CompressColumn()
 *  which is run in a single separate thread to achieve high-speed
 *  parallelized RLE compression.
 */
void CsvSorter::ColumnRle(int column_index)
{
    // Initialize some useful variables
    std::string fileName = file_name_;
    fileName += "_";
    std::stringstream out;
    out << column_index;
    fileName += out.str();
    fileName += ".csvcomp";

    // Create an output file in which we store the compressed column
    std::ofstream output(fileName, std::ios::out | std::ios::binary);

    if (IS_WORTH_COMPRESSING[column_index]) {
        size_t i;
        unsigned int runLength = 0;
        RleDataStruct data;


        for (i=0; i<rows_-1; i++) {
            if (all_rows_[i][column_index] != all_rows_[i+1][column_index]) {
                runLength++;
                data.data_value_ = all_rows_[i][column_index];
                data.run_length_ = runLength;
                output << data << std::endl;
                runLength = 0;
            } else {
                runLength++;
            }
        }
        if (all_rows_[i][column_index] == all_rows_[i-1][column_index]) {
            data.data_value_ = all_rows_[i][column_index];
            data.run_length_ = runLength + 1;
            output << data << std::endl;
        } else {
            data.data_value_ = all_rows_[i][column_index];
            data.run_length_ = 1;
            output << data << std::endl;
        }
    } else {
        for (size_t i=0; i<rows_; i++) {
            output << all_rows_[i][column_index] << std::endl;
        }
    }

    output.close();

    boost::filesystem::path outPath(fileName);
    if(exists(outPath)) compressed_size_ += boost::filesystem::file_size(outPath);

}

/* Threshold-based decision on compression of a single column.
 * Detailed explanation can be found in the paper (Section 4)
 */
void CsvSorter::IsWorthCompressing(int column_index)
{
    BinaryIndexedTree bit(rows_);
    Queue1MB queue1Mb;

    uint32_t len_curr, len_prev;
    uint32_t r = 1;
    uint16_t s = 0;
    uint8_t o = 0;

    std::string prev, curr;
    int idx = 1;
    bool res;
    len_prev = all_rows_[0][column_index].length();
    prev = all_rows_[0][column_index];

    //Iterate over each row and determine the runs
    for (size_t i=1; i<rows_; i++) {
        len_curr = all_rows_[i][column_index].length();
        curr = all_rows_[i][column_index];

        if (prev == curr) {
            r++;
            len_prev = len_curr;
            prev = curr;
        } else {
            if (r < 128) o = 1;
            else if (r >= 128 && r < 16385 ) o = 2;
            else if (r >= 16385 && r < 2097153) o = 3;
            else o = 4;

            s = len_prev;

            // if the queue is full, pop everything and add it to the BIT
            RunInfo rInfo;
            rInfo.run_length_x_string_length = r * s;
            rInfo.string_length = len_prev;
            rInfo.run_overhead = o;
            bit.add(idx, rInfo);
            ++idx;

            //Reset the run information and begin analyzing the next run
            r = 1;
            s = len_curr;
            o = 0;

            // Special case: We have a single run at the last row
            len_prev = len_curr;
            prev = curr;
        }
    }

    if (r < 128) o = 1;
    else if (r >= 128 && r < 16385 ) o = 2;
    else if (r >= 16385 && r < 2097153) o = 3;
    else o = 4;
    RunInfo rInfo;
    rInfo.run_length_x_string_length = r * s;
    rInfo.run_overhead = o;
    rInfo.string_length = len_curr;
    //queue1Mb.enqueue(rInfo);

    //bit.resizeAdd(1);
    bit.add(idx, rInfo);
    idx++;
    // We now have a complete Binary Indexed Tree...
    // Decide whether compression is worthy
    unsigned int totalColumnSize, totalColumnOverhead, totalSqueezedSize;
    size_t treeSize = bit.get_treesize();
    std::string totalCommand("total_size");
    std::string overheadCommand("run_overhead");
    std::string stringCommand("string_length");
    totalColumnSize = bit.read(treeSize - 1, totalCommand);
    totalColumnOverhead = bit.read(treeSize - 1, overheadCommand);
    totalSqueezedSize = bit.read(treeSize - 1, stringCommand);

    // Main compression decision point
    if (totalColumnSize <= totalSqueezedSize + totalColumnOverhead)
        // This column is not worth compressing
        IS_WORTH_COMPRESSING[column_index] = false;
    else
        // This column is worth compressing
        IS_WORTH_COMPRESSING[column_index] = true;
    for(size_t i =0; i < IS_WORTH_COMPRESSING.size(); i++)
        std::cout << IS_WORTH_COMPRESSING[i];
    std::cout << std::endl;
}
