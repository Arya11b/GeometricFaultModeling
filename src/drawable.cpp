#include "drawable.hpp"

Drawable::Drawable() {
	id = idspec++;
}
int Drawable::idspec{};

bool Drawable::isRendered() const{
	return vid > -1;
}

void Drawable::setVid(int vid){
	this->vid = vid;
}