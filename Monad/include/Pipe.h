#pragma once

#include <boost/optional/optional.hpp>

#include <tuple>

namespace detail
{
   /** Non-tuples and tuples with more than 1 entry
       are forwarded as they are.
    */
   template <typename V>
   static auto normalize(V&& v) 
   {  return std::forward<V>(v); }

   /** Tuples with exactly 1 entry are transformed
       to the plain value.
    */
   template <typename V>
   static auto normalize(std::tuple<V>&& v) -> V 
   {  return std::forward<V>(std::get<0>(v)); }
   
   template <typename V>
   static auto normalize(boost::optional<std::tuple<V>>&& v) -> boost::optional<V> 
   {
      if (v)
      {  return boost::make_optional(normalize(*v)); }
      return boost::optional<decltype(normalize(*v))>( /*empty*/ );
   }
}

/** Argument is moved into function and return value
    of the function is moved further. Function transforms
    the given value.
   
    Non-void function version.
 */
template <typename T, typename F>
auto operator | (T&& t, F&& f) -> typename std::enable_if<!std::is_void<decltype(detail::normalize(f(t)))>::value, decltype(detail::normalize(f(t)))>::type 
{  return std::forward<decltype(detail::normalize(f(t)))>(detail::normalize(f(std::forward<T>(t)))); }

/** When the argument is optional, the function is
    called only when the argument is set.

    Non-void function version for optional arguments.
 */
template <typename T, typename F>
auto operator | (boost::optional<T>&& t, F&& f) -> typename std::enable_if<!std::is_void<decltype(detail::normalize(f(*t)))>::value, boost::optional<decltype(operator|(*t, f))>>::type
{  
   if (t)
   {  return boost::make_optional(operator|(*t, f)); }
   return boost::optional<decltype(operator|(*t, f))>( /*empty*/ );
}

/** Function is called with a copy of the argument.
    The argument is forwarded to the next.
    
    \remark We don't have to normalize here, because the argument is forwarded and not changed, hence it is already normalized
   
    Void function version.
 */
template <typename T, typename F>
auto operator | (T&& t, F&& f) -> typename std::enable_if<std::is_void<decltype(f(t))>::value, T>::type
{  
   auto const& _t(t);
   f(_t); ///< Force const reference as parameter
   return std::forward<T>(t); //< forward parameter
} 

/** Function is called when the optional is present with a copy of the value.
    The optional argument is forwarded to the next.
    
    \remark We don't have to normalize here, because the argument is forwarded and not changed, hence it is already normalized
   
    Void function version for optional arguments.
 */
template <typename T, typename F>
auto operator | (boost::optional<T>&& t, F&& f) -> typename std::enable_if<std::is_void<decltype(f(*t))>::value, boost::optional<T>>::type
{  
   if (t) 
   {  operator|(*t, f); } 
   return std::forward<boost::optional<T>>(t); //< forward parameter
}
