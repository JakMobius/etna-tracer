#pragma once

/**
 * An extremely fast implementation of three-dimensional
 * vector.
 *
 * Authored:                by KingCakeTheFruity
 * Vectorization:           by AlexRoar
 * Debug and inlining help: by JakMobius
 */

#include <cmath>
#include <iostream>
#include "matrix4.hpp"

extern const float FLOAT_EPS;
extern const float DOUBLE_EPS;

template<typename T>
struct Vec3 {
    typedef T content3 __attribute__((ext_vector_type(4)));

    content3 content;

    Vec3<T>(): content {0, 0, 0} {};

    Vec3<T>(T x, T y, T z): content {x, y, z} {};

    explicit Vec3<T>(content3 newContent): content (newContent) {}


    inline T len() const { return sqrt(len_squared()); }

    inline T len_squared() const {
        const auto squared = content * content;
        return squared[0] + squared[1] + squared[2];
    }

    inline Vec3<T> normal() const {
        T l = len();
        if(l < FLOAT_EPS) return {0, 0, 0};
        else return Vec3<T> {content / l};
    }

    inline void normalize() {
        T l = len();
        if(l < FLOAT_EPS) content = {0, 0, 0};
        else content /= l;
    }

    [[nodiscard]] inline bool is_zero() const {
        const auto squared = (content * content) < FLOAT_EPS;
        return squared[0] * squared[1] * squared[2];
    }

    inline T dot(const Vec3<T> &other) const {
        const auto res = (content * other.content);
        return res[0] + res[1] + res[2];
    }

    inline Vec3<T> cross(const Vec3<T> &other) const {
        const content3 left_first = {content[1], content[2], content[0]};
        const content3 right_first = {other.content[2], other.content[0], other.content[1]};
        const content3 left_second = {content[2], content[0], content[1]};
        const content3 right_second = {other.content[1], other.content[2], other.content[0]};
        const auto res = left_first * right_first - right_second * left_second;
        return Vec3<T> {res};
    }

    void set_x(T x) { content[0] = x; }
    void set_y(T y) { content[1] = y; }
    void set_z(T z) { content[2] = z; }
    void set(int i, T x) { content[i] = x; }

    T operator[](const int i) const { return content[i]; }

    inline Vec3<T> operator+() { return *this; }

    inline Vec3<T> operator-() const { return {content * -1}; }

    inline Vec3<T> operator+(const Vec3<T> &second) const { return Vec3<T> {content + second.content}; }

    inline Vec3<T> operator-(const Vec3<T> &second) const { return Vec3<T> {content - second.content}; }

    inline Vec3<T> operator*(const Vec3<T> &second) const { return Vec3<T> {content * second.content}; }

    inline Vec3<T> operator/(const Vec3<T> &second) const { return Vec3<T> {content / second.content}; }

    inline Vec3<T> operator*(const T k) const { return Vec3<T> {content * k}; }

    inline Vec3<T> operator/(T k) const { return Vec3<T> {content / k}; }

    inline Vec3<T> &operator+=(const Vec3<T> &second) { content += second.content; return *this; }

    inline Vec3<T> &operator-=(const Vec3<T> &second) { content -= second.content; return *this; }

    inline Vec3<T> &operator*=(const Vec3<T> &second) { content *= second.content; return *this; }

    inline Vec3<T> &operator*=(const T k) { content *= k; return *this; }

    inline Vec3<T> &operator/=(const Vec3<T> &second) { content /= second.content; return *this; }

    inline Vec3<T> &operator/=(const T k) { content /= k; return *this; }

    void transform_unbound(const Matrix4<T> &other) {

        content = {
                other.transform_x(content[0], content[1], content[2], 0),
                other.transform_y(content[0], content[1], content[2], 0),
                other.transform_z(content[0], content[1], content[2], 0)
        };
    }

    inline Vec3<T> &operator*=(const Matrix4<T> &other) {

        content = {
            other.transform_x(content[0], content[1], content[2]),
            other.transform_y(content[0], content[1], content[2]),
            other.transform_z(content[0], content[1], content[2])
        };

        return *this;
    }

    inline bool operator==(const Vec3<T> &second) const {
        const auto res = content - second.content;
        return fabs(res[0]) < FLOAT_EPS && fabs(res[1]) < FLOAT_EPS && fabs(res[2]) < FLOAT_EPS;
    }
};

typedef Vec3<double> Vec3d;
typedef Vec3<float> Vec3f;