#ifndef MWAV_EXCEPTION_H_
#define MWAV_EXCEPTION_H_

#include <exception>
#include <string>

namespace mwav {
/**
 * @brief Exception class for MWAV.
 * @details Contains an exception message string that can be retrieved with
 * what().
 */
class Exception : public std::exception {
 public:
  Exception(std::string message) : message_(message) {}
  const char* what() const throw() { return message_.c_str(); }

 private:
  std::string message_;
};
}  // namespace mwav

#endif  // MWAV_EXCEPTION_H_