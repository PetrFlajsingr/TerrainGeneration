#pragma clang diagnostic push
#pragma ide diagnostic ignored "cppcoreguidelines-pro-type-member-init"
//
// Created by Petr Flajsingr on 2019-01-25.
//

#ifndef CSV_READER_LOGGER_H
#define CSV_READER_LOGGER_H

#include <chrono>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <now.h>

/**
 * Types of log messages
 */
enum class LogLevel { Verbose, Info, Status, Debug, Warning, Error };

namespace LoggerStreamModifiers {
struct verbose{};
struct info{};
struct status{};
struct debug{};
struct warning{};
struct error{};
struct flush{};
struct out{};
struct err{};

template <typename T>
constexpr LogLevel modifierToLogLevel() {
  if constexpr (std::is_same_v<T, verbose>) {
    return LogLevel::Verbose;
  }
  if constexpr (std::is_same_v<T, info>) {
    return LogLevel::Info;
  }
  if constexpr (std::is_same_v<T, status>) {
    return LogLevel::Status;
  }
  if constexpr (std::is_same_v<T, debug>) {
    return LogLevel::Debug;
  }
  if constexpr (std::is_same_v<T, warning>) {
    return LogLevel::Warning;
  }
  if constexpr (std::is_same_v<T, error>) {
    return LogLevel::Error;
  }
}

template <typename T>
constexpr bool is_logger_stream_modifier_v = is_one_of_v<T, verbose, info, status, debug, warning, error>;
template<typename T>
constexpr bool is_logger_flusher_v = std::is_same_v<flush, T>;
template<typename T>
constexpr bool is_stream_v = is_one_of_v<T, out, err>;
}

template <bool EnableDebug> class Logger {
private:
  /**
   *
   * @return Current time as HH-MM-SS
   */
  [[nodiscard]] std::string getTime() const {
    auto t = std::time(nullptr);
    auto tm = *std::localtime(&t);
    std::stringstream ss;
    ss << std::put_time(&tm, "%H-%M-%S");
    return ss.str();
  }
  /**
   * Log tags
   * @param level
   * @return
   */
  [[nodiscard]] std::string levelToString(LogLevel level) const {
    switch (level) {
    case LogLevel::Verbose:
      return "";
    case LogLevel::Info:
      return "[INFO]";
    case LogLevel::Status:
      return "[STATUS]";
    case LogLevel::Debug:
      return "[DEBUG]";
    case LogLevel::Warning:
      return "[WARNING]";
    case LogLevel::Error:
      return "[ERROR]";
    }
    throw std::exception();
  }

  std::chrono::nanoseconds startTimeMs;
  std::chrono::nanoseconds endTimeMs;

  LogLevel defaultLevel = LogLevel::Verbose;
  bool defaultPrintTime = true;

  [[nodiscard]] std::string indent(uint level) const {
    auto cnt = level * 2;
    return std::string(cnt, ' ');
  }

  template <typename T>
  void print(T&& value, uint indentLevel = 0) const {
    using namespace std::string_literals;
    if constexpr (is_iterable_v<std::decay_t<T>> && !std::is_same_v<std::string, std::decay_t<T>>) {
      print(indent(indentLevel));
      print("Container, size: "s + std::to_string(value.size()));
      print(" {\n");
      for (const auto &val : value) {
        print(indent(indentLevel + 1));
        print(val, indentLevel + 1);
        print(",\n");
      }
      print(indent(indentLevel));
      print("},\n");
    } else if constexpr (is_vec_specialisation_v<T>) {
      print(indent(indentLevel));
      print("glm::vec" + std::to_string(value.length()) + ":");
      print(" {");
      for (auto i = 0; i < value.length(); ++i) {
        print(value[i]);
        if (i < value.length() - 1) {
          print(", ");
        }
      }
      print("}");
    } else {
      std::cout << value;
    }
  }
public:
  template <LogLevel Level, bool PrintTime = false, bool PrintNewLine = true, typename... T> void log(T &&... message) const {
    if constexpr (!EnableDebug && Level == LogLevel::Debug) {
      return;
    }
    if constexpr (Level != LogLevel::Verbose) {
      if constexpr (PrintTime) {
        std::cout << levelToString(Level) + " " + getTime() + ": ";
      } else {
        std::cout << levelToString(Level) + ": ";
      }
    }
    (print(message), ...);
    if (PrintNewLine) {
      std::cout << std::endl;
    }
  }
  void startTime() {
    startTimeMs = now<std::chrono::nanoseconds>();
  }
  void endTime() {
    endTimeMs = now<std::chrono::nanoseconds>();
  }

  /**
   * Print time difference between endTime() and startTime() calls
   */
  void printElapsedTime() {
    auto tmp = endTimeMs - startTimeMs;
    log<LogLevel::Verbose>("Time elapsed: " + std::to_string(tmp.count()) + " ns");
  }

  void setDefaultLevel(LogLevel logLevel) {
    defaultLevel = logLevel;
  }

  void setDefaultPrintTime(bool printTime) {
    defaultPrintTime = printTime;
  }

private:
  template <LogLevel logLevel>
  struct OutOperator {
    Logger &logger;
    explicit OutOperator(Logger &logger) :logger(logger) {}

    template <typename T>
    auto operator<<(const T &rhs) {
      if constexpr (LoggerStreamModifiers::is_logger_flusher_v<T>) {
        std::cout.flush();
      } else if constexpr (logLevel == LogLevel::Verbose) {
        logger.log<logLevel, false, false>(rhs);
        return *this;
      } else {
        logger.log<logLevel, false, false>(rhs);
        return OutOperator<LogLevel::Verbose>(logger);
      }
    }
  };

public:

  template <typename T>
  auto operator<<(const T &rhs) {
    if constexpr(LoggerStreamModifiers::is_logger_stream_modifier_v<T>) {
      return OutOperator<LoggerStreamModifiers::modifierToLogLevel<T>()>(*this);
    } else if constexpr (LoggerStreamModifiers::is_logger_flusher_v<T>) {
      std::cout.flush();
      return *this;
    } else {
      log<LogLevel::Info, true, false>(rhs);
      return OutOperator<LogLevel::Verbose>(*this);
    }
  }


};

#endif // CSV_READER_LOGGER_H

#pragma clang diagnostic pop