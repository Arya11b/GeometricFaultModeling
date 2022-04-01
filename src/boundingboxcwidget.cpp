#include "boundingboxcwidget.hpp"


BoundingBoxCWidget::BoundingBoxCWidget() :CWidget("Bounding Box Window", 300, 300) {}

void BoundingBoxCWidget::initModel(std::shared_ptr<Viewer> v) {
    this->v = v;
    bb.init(8.f, 8.f, 8.f);
}
void BoundingBoxCWidget::setBounds(float x, float y, float z){
    bb.init(x,y,z);
}

Eigen::MatrixX3d BoundingBoxCWidget::getBounds() {
    return bb.object.getVertices();
}

double BoundingBoxCWidget::x() {
    return bb.x_dim;
}

double BoundingBoxCWidget::y() {
    return bb.y_dim;
}

double BoundingBoxCWidget::z() {
    return bb.z_dim;
}

// In case you found time:
//double width_slider_value;
//double height_slider_value;

void BoundingBoxCWidget::initContent() {
    ImGui::GetIO().ConfigWindowsMoveFromTitleBarOnly = true;

    ImGui::BeginChild("control pad");

    if (ImGui::CollapsingHeader("Specs", ImGuiTreeNodeFlags_DefaultOpen))
    {
        ImGui::Checkbox("Show Bounding Box", &showBoundingBox);
        //ImGui::Checkbox("Show Points In the Viewer", &showPointsViewer);
    }

    ImGui::EndChild();

    // room for refactoring
    if (showBoundingBox) {
        if (!isShowing) {
            v->render(bb);
            isShowing = true;
        }
    }
    else {
        isShowing = false;
        v->clear(bb.object);
    }

}
