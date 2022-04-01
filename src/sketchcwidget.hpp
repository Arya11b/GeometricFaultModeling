#ifndef SketchCWidget_H
#define SketchCWidget_H

#include <CurveN.hpp>

#include "cwidget.hpp"
#include <memory>
#include "object.hpp"
#include "sketchplane.hpp"
#include "faultcwidget.hpp"
#include "boundingboxcwidget.hpp"

// TODO:
//      update sketch region on resize
// 
// Done:
//      slider for y, z
//      plane dimensions follow bounding box dimensions


class SketchCWidget : public CWidget {

public:
    SketchCWidget();

    void initModel(std::shared_ptr<Viewer> v, BoundingBoxCWidget* bbw, FaultCWidget* fw);

    Object& getObject();
private:
    FaultCWidget* fw;

    bool sketched_on_surface = false;

    Curve2D curve; //sketch/fault curve
    Curve2D oldCurve; // used for oversketching

    SketchPlane sp;

    BoundingBoxCWidget* bbw;

    ////sketch curve
    std::vector<Curve2D> scurvecol;
    //Eigen::MatrixX2d swcurve;




    ImVec2 sketchRegSize{};
    ImVec2 sketchRegBegPos{};
    ImVec2 sketchRegEndPos{};

    float x_slider_value{};
    float y_slider_value{};
    float z_slider_value{};


    bool showPoints{};
    bool overSketch{};



    void addCurveEndPoints(Curve2D& curve);
    void removeCurveEndPoints(Curve2D& curve, int n);

    void drawSketchPoints() const;
    void drawSketchLines(int r = 255, int g = 0, int b = 0, int alpha = 255) const;
    void drawSketchLines(const Curve2D& curve, int r = 255, int g = 0, int b = 0, int alpha = 255) const;
    void drawSketchLines(const Eigen::MatrixX2d& curve, int r = 128, int g = 0, int b = 0, int alpha = 255) const;
    
    void makeSketchRegion();

    //overrides
    void initContent() override;

    void onDragging(int cursorId) override;
    void onClick(int cursorId) override;
    void onRelease(int cursorId) override;

};

#endif // !SKETCHWINDOW_H
