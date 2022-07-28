#ifndef BUILDIRINFO_HPP
#define BUILDIRINFO_HPP

#include <filesystem>
#include <map>
#include <vector>

namespace build_dir_status {

namespace fs = std::filesystem;

struct BuildDirectoryInfo
{
  static BuildDirectoryInfo parseBranchFile(const fs::path& branchPath);
  //static std::map<std::string, std::vector<BuildDirectoryInfo>> findAllBuildDirectoryInfos();
  static std::vector<BuildDirectoryInfo> findAllBuildDirectoryInfos();

  fs::path directoryPath;
  std::string branch;
  std::string sha;
  std::string build_type;
  std::string build_time;
  std::string build_elapsed_time;
  std::string project;
  std::string start_time_str;
  std::string end_time_str;

  friend std::ostream& operator<<(std::ostream& os, BuildDirectoryInfo const& self);
};

}  // namespace build_dir_status

#endif  // BUILDIRINFO_HPP
