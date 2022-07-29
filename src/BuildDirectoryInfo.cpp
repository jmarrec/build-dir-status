#include "BuildDirectoryInfo.hpp"
#include "TimeUtilities.hpp"

#include <ctre.hpp>
#include <fmt/format.h>

#include <array>
#include <fstream>
#include <ostream>
#include <string>

namespace build_dir_status {

std::ostream& operator<<(std::ostream& os, BuildDirectoryInfo const& self) {
  return os << self.project << '\n'
            << "directoryPath=" << self.directoryPath << '\n'
            << "branch=" << self.branch << ", sha=" << self.sha << ", build_type=" << self.build_type << '\n'
            << "build_elapsed_time=" << self.build_elapsed_time << '\n'
            << "build_time=" << self.build_time << '\n';
}

BuildDirectoryInfo BuildDirectoryInfo::parseBranchFile(const fs::path& branchPath) {
  BuildDirectoryInfo info;
  info.directoryPath = branchPath.parent_path();

  info.project = info.directoryPath.filename().string();
  if (info.project.find("App") != std::string::npos) {
    info.project = "OpenStudioApplication";
  } else if (info.project.find("EnergyPlus") != std::string::npos) {
    info.project = "EnergyPlus";
  } else if (info.project.find("OS") != std::string::npos) {
    info.project = "OpenStudio";
  }

  std::ifstream input(branchPath);
  std::string line;

  // First line
  std::getline(input, line);
  if (auto [whole, branch, sha, build_type] =
        ctre::match<"(?<branch>.*?)\\.(?<sha>\\w{5,15})-(?<build_type>Release|Debug|RelWithDebInfo|MinSizeRel)">(line);
      whole) {
    info.build_type = build_type;
    info.branch = branch;
    info.sha = sha;
  }

  // Second: start
  std::getline(input, line);
  if (line.starts_with("start=")) {

    info.start_time_str = line.substr(6, line.size() - 6 - 6);
    info.build_elapsed_time = elapsed_time(info.start_time_str, "");
    // auto start_pos = line.find("=");
    // auto end_pos = line.rfind("+");
    // info.start_time_str = line.substr(start_pos + 1, end_pos - start_pos - 1);
  }

  // Third: end, only if build finished
  std::getline(input, line);
  info.build_time = "NaN";
  if (!line.empty()) {
    if (line.starts_with("end=")) {
      info.end_time_str = line.substr(4, line.size() - 6 - 4);
      info.build_time = elapsed_time(info.start_time_str, info.end_time_str);
    }
  }

  //for (std::string line; getline(input, line);) {
  //  fmt::print("{}\n", line);
  //}

  return info;
}

// std::map<std::string, std::vector<BuildDirectoryInfo>> findAllBuildDirectoryInfos() {

std::vector<BuildDirectoryInfo> BuildDirectoryInfo::findAllBuildDirectoryInfos() {
  std::array<fs::path, 5> potentialRootDirs{
    fs::path("/home/julien/Software/Others/"),
    fs::path("/media/DataExt4/Software/Others/"),
    fs::path("/Users/julien/Software/Others/"),
    fs::path("C:/src/"),
    // Cur Dir
    fs::absolute(fs::path(".")),
  };

  std::vector<BuildDirectoryInfo> result;

  for (const auto& rootDir : potentialRootDirs) {
    if (fs::is_directory(rootDir)) {
      for (const auto& entry : fs::directory_iterator(rootDir)) {
        if (entry.is_directory()) {
          fs::path branchPath = entry.path() / ".branch";
          if (fs::is_regular_file(branchPath)) {
            result.push_back(BuildDirectoryInfo::parseBranchFile(branchPath));
          }
        }
      }
    }
  }

  return result;
}

}  // namespace build_dir_status
