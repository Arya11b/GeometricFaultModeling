#ifndef DRAWABLE_H
#define DRAWABLE_H

#include <igl/opengl/glfw/Viewer.h>

class Drawable
{
	friend class Viewer;
public:
	Drawable();
	bool isRendered() const;
	void setVid(int vid);

	virtual void render(igl::opengl::glfw::Viewer* viewer) = 0;
protected:
	int id{};
	int vid{-1};
	static int idspec;
};


#endif // !DRAWABLE_H
