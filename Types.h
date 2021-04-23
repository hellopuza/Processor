#include <limits.h>
#include <math.h>

#ifndef TYPES_H
#define TYPES_H

    template<typename TYPE> const TYPE POISON;

    template<> const double POISON<double> = NAN;
    template<> const float  POISON<float>  = NAN;
    template<> const int    POISON<int>    = INT_MAX;
    template<> const size_t POISON<size_t> = UINT_MAX;
    template<> const char   POISON<char>   = '\0';
    template<>       char*  POISON<char*>  = nullptr;


    template<typename TYPE> const char* PRINT_TYPE;

    template<> const char* PRINT_TYPE<double> = "double";
    template<> const char* PRINT_TYPE<float>  = "float";
    template<> const char* PRINT_TYPE<int>    = "int";
    template<> const char* PRINT_TYPE<size_t> = "size_t";
    template<> const char* PRINT_TYPE<char>   = "char";
    template<> const char* PRINT_TYPE<char*>  = "char*";


    template<typename TYPE> const char* PRINT_FORMAT;

    template<> const char* PRINT_FORMAT<double> = "%lf";
    template<> const char* PRINT_FORMAT<float>  = "%f";
    template<> const char* PRINT_FORMAT<int>    = "%d";
    template<> const char* PRINT_FORMAT<size_t> = "0x%08X";
    template<> const char* PRINT_FORMAT<char>   = "%c";
    template<> const char* PRINT_FORMAT<char*>  = "%s";

#endif // TYPES_H