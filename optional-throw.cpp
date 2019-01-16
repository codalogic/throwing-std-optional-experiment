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

template< class T >
class MyOptional
{
    T v;
    bool has_v;

public:
    MyOptional() : has_v( false ) {}
    MyOptional( const T & v_in ) : v(v_in), has_v( true ) {}
    bool has_value() const { return has_v; }
    T value_or( const T & other ) const
    {
        if( has_v )
            return v;
        return other;
    }
    template< class U >
    T value_or()
    {
        if( ! has_v )
            throw U();
        return v;
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
    enum Bailouts { Res1, Res2 };

    // This shows how we attempt to do it
    try
    {
        MyOptional<int> mi = maybe_int();
        std::cout << mi.value_or< Bail<Res1> >() << "\n";
        Bad( "using_bail_template_class() : Should have thrown" );
    }

    // This shows how we pick up the pieces if it goes wrong
    catch( const Bail<Res1> & )
    {
        Good( "using_bail_template_class() : Res1 thrown" );
    }
    catch( const Bail<Res2> & )
    {
        Bad( "using_bail_template_class() : Res2 thrown. Should have thrown Res1 instead" );
    }
}

void using_bail_template_class_again()  // To test there's no conflicts when compiling
{
    enum Bailouts { Res1, Res2 };
    try
    {
        MyOptional<int> mi = maybe_int();
        std::cout << mi.value_or< Bail<Res2> >() << "\n";
        Bad( "using_bail_template_class_again() : Should have thrown" );
    }
    catch( const Bail<Res1> & )
    {
        Bad( "using_bail_template_class_again() : Res1 thrown. Should have thrown Res2 instead" );
    }
    catch( const Bail<Res2> & )
    {
        Good( "using_bail_template_class_again() : Res2 thrown" );
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
