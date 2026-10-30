# dbg
A debug logger in C++ 20

## Features
- Single Header only and needs a minimum of C++ 20 to work.
- Compile time string formatting and validation.
- Print any arbitrary object with proper fallback converters.
- Add and register custom converters for arbitrary objects at runtime and see them spring into action!
- Output to multiple sinks and control the printing for each sink granularly!
- Add and register sinks at runtime.
- Supports printing ANSI colour codes to the sinks, as per the sink's configuration.
- Sinks are designed to be stateless.
- Custom primtive log message formatting options.
- Does demangling of names for unknown arbitrary objects, if the platform allows, otherwise just prints the mangled names.

## Quick Start

The quickest way to get started with the project is:
1. Dowload the code or `git clone https://github.com/Ar6021/dbg.git --depth 1`

2. `cd` into the directory and run `python amalgamate.py`

3. The output by default will be produced at `dist/dbg.hpp`, copy it to the desired folder and use it!

## Example Usage

**Simple Example:**
```c++
#include "dist/dbg.hpp"

int main() {
    dbg::logf<"Hello World!\n">();
    return 0;
}

```

Output:
```
[2026-10-29 20:03:18.238] @ int main() >> src/main.cpp:4 => Hello World!
```

**Elaborate Example:** (as given in `src/main.cpp`)
```c++
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
```

Output:
![elaborate_output](https://github.com/user-attachments/assets/662cf7fb-2953-4f76-97f0-10ed1fffc95b)

**Give it a try!**