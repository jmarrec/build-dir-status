#include <gtest/gtest.h>

#include "../src/BuildDirectoryInfo.hpp"

#include <src/resources.hxx>

#include <algorithm>
#include <filesystem>
#include <string>
#include <string_view>

namespace fs = std::filesystem;
using namespace build_dir_status;

TEST(BuildDirInfoTest, parseBranchFile) {

  fs::path branchPath = testDirPath() / fs::path("OS-build/branch_test.txt");
  ASSERT_TRUE(fs::exists(branchPath));
  ASSERT_TRUE(fs::is_regular_file(branchPath));

  BuildDirectoryInfo info = BuildDirectoryInfo::parseBranchFile(branchPath);

  EXPECT_EQ(testDirPath() / "OS-build", info.directoryPath);
  EXPECT_EQ("OpenStudio", info.project);

  // Line 1: 4547_ATUSingleDuctInletSideMixer_DSOA.cde1466a33-Release
  EXPECT_EQ("4547_ATUSingleDuctInletSideMixer_DSOA", info.branch);
  EXPECT_EQ("cde1466a33", info.sha);
  EXPECT_EQ("Release", info.build_type);

  // Line 2: start=2022-07-28T15:45+02:00
  EXPECT_EQ("2022-07-28T15:45", info.start_time_str);

  // Line 3: end=2022-07-28T16:13+02:00
  EXPECT_EQ("2022-07-28T16:13", info.end_time_str);

  EXPECT_EQ("00:28", info.build_time);
  // This depends on the currrent time...
  EXPECT_NE("", info.build_elapsed_time);
}

// This test is specific to my machine
TEST(BuildDirInfoTest, DISABLED_findAll) {

  auto buildDirInfos = BuildDirectoryInfo::findAllBuildDirectoryInfos();
  EXPECT_GT(buildDirInfos.size(), 2);

  auto countForProject = [&buildDirInfos](std::string_view projectName) {
    return std::count_if(std::cbegin(buildDirInfos), std::cend(buildDirInfos),
                         [&projectName](const auto& buildDirInfo) { return buildDirInfo.project == projectName; });
  };
  EXPECT_GE(countForProject("OpenStudio"), 2);
  EXPECT_GE(countForProject("OpenStudioApplication"), 2);
  EXPECT_GE(countForProject("EnergyPlus"), 2);

  // for (const auto& buildDirInfo : buildDirInfos) {
  //   EXPECT_FALSE(true) << buildDirInfo;
  // }
}
