#include "dbg/dbg.hpp"

struct Arbitrary {
    int num1;
    int num2;
};

struct Vector3D {
    float x, y, z;
};

dbg::FmtStream vec3Dconverter(const Vector3D& v) {
    dbg::FmtStream fs;
    fs << dbg::Style::Red << "Vector3D" << dbg::Style::Cyan
        << " { .x = " << v.x << ", .y = " << v.y << ", .z = " << v.z << " }"
        << dbg::Style::Reset;
    return fs;
}

int main() {

    dbg::globalDebugHandle->registerType<Vector3D>(vec3Dconverter);

    dbg::logf<"Hello World!\n">();
    dbg::logf<"Arbitrary data {}\n">(Arbitrary());
    dbg::logf<"Not 'arbitrary' data {}\n">(Vector3D());
    dbg::logf<"Fancy some braces? {{}} {{{{ }} }} }} }} {{ {{ {{{{{{}}{}{}}}}}\n">(':', 'P');
    dbg::logf<"int = {}, float = {}, string = {}\n">(100, 3.1415f, "What?");

    return 0;
}