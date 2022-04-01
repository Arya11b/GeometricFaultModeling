#ifndef IMVEC2MISC_H
#define IMVEC2MISC_H

#include<imgui.h>

ImVec2 operator-(const ImVec2& v1, const ImVec2& v2) {
    return ImVec2(v1.x - v2.x, v1.y - v2.y);
}

ImVec2 operator+(const ImVec2& v1, const ImVec2& v2) {
    return ImVec2(v1.x + v2.x, v1.y + v2.y);
}

ImVec2 operator/(const ImVec2& vec, const int& scalar) {
    return ImVec2(vec.x / double(scalar), vec.y / double(scalar));
}


ImVec2 operator*(const ImVec2& vec, const int& scalar) {
    return ImVec2(vec.x * double(scalar), vec.y * double(scalar));
}
#endif // !IMVEC2MISC