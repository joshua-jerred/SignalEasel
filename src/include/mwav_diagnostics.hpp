/**
 * @file mwav_diagnostics.hpp
 * @author Joshua Jerred (https://joshuajer.red)
 * @brief MWAV exception and error codes.
 * @date 2023-07-22
 * @copyright Copyright (c) 2023
 */

#ifndef MWAV_DIAGNOSTICS_HPP_
#define MWAV_DIAGNOSTICS_HPP_

namespace mwav {

/**
 * @brief Exception class for MWAV.
 * @details Contains an exception message string that can be retrieved with
 * what().
 */
class Exception : public std::exception {
 public:
  Exception(std::string message) : message_(message) {
  }
  const char *what() const throw() {
    return message_.c_str();
  }

 private:
  std::string message_;
};

}  // namespace mwav

#endif /* MWAV_DIAGNOSTICS_HPP_ */