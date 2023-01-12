#ifndef __COLOR_H__
#define __COLOR_H__

struct Color {
    constexpr Color() = default;
    constexpr Color(float r, float g, float b, float a) : r(r), g(g), b(b), a(a) {}

    float r = 1.0f;
    float g = 1.0f;
    float b = 1.0f;
    float a = 1.0f;
};

#endif // __COLOR_H__