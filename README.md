# throwing-std-optional-experiment

An experiment allowing the calling function to choose the exception thrown
when std::optional.value() is called without a value, rather than having
the exception hard coded to `std::bad_optional_access`.

Additional methods are:

```cpp
template< class U >
constexpr T& value_or() &;

template< class U >
constexpr const T & value_or() const &;

template< class U >
constexpr T&& value_or() &&;

template< class U >
constexpr const T&& value_or() const &&;
```

They are used as:

```cpp
struct Badint {};
try
{
    MyOptional<int> mi = maybe_int();
    int i = mi.value_or<Badint>();
}
catch( const Badint & )
{
    // Handle maybe_int() not returning an int
}
```

A `Bail` template class of the form:

```cpp
template< int T >
class Bail {};
```

allows specifying error conditions in a more concise way using enums:

```cpp
// This captures the problems we might encounter
enum Bailouts { Error1, Error2 };

// This shows how we attempt to do it
try
{
    int i1 = maybe_int().value_or< Bail<Error1> >();
    int i2 = maybe_int().value_or< Bail<Error2> >();
}

// This shows how we pick up the pieces if it goes wrong
catch( const Bail<Error1> & )
{
    //...
}
catch( const Bail<Error2> & )
{
    //...
}
```
