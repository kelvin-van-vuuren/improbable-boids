#ifndef RANDOM_H
#define RANDOM_H

#include <random>
#include <type_traits>

// Magic to infer Integer/Real distribution from the type parameter
// https://stackoverflow.com/a/55275707/14789685
template <class T>
using uniform_distribution = typename std::conditional<
    std::is_floating_point<T>::value, std::uniform_real_distribution<T>,
    typename std::conditional<std::is_integral<T>::value,
                              std::uniform_int_distribution<T>,
                              void>::type>::type;

namespace Random {

std::mt19937_64 &getEngine() {

  static thread_local std::random_device rd;
  static thread_local std::mt19937_64 engine(rd);
  return engine(rd);
}

template <typename T> T generateNumberBetween(T min, T max) {
  uniform_distribution<T> distribution(min, max);
  return distribution(getEngine());
}

} // namespace Random

#endif // RANDOM_H
