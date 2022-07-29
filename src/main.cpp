#include "BuildDirectoryInfo.hpp"

#include <array>
#include <map>
#include <memory>   // for shared_ptr, __shared_ptr_access
#include <string>   // for string, basic_string, operator+, to_string
#include <sstream>  // for stringstream
#include <vector>   // for vector

#include <ftxui/component/captured_mouse.hpp>      // for ftxui
#include <ftxui/component/component.hpp>           // for Input, Renderer, Vertical
#include <ftxui/component/component_base.hpp>      // for ComponentBase
#include <ftxui/component/component_options.hpp>   //
#include <ftxui/component/screen_interactive.hpp>  // for Component, ScreenInteractive
#include <ftxui/dom/elements.hpp>                  // for operator|, Element, size, border, frame, vscroll_indicator, HEIGHT, LESS_THAN
#include <ftxui/dom/table.hpp>                     // for Table, TableSelection
#include <ftxui/screen/screen.hpp>                 // for Screen
#include <ftxui/screen/color.hpp>
#include <ftxui/dom/node.hpp>

#include <fmt/format.h>

using DirInfoAndState = std::pair<build_dir_status::BuildDirectoryInfo, bool>;
using BuilDirInfoMap = std::map<std::string, std::vector<DirInfoAndState>>;

ftxui::Element BuildDirInfo(const build_dir_status::BuildDirectoryInfo& info) {
  std::stringstream ss;
  ss << info;
  std::vector<ftxui::Element> rows;
  for (std::string line; std::getline(ss, line);) {
    rows.push_back(ftxui::text(line));
  }

  return ftxui::vbox(rows);
}

ftxui::Element highLevelInfo(const BuilDirInfoMap& buildDirInfoMap) {
  int nTot = 0;
  std::map<std::string, int> numByProject;

  // cppcheck-suppress unassignedVariable
  for (const auto& [tabName, infos] : buildDirInfoMap) {
    numByProject[tabName] = 0;
    for (const auto& info : infos) {
      if (info.second) {
        ++nTot;
        ++numByProject[tabName];
      }
    }
  }
  if (nTot == 0) {
    return ftxui::text("Zero build selected");
  }

  std::vector<ftxui::Element> rows;
  rows.push_back(ftxui::text(fmt::format("There are {} builds selected", nTot)));
  for (const auto& [tabName, n] : numByProject) {
    if (n > 0) {
      // rows.push_back(ftxui::separator());
      rows.push_back(ftxui::text(fmt::format("\n * {} {}", n, tabName)));
    }
  }
  return ftxui::vbox(rows);
}

int main() {

  using namespace build_dir_status;

  auto screen = ftxui::ScreenInteractive::TerminalOutput();

  std::vector<std::string> tabNames;

  auto buildDirInfos = BuildDirectoryInfo::findAllBuildDirectoryInfos();
  BuilDirInfoMap buildDirInfoMap;
  for (const auto& info : buildDirInfos) {
    if (buildDirInfoMap.find(info.project) == buildDirInfoMap.end()) {
      buildDirInfoMap.insert(std::make_pair(info.project, std::vector<DirInfoAndState>{std::make_pair(info, false)}));

    } else {
      buildDirInfoMap.at(info.project).push_back(std::make_pair(info, false));
    }
  }

  auto build_info = ftxui::text("");

  std::string quit_text = "Quit";

  int tab_selected = 0;
  auto tab_toggle = ftxui::Toggle(&tabNames, &tab_selected);

  auto tab_container = ftxui::Container::Tab({}, &tab_selected);

  for (auto& [tabName, infos] : buildDirInfoMap) {

    // std::vector<std::vector<ftxui::Element>> tableRows;

    auto container = ftxui::Container::Vertical({});
    for (auto& info : infos) {

      // std::vector<ftxui::Element> tableRow;

      auto option = ftxui::CheckboxOption();
      option.on_change = [&build_info, &info]() { build_info = BuildDirInfo(info.first); };
      auto checkbox = ftxui::Checkbox(info.first.directoryPath.filename().string(), &(info.second), option);
      // checkbox->OnEvent(ftxui::Event(   FOCUS_EVENT
      // tableRow.push_back(checkbox->Render());
      // tableRow.push_back(ftxui::text(info.first.directoryPath.filename().string()));
      container->Add(checkbox);
    }
    tab_container->Add(container);

    // for (auto& info : infos) {
    //   tableContent.push_back
    // }

    tabNames.push_back(tabName);
  }

  auto quit_button = ftxui::Button(&quit_text, screen.ExitLoopClosure(), ftxui::ButtonOption::Ascii());

  auto firstrow = ftxui::Container::Horizontal({tab_toggle, quit_button});

  auto container = ftxui::Container::Vertical({
    firstrow,
    tab_container,
  });

  auto renderer = ftxui::Renderer(container, [&] {
    return ftxui::vbox({
             ftxui::hbox({
               tab_toggle->Render(),
               ftxui::filler(),
               quit_button->Render() | ftxui::size(ftxui::WIDTH, ftxui::GREATER_THAN, 20),
             }),
             ftxui::separator(),
             tab_container->Render(),
             ftxui::separator(),
             build_info,
             ftxui::separator(),
             highLevelInfo(buildDirInfoMap),
           })
           | ftxui::border;
  });

  screen.Loop(renderer);

  // On exit, we print the needed command to stdout
  bool firstTime = true;
  for (const auto& [tabName, infos] : buildDirInfoMap) {
    for (const auto& info : infos) {
      if (info.second) {
        if (!firstTime) {
          fmt::print(" \\\n");
        }
        fmt::print("cd {} && N=14 ./ninja.sh None;", info.first.directoryPath.string());
        firstTime = false;
      }
    }
  }
  fmt::print("\n");
}
