/*
 * memory.hpp
 *
 *      Author: belyaev
 */

#ifndef MEMORY_HPP_
#define MEMORY_HPP_

namespace callophrys {
namespace util {

template<class T>
std::unique_ptr<T> unique(T* ptr) { return std::unique_ptr<T>{ ptr } }

template<class T>
std::unique_ptr<const T> unique(const T* ptr) { return std::unique_ptr<const T>{ ptr } }

template<class T, class ...Args>
std::unique_ptr<T> make_unique(Args&&... args) { return unique(new T{ std::forward<Args>(args)... }); }

} /* namespace util */
} /* namespace callophrys */

#endif /* MEMORY_HPP_ */