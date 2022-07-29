#include <gtest/gtest.h>

#include "../src/TimeUtilities.hpp"  // for elapsed_time, convert_str_to_time_t

#include <string>  // for string, allocator

using namespace build_dir_status;

TEST(TimeUtilitiesTest, convert_str_to_time_t) {

  std::string time_str = "2022-07-28T15:45";
  EXPECT_NO_THROW(convert_str_to_time_t(time_str));

  time_str = "blablabla";
  EXPECT_ANY_THROW(convert_str_to_time_t(time_str));
}

TEST(TimeUtilitiesTest, elapsed_time) {

  std::string start_time_str = "2022-07-28T15:45";
  std::string end_time_str = "2022-07-28T16:47";

  std::string elapsed = elapsed_time(start_time_str, end_time_str);
  EXPECT_EQ("01:02", elapsed_time(start_time_str, end_time_str));

  end_time_str = "2022-07-29T14:45";
  EXPECT_EQ("23:00", elapsed_time(start_time_str, end_time_str));

  end_time_str = "2022-07-29T16:45";
  EXPECT_EQ("1 day", elapsed_time(start_time_str, end_time_str));

  end_time_str = "2022-07-30T14:45";
  EXPECT_EQ("1 day", elapsed_time(start_time_str, end_time_str));

  end_time_str = "2022-07-30T16:45";
  EXPECT_EQ("2 days", elapsed_time(start_time_str, end_time_str));

  end_time_str = "2022-07-31T16:45";
  EXPECT_EQ("3 days", elapsed_time(start_time_str, end_time_str));

  end_time_str = "2022-08-01T16:45";
  EXPECT_EQ("4 days", elapsed_time(start_time_str, end_time_str));
}
