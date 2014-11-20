#include<vector>


#include "CsvRow.h"
#include "Global.h"

using namespace std;

/* Memory-optimized row data holder technique. */
void CsvRow::set_row_data(vector<std::string> row_data)
{
    // Clear to make sure
    row_data_.clear();

    // Using boost::flyweights ensures elimination of duplicate values and saves lots of memory
    for(int i=0; i<row_data.size(); i++){
        row_data_.push_back(boost::flyweights::flyweight<std::string>(row_data[i]));
    }
        //row_data_.push_back(row_data[i]);
}

/* Pull-out the data from the boost::flyweights which were
 * used above in order to reduce memory consumption.
 */
std::vector<std::string> CsvRow::get_row_data()
{
    // Flyweights are now put into strings
    std::vector<std::string> to_return;

    // Get data from each flyweight
    for (int i=0; i<row_data_.size(); i++) {
        to_return.push_back(row_data_[i].get());
    }
    return to_return;
}

/* Assignment operator overload for CsvRow -- self-explanatory */
CsvRow& CsvRow::operator=(const CsvRow& rhs)
{
    if (this != &rhs) {
        row_data_ = rhs.row_data_;
    }
    return *this;
}

/* Adress-offset access operator overload -- provides an easy
 *  array-like abstraction layer on top of CsvRow.
 */
std::string const& CsvRow::operator[](std::size_t index) const
{
    boost::flyweights::flyweight<std::string> fly = row_data_[index];
    return fly.get();
}

/* Returns the number of columns */
std::size_t CsvRow::size() const
{
    return row_data_.size();
}

/* Read the next row from where the iterator is currently poining in the corresponding CSV file. */
void CsvRow::ReadNextRow(std::istream& stream)
{
    // Read the first line
    std::string line;
    std::getline(stream, line);

    // Init a stringstream for splitting cells later
    std::string cell;
    size_t pos = 0;

    // Clear for sure
    row_data_.clear();


    // Read next line and split into an array using the provided delimiter
    // positions as cut-points
    while((pos = line.find(delimiter)) != std::string::npos) {
        cell = line.substr(0, pos);
        row_data_.push_back(boost::flyweights::flyweight<std::string>(cell));
        line.erase(0, pos + delimiter.length());
    }
    row_data_.push_back(boost::flyweights::flyweight<std::string>(line));
}

