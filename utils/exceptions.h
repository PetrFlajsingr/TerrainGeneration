//
// Created by petr on 10/31/19.
//

#ifndef TERRAINGENERATION_EXCEPTIONS_H
#define TERRAINGENERATION_EXCEPTIONS_H

#include <exception>
#include <experimental/source_location>
#include <string>

namespace exc {
class Throwable : public std::exception {
public:
  explicit Throwable(std::string_view msg,
               std::experimental::source_location srcLoc =
                   std::experimental::source_location::current());
  [[nodiscard]] const char *what() const noexcept override;

protected:
  std::string what_;
};

class Error : public Throwable {
  using Throwable::Throwable;
};
class Exception : public Throwable {
  using Throwable::Throwable;
};

class ProgrammingError : public Error {
public:
  explicit ProgrammingError(std::string_view msg);
};

class InternalError : public Error {
  using Error::Error;
};

}

#endif // TERRAINGENERATION_EXCEPTIONS_H
