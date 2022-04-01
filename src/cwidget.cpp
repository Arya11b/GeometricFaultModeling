#include "cwidget.hpp"
CWidget::CWidget(const char* name, int width, int height) {
	this->name = name;
	this->width = width;
	this->height = height;
}

void CWidget::init() {
	init(this->name);
}

void CWidget::init(const char* name) {
	this->name = name;
	ImGui::Begin(name);
	ImGui::SetNextWindowSize(ImVec2(width, height), ImGuiCond_FirstUseEver);
	initContent();
	callbacks();
	ImGui::End();
}

void CWidget::callbacks() {
	for (int i = 0; i < 3; i++) // 0 is for left-click, 1 for right-click, to for mid mouse button | could be possibly refined
	{
		if (ImGui::IsMouseClicked(i)) onClick(i);
		if (ImGui::IsMouseReleased(i)) onRelease(i);
		if (ImGui::IsMouseDragging(i)) onDragging(i);
	}
}
