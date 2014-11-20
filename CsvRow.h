/** A class representing a single row in a csv formatted file.
 *  Implements basic manipulation of CSV files.
 */

/* CsvRow uses the method ReadNextRow to read a single row
    from a .csv file and splits it into a vector of strings
    which represents the cells in a single row, separated by
    a special delimiter
*/

#ifndef CSVROW_H_INCLUDED
#define CSVROW_H_INCLUDED

#include <iterator>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>

#include <boost/flyweight.hpp>


class CsvRow
{
    public:
        /* Setters and getters */
        void set_row_data(std::vector<std::string>);
        std::vector<std::string> get_row_data(void);

        /* Utility and logical operators */
        CsvRow& operator=(const CsvRow &rhs);
        std::string const& operator[](std::size_t) const;
        std::size_t size() const;

       /* A function to iterate to the next row and read the data in the file associated with
        * the given stream
        */
        void ReadNextRow(std::istream&);

    private:
        // Holder for the row data that uses flyweight to reduce memoryconsumption
        std::vector<boost::flyweights::flyweight<std::string> > row_data_;
};

/* Input stream operator to read the next row from the stream */
inline std::istream& operator>>(std::istream& stream, CsvRow& csv_row)
{
    csv_row.ReadNextRow(stream);
    return stream;
}

#endif // CSVROW_H_INCLUDED

