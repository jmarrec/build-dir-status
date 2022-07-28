#include "TimeUtilities.hpp"

#include <fmt/format.h>

#include <chrono>
#include <exception>
#include <iomanip>
#include <sstream>

namespace build_dir_status {

std::time_t convert_str_to_time_t(const std::string& time_str) {
  std::tm t = {};
  std::istringstream ss(time_str);
  // ss.imbue(std::locale("en_US.utf-8"));
  ss >> std::get_time(&t, "%Y-%m-%dT%H:%M");
  if (ss.fail()) {
    throw std::runtime_error(fmt::format("Parse failed for: {}\n", time_str));
  }
  //std::cout << std::put_time(&t, "%c") << '\n';
  //
  std::time_t the_time = std::mktime(&t);
  if (the_time == -1) {
    throw std::runtime_error(fmt::format("Couldn't mktime {}\n", time_str));
  }
  return the_time;
}

std::string elapsed_time(const std::string& start_time_str, const std::string& end_time_str) {

  std::time_t start_time = convert_str_to_time_t(start_time_str);
  std::time_t end_time = end_time_str.empty() ? std::time(nullptr) : convert_str_to_time_t(end_time_str);

  // don't care about fractional seconds
  int elapsed_seconds = static_cast<int>(std::difftime(end_time, start_time));
  constexpr int seconds_in_day = 24 * 60 * 60;
  int nDays = elapsed_seconds / seconds_in_day;
  if (nDays > 1) {
    return fmt::format("{} days", nDays);
  } else if (nDays == 1) {
    return fmt::format("{} day", nDays);
  }

  int hHours = elapsed_seconds / 3600;
  int nMinutes = (elapsed_seconds % 3600) / 60;
  return fmt::format("{:02}:{:02}", hHours, nMinutes);
}

}  // namespace build_dir_status
