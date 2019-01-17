//----------------------------------------------------------------------------
// Licensed under the MIT/X11 license - https://opensource.org/licenses/MIT
//----
// Copyright (c) 2019, Codalogic Ltd (http://www.codalogic.com)
//
// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included
// in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
// THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.
//----------------------------------------------------------------------------

#if defined(_MSC_VER)
// Require error when nonstandard extension used :
//		'token' : conversion from 'type' to 'type'
//		e.g. disallow foo( /*const*/ foo & ) for copy constructor
#pragma warning(error: 4239)
#endif

#include "annotate-lite.h"
using namespace annotate_lite;

#include <string>
#include <iostream>

// A simplified version of std::optional
template< class T >
class MyOptional
{
    T v;
    bool has_v;

public:
    MyOptional() : has_v( false ) {}
    MyOptional( const T & v_in ) : v(v_in), has_v( true ) {}
    bool has_value() const { return has_v; }

    // 'Standard' value_or( U&& ) implementations based on https://en.cppreference.com/w/cpp/utility/optional/value_or
    template< class U >
    constexpr T value_or( U&& default_value ) const&
    {
        if( has_v )
            return v;
        return static_cast<T>(std::forward<U>(default_value));
    }
    template< class U >
    constexpr T value_or( U&& default_value ) &&
    {
        if( has_v )
            return std::move(v);
        return static_cast<T>(std::forward<U>(default_value));
    }

    // New variants of value_or() throwing user specified exceptions
    template< class U >
    constexpr T& value_or() &
    {
        if( ! has_v )
            throw U();
        return v;
    }
    template< class U >
    constexpr const T & value_or() const &
    {
        if( ! has_v )
            throw U();
        return v;
    }
    template< class U >
    constexpr T&& value_or() &&
    {
        if( ! has_v )
            throw U();
        return std::move(v);
    }
    template< class U >
    constexpr const T&& value_or() const &&
    {
        if( ! has_v )
            throw U();
        return std::move(v);
    }
};

template< int T >
class Bail {};

MyOptional<int> maybe_int()
{
    return MyOptional<int>();
}

void using_struct_errors()
{
    struct OptOut {};
    try
    {
        MyOptional<int> mi = maybe_int();
        std::cout << mi.value_or(101) << "\n";
        std::cout << mi.value_or<OptOut>() << "\n";
        Bad( "using_struct_errors() : Should have thrown" );
    }
    catch( const OptOut & )
    {
        Good( "using_struct_errors() : Throw done" );
    }
}

// This says what we want to do
void using_bail_template_class()
{
    // This captures the problems we might encounter
    enum Bailouts { Error1, Error2 };

    // This shows how we attempt to do it
    try
    {
        int i1 = maybe_int().value_or< Bail<Error1> >();
        Bad( "using_bail_template_class() : Should have thrown" );
        int i2 = maybe_int().value_or< Bail<Error2> >();
    }

    // This shows how we pick up the pieces if it goes wrong
    catch( const Bail<Error1> & )
    {
        Good( "using_bail_template_class() : Error1 thrown" );
    }
    catch( const Bail<Error2> & )
    {
        Bad( "using_bail_template_class() : Error2 thrown. Should have thrown Error1 instead" );
    }
}

void using_bail_template_class_again()  // To test there's no conflicts when compiling
{
    enum Bailouts { Error1, Error2 };
    try
    {
        MyOptional<int> mi = maybe_int();
        std::cout << mi.value_or< Bail<Error2> >() << "\n";
        Bad( "using_bail_template_class_again() : Should have thrown" );
    }
    catch( const Bail<Error1> & )
    {
        Bad( "using_bail_template_class_again() : Error1 thrown. Should have thrown Error2 instead" );
    }
    catch( const Bail<Error2> & )
    {
        Good( "using_bail_template_class_again() : Error2 thrown" );
    }
}

int main( int argc, char * argv[] )
{
    using_struct_errors();
    using_bail_template_class();
    using_bail_template_class_again();

    Report();
    
    return 0;
}
