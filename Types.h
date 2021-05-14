#ifndef TYPES_H
#define TYPES_H

#include <type_traits>
#include <limits.h>
#include <string.h>
#include <math.h>


    template<typename TYPE> const TYPE POISON;

    template<> constexpr double POISON<double> = NAN;
    template<> constexpr float  POISON<float>  = NAN;
    template<> constexpr int    POISON<int>    = INT_MAX;
    template<> constexpr size_t POISON<size_t> = UINT_MAX;
    template<> constexpr char   POISON<char>   = '\0';
    template<> constexpr char*  POISON<char*>  = nullptr;


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
    template<> const char* PRINT_FORMAT<size_t> = "0x%p";
    template<> const char* PRINT_FORMAT<char>   = "%c";
    template<> const char* PRINT_FORMAT<char*>  = "%s";


//------------------------------------------------------------------------------
/*! @brief   Check if value is POISON.
 *
 *  @param   value       Value to be checked
 *
 *  @return 1 if value is POISON, else 0
 */

    template <typename TYPE>
    int isPOISON (TYPE value)
    {
        if (value == POISON<TYPE>) return 1;

        if (isnan(*(double*)&POISON<TYPE>))
            if (isnan(*(double*)&value))
                return 1;
            else
                return 0;

        return (value == POISON<TYPE>);
    }

//------------------------------------------------------------------------------
/*! @brief   Ñopy the contents of one variable to another.
 *
 *  @param   dst         Destination variable
 *  @param   src         Source variable
 */

    #define copyType(dst, src)                              \
            if constexpr (std::is_same<TYPE, char*>::value) \
                strcpy(dst, src);                           \
            else                                            \
                dst = src;                                  \

//------------------------------------------------------------------------------



#endif // TYPES_H
