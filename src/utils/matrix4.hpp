#pragma once

#include <cmath>
#include <cstring>

template<typename ValueType>
class Matrix4 {
public:
    ValueType data[16];

    Matrix4() { reset(); };
    ~Matrix4() = default;
    Matrix4(const Matrix4& other) { set(other.data); };
    Matrix4& operator=(const Matrix4& other);

    explicit Matrix4(const ValueType (&data)[16]) { set(data); };

    Matrix4 operator*(const Matrix4& other) const;

    void set(const ValueType (&other)[16]);

    ValueType transform_x(ValueType x, ValueType y, ValueType z) const { return x * data[0] + y * data[4] + z * data[8] + data[12]; }
    ValueType transform_y(ValueType x, ValueType y, ValueType z) const { return x * data[1] + y * data[5] + z * data[9] + data[13]; }
    ValueType transform_z(ValueType x, ValueType y, ValueType z) const { return x * data[2] + y * data[6] + z * data[10] + data[14]; }

    ValueType transform_x(ValueType x, ValueType y, ValueType z, ValueType w) const { return x * data[0] + y * data[4] + z * data[8] + w * data[12]; }
    ValueType transform_y(ValueType x, ValueType y, ValueType z, ValueType w) const { return x * data[1] + y * data[5] + z * data[9] + w * data[13]; }
    ValueType transform_z(ValueType x, ValueType y, ValueType z, ValueType w) const { return x * data[2] + y * data[6] + z * data[10] + w * data[14]; }

    static Matrix4<ValueType> rotation_x_matrix(ValueType angle);
    static Matrix4<ValueType> rotation_y_matrix(ValueType angle);
    static Matrix4<ValueType> rotation_z_matrix(ValueType angle);
    static Matrix4<ValueType> scale_matrix(ValueType x, ValueType y, ValueType z);
    static Matrix4<ValueType> translation_matrix(ValueType x, ValueType y, ValueType z);

    void reset();
};

template<typename ValueType>
Matrix4<ValueType>& Matrix4<ValueType>::operator=(const Matrix4<ValueType> &other) {
    if (this == &other) return *this;
    set(other.data);
    return *this;
}

template<typename ValueType>
Matrix4<ValueType> Matrix4<ValueType>::operator*(const Matrix4<ValueType> &other) const {
    Matrix4<ValueType> result;

    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++){
            result.data[i * 4 + j] = 0;
            for (int k = 0; k < 4; k++){
                result.data[i * 4 + j] += data[i * 4 + k] * other.data[k * 4 + j];
            }
        }
    }

    return result;
}

template<typename ValueType>
Matrix4<ValueType> Matrix4<ValueType>::scale_matrix(ValueType x, ValueType y, ValueType z) {
    return Matrix4<ValueType>({
        x, 0, 0, 0,
        0, y, 0, 0,
        0, 0, z, 0,
        0, 0, 0, 1
    });
}

template<typename ValueType>
Matrix4<ValueType> Matrix4<ValueType>::translation_matrix(ValueType x, ValueType y, ValueType z) {
    return Matrix4<ValueType>({
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        x, y, z, 1
    });
}

template<typename ValueType>
Matrix4<ValueType> Matrix4<ValueType>::rotation_x_matrix(ValueType angle) {
    ValueType s = sin(angle);
    ValueType c = cos(angle);
    return Matrix4<ValueType>({
        1, 0,  0, 0,
        0, c,  s, 0,
        0, -s, c, 0,
        0, 0,  0, 1
    });
}

template<typename ValueType>
Matrix4<ValueType> Matrix4<ValueType>::rotation_y_matrix(ValueType angle) {
    ValueType s = sin(angle);
    ValueType c = cos(angle);
    return Matrix4<ValueType>({
        c,  0, s, 0,
        0,  1, 0, 0,
        -s, 0, c, 0,
        0,  0, 0, 1
    });
}

template<typename ValueType>
Matrix4<ValueType> Matrix4<ValueType>::rotation_z_matrix(ValueType angle) {
    ValueType s = sin(angle);
    ValueType c = cos(angle);
    return Matrix4<ValueType>({
        c,  s, 0, 0,
        -s, c, 0, 0,
        0,  0, 1, 0,
        0,  0, 0, 1
    });
}

template<typename ValueType>
void Matrix4<ValueType>::set(const ValueType (&other)[16]) {
    for(int i = 0; i < 16; i++) data[i] = other[i];
}

template<typename ValueType>
void Matrix4<ValueType>::reset() {
    set({ 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 });
}

typedef Matrix4<double> Matrix4d;
typedef Matrix4<float> Matrix4f;