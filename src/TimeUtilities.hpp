#ifndef TIMEUTILITIES_HPP
#define TIMEUTILITIES_HPP

#include <chrono>
#include <string>

namespace build_dir_status {

std::time_t convert_str_to_time_t(const std::string& time_str);

std::string elapsed_time(const std::string& start_time_str, const std::string& end_time_str);

}  // namespace build_dir_status

#endif  // TIMEUTILITIES_HPP
