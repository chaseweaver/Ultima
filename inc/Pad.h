#ifndef PAD_H
#define PAD_H

#ifdef _MSC_VER
#pragma once
#endif

#include <iostream>
#include <string>

/*
 * Pad::pad(T, T, T)
 * Pads a Template variable.
 */
template < typename T >
void pad(std::basic_string< T >& s, typename std::basic_string< T >::size_type n, T c) {
  if (n > s.length()) s.append(n - s.length(), c);
};

#endif