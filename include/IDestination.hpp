#pragma once
//
// C++ STL
//
#include <string>
namespace JSONLib
{
    //
    // Interface for writing destination stream during JSON stringification.
    //
    class IDestination
    {
    public:
        virtual ~IDestination() = default;
        virtual void add(const std::string &bytes) = 0;
        virtual void add(const char ch) = 0;
    };
} // namespace JSONLib
