#ifndef CSVITERATOR_H_INCLUDED
#define CSVITERATOR_H_INCLUDED

#include "CsvRow.h"


class CsvIterator
{
    public:
        typedef std::input_iterator_tag    iterator_category;
        typedef CsvRow                     value_type;
        typedef std::size_t                difference_type;
        typedef CsvRow*                    pointer;
        typedef CsvRow&                    reference;

        /* Constructors */
        CsvIterator();
        CsvIterator(std::istream&);
        ~CsvIterator();

        /* Utility opeators */
        CsvIterator& operator++();
        CsvIterator operator++(int);
        CsvRow const& operator*() const;
        CsvRow const* operator->() const;

        /* Logical operators */
        bool operator==(CsvIterator const&);
        bool operator!=(CsvIterator const&);

        /* Setters and getters */
        void set_stream(std::istream*);
        void set_row(CsvRow);
        std::istream* get_stream();
        CsvRow get_row();

    private:
        // Stream to a csv file
        std::istream* stream_;

        // Csv file row abstraction
        CsvRow row_;
};

#endif // CSVITERATOR_H_INCLUDED
