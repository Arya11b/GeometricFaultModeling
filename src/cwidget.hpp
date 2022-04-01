#ifndef WINDOW_H
#define WINDOW_H

#include<imgui.h>
#include<memory>
#include "viewer.hpp"

// controller-widget class
class CWidget {
public:
	CWidget() = delete;
	CWidget(const char* name, int width, int height);
	void init();
	void init(const char* name);
protected:
	std::shared_ptr<Viewer> v;
	
	virtual void callbacks();
	virtual void initContent() = 0;
	virtual void onClick(int cursorId) = 0;
	virtual void onRelease(int cursorId) = 0;
	virtual void onDragging(int cursorId) = 0;
	const char* name{};
	int width{};
	int height{};

};


#endif 