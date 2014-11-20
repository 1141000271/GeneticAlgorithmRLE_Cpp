#include <vector>
#include <boost/flyweight.hpp>
#include "CsvIterator.h"

/* Default constructor. Does nothing... */
CsvIterator::CsvIterator() : stream_(NULL) {}

/* Constructor using a stream to CSV file */
CsvIterator::CsvIterator(std::istream& stream)
{
    if (stream.good()) stream_ = &stream;
    else stream_ = NULL;

    ++(*this);
}

/* Default destructor. Does nothing... */
CsvIterator::~CsvIterator()
{
     //stream_->close();
}

/* Pre-increment operator overload -- read next row from CSV */
CsvIterator& CsvIterator::operator++()
{
    if (stream_) {
        (*stream_) >> row_;
        if (!stream_->good()) stream_ = NULL;
    }
    return *this;
}

/* Post-increment operator overload -- same as above */
CsvIterator CsvIterator::operator++(int)
{
    CsvIterator tmp(*this);
    ++(*this);
    return tmp;
}

/* Unary * operator overload -- get the current row */
CsvRow const& CsvIterator::operator*() const
{
    return row_;
}

/* Class-member access operator overload -- returns a pointer to the current row in the CSV file.*/
CsvRow const* CsvIterator::operator->() const
{
    return &row_;
}

/* Equals operator overload -- compare two streams(CsvIterators) */
bool CsvIterator::operator==(CsvIterator const& rhs)
{
    // Check if both are equal and non-null
    bool both_equal = (this == &rhs);
    // Or if both are null
    bool both_null = ((stream_ == NULL) && (rhs.stream_ == NULL));

    return both_equal || both_null;
}

/* Self-explanatory overload */
bool CsvIterator::operator!=(CsvIterator const& rhs)
{
    return !((*this) == rhs);
}

/* Self-explanatory setter */
void CsvIterator::set_stream(std::istream* stream) {stream_ = stream;}

/* Self-explanatory setter */
void CsvIterator::set_row(CsvRow row) {row_ = row;}

/* Self-explanatory getter */
std::istream* CsvIterator::get_stream() {return stream_;}

/* Self-explanatory getter */
CsvRow CsvIterator::get_row() {return row_;}

