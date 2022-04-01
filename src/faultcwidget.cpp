#include "faultcwidget.hpp"

#include <imgui.h>

FaultCWidget::FaultCWidget() : CWidget("Fault Object Window", 650, 600) {}

FaultCWidget::~FaultCWidget() {}

void FaultCWidget::initModel(std::shared_ptr<Viewer> v) { this->v = v; }

void FaultCWidget::initContent() {
  if (ImGui::ListBox("Fault Objects", &select_index, names)) {
    std::cout << select_index;
  }
  if (ImGui::Button("unselect")) {
    select_index = -1;
  }
  ImGui::SameLine();
  if (select_index > -1) {
    auto& fs = fault.ent.at(select_index);
    if (!fs.hidden) {
      if (ImGui::Button("hide")) {
        v->clear(fs.f);
        fs.hidden = true;
      }
    } else {
      if (ImGui::Button("show")) {
        v->render(fs.f);
        fs.hidden = false;
      }
    }
  }
}

void FaultCWidget::onDragging(int cursorId) {}

void FaultCWidget::onClick(int cursorId) {}

void FaultCWidget::onRelease(int cursorId) {}

std::vector<std::string> FaultCWidget::getNames() {
  std::vector<std::string> names;
  for (int i = 0; i < fault.ent.size(); i++) {
    names.emplace_back(fault.ent.at(i).name);
  }
  return names;
}

int FaultCWidget::index() { return select_index; }

// adds Fault f to the database
void FaultCWidget::add(Fault&& f) {
  FaultAttribute fa;
  fa.f = f;
  fa.name =
      "Fault " + ((fa.id < 10) ? "00" + std::to_string(fa.id)
                               : ((fa.id < 100) ? "0" + std::to_string(fa.id)
                                                : std::to_string(fa.id)));
  // std::cout << "behold: " << std::endl;
  // std::cout << fa.name << std::endl;
  fault.ent.insert(fa);
  names = getNames();
}

void FaultCWidget::remove(int index) {
  assert(index < fault.ent.size());
  std::cout << "not implemented\n";
  // fault.ent.erase(fault.ent.begin() + index);
}
Fault& FaultCWidget::at(int index) {
  assert(index < fault.ent.size());
  return fault.ent.at(index).f;
}
Fault& FaultCWidget::atSelection() {
  assert(select_index >= 0);
  return fault.ent.at(select_index).f;
  // TODO: insert return statement here
}
int FaultCWidget::size() { return fault.ent.size(); }
