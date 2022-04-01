#include "sketchcwidget.hpp"

#include <imgui.h>
#include "functions/calculate_intersection_curves.hpp"
#include "functions/separate_object_by_interpolant.hpp"
#include "geo/marching_squares.hpp"
#include "defs/imvec2misc.hpp"
#include "fault.hpp"

SketchCWidget::SketchCWidget() : CWidget("Sketch Window", 350, 400) {}

void SketchCWidget::initModel(std::shared_ptr<Viewer> v, BoundingBoxCWidget *bbw, FaultCWidget *fw)
{
    this->v = v;
    this->bbw = bbw;
    this->fw = fw;

    sp.init(bbw->x(), bbw->y());
}

Object &SketchCWidget::getObject()
{
    return sp.object;
}

void SketchCWidget::initContent()
{

    ImGui::GetIO().ConfigWindowsMoveFromTitleBarOnly = true;
    ImVec2 SketchCWidgetSize = ImVec2(350, 400);

    // init stuff
    ImVec2 margin = ImVec2(50, 50);

    sketchRegBegPos = ImGui::GetWindowPos() + margin;
    sketchRegSize = ImVec2(300, 300);
    sketchRegEndPos = sketchRegBegPos + sketchRegSize;

    ImGui::BeginChild("Sketching windows", SketchCWidgetSize, true);

    makeSketchRegion();

    ImGui::EndChild();

    ImGui::BeginChild("control pad");

    if (ImGui::CollapsingHeader("Sketch-based Operations", ImGuiTreeNodeFlags_DefaultOpen))
    {

        ImGui::Checkbox("oversketching", &overSketch);
    }

    if (ImGui::CollapsingHeader("plane position", ImGuiTreeNodeFlags_DefaultOpen))
    {
        double x_range{bbw->x() / 2};
        double y_range{bbw->y() / 2};
        double z_range{bbw->z() / 2};
        if (ImGui::Button("X plane"))
        {
            sp.makeX(bbw->y(), bbw->z());
        }
        if (ImGui::Button("Y plane"))
        {
            sp.makeY(bbw->x(), bbw->z());
        }
        if (ImGui::Button("Z plane"))
        {
            sp.makeZ(bbw->x(), bbw->y());
        }
        if (ImGui::SliderFloat("X", &x_slider_value, -x_range, x_range))
        {
            v->clear(sp.object);
            Eigen::RowVector3d pos = sp.object.position();
            pos(0, 0) = x_slider_value;
            sp.object.setPosition(pos);
            v->render(sp.object);
        }
        if (ImGui::SliderFloat("Y", &y_slider_value, -y_range, y_range))
        {
            v->clear(sp.object);
            Eigen::RowVector3d pos = sp.object.position();
            pos(0, 1) = y_slider_value;
            sp.object.setPosition(pos);
            v->render(sp.object);
        }
        if (ImGui::SliderFloat("Z", &z_slider_value, -z_range, z_range))
        {
            
            v->clear(sp.object);
            Eigen::RowVector3d pos = sp.object.position();
            pos(0, 2) = z_slider_value;
            sp.object.setPosition(pos);

            //march squares
            scurvecol.clear();
            for(int i=0; i<fw->size();i++){
                std::vector<Curve2D> scurve;
                functions::calculate_curve_intersections(fw->at(i).getGeometry(),sp.object.get(),scurve);
                            auto sppos = sp.dl() + sp.object.position();
            Point2D basep(sppos(0) , sppos(1));

            for (auto& c:scurve){
                for (size_t i = 0; i < c.size(); i++)
                {
                    c[i] -= basep;
                    c[i][1] /= sp.height;
                    c[i][0] /= sp.width;
                    c[i][1] = 1 - c[i][1];
                    //c[i] /= c[i].norm();
                }
                scurvecol.emplace_back(c);
                
            }
            }


            //v->render(sp.object);
        }
        if(ImGui::Button("sketch on surface")){
            if(fw->index() >= 0) {
                auto obj = fw->atSelection().getGeometry();
                v->setRaycastObject(obj);
                v->setRaycasterCallback();
            }
        }
        if(v->releasedMouse()){
                auto sketchPoints = v->getSketchPoints();
                Fault f;
                FCurve fc;
                v->resetCallbacks();
                fc.init(sketchPoints);
                fc.standardFilter();
                fc.calculateNormals(fw->atSelection().getImplicit());
                f.addUFCurve(fc);
                f.setBounds(bbw->getBounds());
                f.interpolate();
                fw->add(std::move(f));
                for (int i = 0; i < fw->size(); i++)
                {
                    v->render(fw->at(i));
                }
                //v->render(f);
        }
    }
    if (ImGui::CollapsingHeader("Specs", ImGuiTreeNodeFlags_DefaultOpen))
    {
        ImGui::Checkbox("Show Points", &showPoints);
        //ImGui::Checkbox("Show Points In the Viewer", &showPointsViewer);
    }

    ImGui::EndChild();

    for(const auto& sc: scurvecol) {
        drawSketchLines(sc,255,0,0);
    }
    //if (faults.size() != 0) {
    //drawSketchLines(0, 0, 255);
    //}
    if (showPoints)
    {
        drawSketchPoints();
    }
}

// adds curve points to the beginning and end point of the curve so that they completely divide the region in the plane
void SketchCWidget::addCurveEndPoints(Curve2D &curve)
{
    if (curve.size() < 2)
        return;

    auto v = curve.atEnd() - curve.at(curve.size() - 2);

    double y_len1 = abs(curve.atEnd().y() - curve.at(curve.size() - 2).y());
    double y_len2 = 1 - curve.atEnd().y();

    double x_len1 = abs(curve.atEnd().x() - curve.at(curve.size() - 2).x());
    double x_len2 = 1 - curve.atEnd().x();

    double dirX = (curve.at(curve.size() - 2) - curve.at(curve.size() - 2)).x();
    double dirY = (curve.at(curve.size() - 2) - curve.at(curve.size() - 2)).y();
}

void SketchCWidget::removeCurveEndPoints(Curve2D &curve, int n)
{
    if (curve.size() < 2 * n)
        return;
    Curve2D newCurve;
    for (int i = n; i < curve.size() - n; i++)
    {
        newCurve.add(curve.at(i));
    }
    curve = newCurve;
}

// makes sense

void SketchCWidget::drawSketchPoints() const
{
    for (int i = 0; i < curve.size(); i++)
    {
        ImVec2 pwin1;
        pwin1.x = curve[i].x() * sketchRegSize.x + sketchRegBegPos.x;
        pwin1.y = curve[i].y() * sketchRegSize.y + sketchRegBegPos.y;
        ImGui::GetWindowDrawList()->AddCircleFilled(pwin1, 6, IM_COL32(255, 255, 255, 255), 8);
    }
}

void SketchCWidget::drawSketchLines(int r, int g, int b, int alpha) const
{
    for (int i = 1; i < curve.size(); i++)
    {
        ImVec2 pwin1, pwin2;
        pwin1.x = curve[i - 1].x() * sketchRegSize.x + sketchRegBegPos.x;
        pwin1.y = curve[i - 1].y() * sketchRegSize.y + sketchRegBegPos.y;
        pwin2.x = curve[i].x() * sketchRegSize.x + sketchRegBegPos.x;
        pwin2.y = curve[i].y() * sketchRegSize.y + sketchRegBegPos.y;
        ImGui::GetWindowDrawList()->AddLine(pwin1, pwin2, IM_COL32(r, g, b, alpha), 3.0f);
    }
}


//refractor later
void SketchCWidget::drawSketchLines(const Eigen::MatrixX2d &curve, int r, int g, int b, int alpha) const
{
    for (int i = 1; i < curve.rows(); i++)
    {
        ImVec2 pwin1, pwin2;
        pwin1.x = curve(i-1,0) * sketchRegSize.x + sketchRegBegPos.x;
        pwin1.y = curve(i-1,1) * sketchRegSize.y + sketchRegBegPos.y;
        pwin2.x = curve(i,0) * sketchRegSize.x + sketchRegBegPos.x;
        pwin2.y = curve(i,1) * sketchRegSize.y + sketchRegBegPos.y;
        ImGui::GetWindowDrawList()->AddLine(pwin1, pwin2, IM_COL32(r, g, b, alpha), 3.0f);
    }
}

void SketchCWidget::drawSketchLines(const Curve2D &curve, int r, int g, int b, int alpha) const
{
    for (int i = 1; i < curve.size(); i++)
    {
        ImVec2 pwin1, pwin2;
        pwin1.x = curve[i - 1].x() * sketchRegSize.x + sketchRegBegPos.x;
        pwin1.y = curve[i - 1].y() * sketchRegSize.y + sketchRegBegPos.y;
        pwin2.x = curve[i].x() * sketchRegSize.x + sketchRegBegPos.x;
        pwin2.y = curve[i].y() * sketchRegSize.y + sketchRegBegPos.y;
        ImGui::GetWindowDrawList()->AddLine(pwin1, pwin2, IM_COL32(r, g, b, alpha), 3.0f);
    }
}

void SketchCWidget::makeSketchRegion()
{
    ImGui::GetWindowDrawList()->AddRect(sketchRegBegPos, sketchRegEndPos, IM_COL32(255, 255, 255, 255), 5.0f);
}

void SketchCWidget::onDragging(int cursorId)
{
    if (cursorId == 0 && ImGui::IsMouseHoveringRect(sketchRegBegPos, sketchRegEndPos))
    {
        ImVec2 cursorPos = ImGui::GetIO().MousePos - sketchRegBegPos;
        Point2D norm(cursorPos.x / sketchRegSize.x, cursorPos.y / sketchRegSize.y);

        curve.add(norm);

        if (!oldCurve.isEmpty())
            drawSketchLines(oldCurve, 0, 0, 255);
        drawSketchLines();
        if (showPoints)
        {
            drawSketchPoints();
        }
    }
}

void SketchCWidget::onClick(int cursorId)
{
    if (cursorId == 0 && ImGui::IsMouseHoveringRect(sketchRegBegPos, sketchRegEndPos))
    {
        if (overSketch)
        {
            oldCurve = curve;
            drawSketchLines(oldCurve, 0, 0, 255);
        }
        curve.clear();
    }
}

void SketchCWidget::onRelease(int cursorId)
{
    // if released mouse inside sketch window
    if (cursorId == 0 && ImGui::IsMouseHoveringRect(sketchRegBegPos - ImVec2(20, 20), sketchRegEndPos + ImVec2(20, 20)) && curve.size() > 1)
    {
        if (overSketch)
        {
            if (!curve.isEmpty())
            {
                Curve2D rest;
                //addCurveEndPoints(xy);
                curve = curve.overSketch(oldCurve, rest, 0.1, 32.f);
                oldCurve.clear();
            }
        }

        // filtering
        // [TODO] move to fcurve class
        //addCurveEndPoints(curve);
        curve.lineFilter(0.002f, 4);
        curve.meanFilter();
        curve.meanFilter();
        sp.addCurveToPlane(curve);

        //removeCurveEndPoints(curve, 1);
        if (fw->index() < 0)
        {

            Fault f;
            f.addUFCurve(sp.fcurve);
            //f.addBoundaryCurvesLike(sp.fcurve);
            f.setBounds(bbw->getBounds());
            f.interpolate();

            fw->add(std::move(f));

            for (int i = 0; i < fw->size(); i++)
            {
                v->render(fw->at(i));
            }
        }
        else
        {
            auto &f{fw->atSelection()};
            f.addUFCurve(sp.fcurve);
            f.interpolate();
            v->render(f);
        }
    }
}
