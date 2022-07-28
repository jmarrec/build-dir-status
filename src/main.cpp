#include "BuildDirectoryInfo.hpp"

#include <array>
#include <memory>  // for shared_ptr, __shared_ptr_access
#include <string>  // for string, basic_string, operator+, to_string
#include <vector>  // for vector

#include "ftxui/component/captured_mouse.hpp"      // for ftxui
#include "ftxui/component/component.hpp"           // for Input, Renderer, Vertical
#include "ftxui/component/component_base.hpp"      // for ComponentBase
#include "ftxui/component/screen_interactive.hpp"  // for Component, ScreenInteractive
#include "ftxui/dom/elements.hpp"                  // for operator|, Element, size, border, frame, vscroll_indicator, HEIGHT, LESS_THAN

#include <fmt/format.h>

int main() {
  using namespace ftxui;
  using namespace build_dir_status;

  std::vector<std::string> tabNames;

  using DirInfoAndState = std::pair<BuildDirectoryInfo, bool>;

  auto buildDirInfos = BuildDirectoryInfo::findAllBuildDirectoryInfos();
  std::map<std::string, std::vector<DirInfoAndState>> buildDirInfoMap;
  for (const auto& info : buildDirInfos) {
    if (buildDirInfoMap.find(info.project) == buildDirInfoMap.end()) {
      buildDirInfoMap.insert(std::make_pair(info.project, std::vector<DirInfoAndState>{std::make_pair(info, false)}));

    } else {
      buildDirInfoMap.at(info.project).push_back(std::make_pair(info, false));
    }
  }

  int tab_selected = 0;
  auto tab_toggle = Toggle(&tabNames, &tab_selected);

  auto tab_container = Container::Tab({}, &tab_selected);

  for (auto& [tabName, infos] : buildDirInfoMap) {

    auto container = Container::Vertical({});
    for (auto& info : infos) {
      fmt::print("{} - {}\n", tabName, info.first.directoryPath.filename().string());
      container->Add(Checkbox(info.first.directoryPath.filename().string(), &(info.second)));
    }

    tab_container->Add(container);
    tabNames.push_back(tabName);
  }

  auto container = Container::Vertical({
    tab_toggle,
    tab_container,
  });

  auto renderer = Renderer(container, [&] {
    return vbox({
             tab_toggle->Render(),
             separator(),
             tab_container->Render(),
           })
           | border;
  });

  auto screen = ScreenInteractive::TerminalOutput();
  screen.Loop(renderer);
}

/*
  int value = 50;

  // The tree of components. This defines how to navigate using the keyboard.
  auto buttons = Container::Horizontal({
    Button("Decrease", [&] { value--; }),
    Button("Increase", [&] { value++; }),
  });

  std::array<bool, 30> states;

  auto container = Container::Vertical({});
  for (int i = 0; i < 30; ++i) {
    states[i] = false;
    container->Add(Checkbox("Checkbox" + std::to_string(i), &states[i]));
  }

  auto renderer = Renderer(container, [&] {
      auto container->Render() | vscroll_indicator | frame | size(HEIGHT, LESS_THAN, 10) | border; });

  auto screen = ScreenInteractive::FitComponent();
  screen.Loop(renderer);

  return 0;
}
*/
