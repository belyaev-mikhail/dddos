/*
 * Nothing.hpp
 *
 *  Created on: Feb 27, 2014
 *      Author: belyaev
 */

#ifndef NOTHING_HPP_
#define NOTHING_HPP_

namespace callophrys {
namespace util {

struct nothing_t {};

inline nothing_t nothing() {
    return nothing_t{};
}

} /* namespace util */
} /* namespace calliphrys */



#endif /* NOTHING_HPP_ */
