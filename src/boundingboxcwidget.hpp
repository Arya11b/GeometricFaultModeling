#ifndef BoundingBoxCWidget_H
#define BoundingBoxCWidget_H

#include "cwidget.hpp"
#include <memory>
#include "boundingbox.hpp"
#include "viewer.hpp"


class BoundingBoxCWidget: public CWidget
{
public:
    BoundingBoxCWidget();

    void initModel(std::shared_ptr<Viewer> v);
    Eigen::MatrixX3d getBounds();
    void setBounds(float x, float y, float z);
    double x();
    double y();
    double z();

    ~BoundingBoxCWidget() {}

private:

    // Model class
    BoundingBox bb;

    bool showBoundingBox{ true };
    bool isShowing{ false };

    
    // In case you found time:
    //double width_slider_value;
    //double height_slider_value;
    void initContent();


    void onDragging(int cursorId) {}
    void onClick(int cursorId) {}
    void onRelease(int cursorId) {}

};


#endif // !BoundingBoxCWidget_H
