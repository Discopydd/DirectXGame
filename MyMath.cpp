#include "MyMath.h"
namespace Math {
    Matrix4x4 Math::MakeScaleMatrix(const Vector3& scale) {
        Matrix4x4 result;

        result.m[0][0] = scale.x;
        result.m[0][1] = 0.0f;
        result.m[0][2] = 0.0f;
        result.m[0][3] = 0.0f;

        result.m[1][0] = 0.0f;
        result.m[1][1] = scale.y;
        result.m[1][2] = 0.0f;
        result.m[1][3] = 0.0f;

        result.m[2][0] = 0.0f;
        result.m[2][1] = 0.0f;
        result.m[2][2] = scale.z;
        result.m[2][3] = 0.0f;

        result.m[3][0] = 0.0f;
        result.m[3][1] = 0.0f;
        result.m[3][2] = 0.0f;
        result.m[3][3] = 1.0f;

        return result;
    }

    Matrix4x4 Math::MakeRotateZMatrix(float radian) {

        Matrix4x4 result;
        result.m[0][0] = cosf(radian);
        result.m[0][1] = sinf(radian);
        result.m[0][2] = 0.0f;
        result.m[0][3] = 0.0f;
        result.m[1][0] = -sinf(radian);
        result.m[1][1] = cosf(radian);
        result.m[1][2] = 0.0f;
        result.m[1][3] = 0.0f;
        result.m[2][1] = 0.0f;
        result.m[2][2] = 1.0f;
        result.m[2][3] = 0.0f;
        result.m[3][0] = 0.0f;
        result.m[3][1] = 0.0f;
        result.m[3][2] = 0.0f;
        result.m[3][3] = 1.0f;

        return result;
    }

    Matrix4x4 Math::MakeTranslateMatrix(const Vector3& translate) {
        Matrix4x4 matrix;

        matrix.m[0][0] = 1.0f;
        matrix.m[0][1] = 0.0f;
        matrix.m[0][2] = 0.0f;
        matrix.m[0][3] = 0.0f;

        matrix.m[1][0] = 0.0f;
        matrix.m[1][1] = 1.0f;
        matrix.m[1][2] = 0.0f;
        matrix.m[1][3] = 0.0f;

        matrix.m[2][0] = 0.0f;
        matrix.m[2][1] = 0.0f;
        matrix.m[2][2] = 1.0f;
        matrix.m[2][3] = 0.0f;

        matrix.m[3][0] = translate.x;
        matrix.m[3][1] = translate.y;
        matrix.m[3][2] = translate.z;
        matrix.m[3][3] = 1.0f;

        return matrix;
    }


    Matrix4x4 Math::MakeRotateYMatrix(float radian) {

        Matrix4x4 result;
        result.m[0][0] = cosf(radian);
        result.m[0][1] = 0.0f;
        result.m[0][2] = -sinf(radian);
        result.m[0][3] = 0.0f;
        result.m[1][0] = 0.0f;
        result.m[1][1] = 1.0f;
        result.m[1][2] = 0.0f;
        result.m[1][3] = 0.0f;
        result.m[2][0] = sinf(radian);
        result.m[2][1] = 0.0f;
        result.m[2][2] = cosf(radian);
        result.m[2][3] = 0.0f;
        result.m[3][0] = 0.0f;
        result.m[3][1] = 0.0f;
        result.m[3][2] = 0.0f;
        result.m[3][3] = 1.0f;

        return result;
    }

    Matrix4x4 Math::Multiply(const Matrix4x4& matrix1, const Matrix4x4& matrix2) {
        Matrix4x4 result = {};
        for (int i = 0; i < 4; ++i) {
            for (int j = 0; j < 4; ++j) {
                for (int k = 0; k < 4; ++k) {
                    result.m[i][j] += matrix1.m[i][k] * matrix2.m[k][j];
                }
            }
        }
        return result;
    }

    Matrix4x4 Math::MakeAffineMatrix(const Vector3& scale, const Vector3& rotation, const Vector3& translation) {
        // Scale
        Matrix4x4 Scale = { 0 };
        Scale.m[0][0] = scale.x;
        Scale.m[1][1] = scale.y;
        Scale.m[2][2] = scale.z;
        Scale.m[3][3] = 1;
        // Rotation
        Matrix4x4 RotationZ = { 0 };
        RotationZ.m[0][0] = cosf(rotation.z);
        RotationZ.m[0][1] = sinf(rotation.z);
        RotationZ.m[1][0] = -sinf(rotation.z);
        RotationZ.m[1][1] = cosf(rotation.z);
        RotationZ.m[2][2] = RotationZ.m[3][3] = 1;
        Matrix4x4 RotationX = { 0 };
        RotationX.m[1][1] = cosf(rotation.x);
        RotationX.m[1][2] = sinf(rotation.x);
        RotationX.m[2][1] = -sinf(rotation.x);
        RotationX.m[2][2] = cosf(rotation.x);
        RotationX.m[0][0] = RotationX.m[3][3] = 1;
        Matrix4x4 RotationY = { 0 };
        RotationY.m[0][0] = cosf(rotation.y);
        RotationY.m[2][0] = sinf(rotation.y);
        RotationY.m[0][2] = -sinf(rotation.y);
        RotationY.m[2][2] = cosf(rotation.y);
        RotationY.m[1][1] = RotationY.m[3][3] = 1;
        Matrix4x4 Rotation = Multiply(RotationX, Multiply(RotationY, RotationZ));
        // Translation
        Matrix4x4 Translation = { 0 };
        Translation.m[0][0] = Translation.m[1][1] = Translation.m[2][2] = Translation.m[3][3] = 1;
        Translation.m[3][0] = translation.x;
        Translation.m[3][1] = translation.y;
        Translation.m[3][2] = translation.z;

        return Multiply(Scale, Multiply(Rotation, Translation));
    }

    Matrix4x4 Math::Inverse(const Matrix4x4& m) {
        float determinant =
            +m.m[0][0] * m.m[1][1] * m.m[2][2] * m.m[3][3]
            + m.m[0][0] * m.m[1][2] * m.m[2][3] * m.m[3][1]
            + m.m[0][0] * m.m[1][3] * m.m[2][1] * m.m[3][2]
            - m.m[0][0] * m.m[1][3] * m.m[2][2] * m.m[3][1]
            - m.m[0][0] * m.m[1][2] * m.m[2][1] * m.m[3][3]
            - m.m[0][0] * m.m[1][1] * m.m[2][3] * m.m[3][2]
            - m.m[0][1] * m.m[1][0] * m.m[2][2] * m.m[3][3]
            - m.m[0][2] * m.m[1][0] * m.m[2][3] * m.m[3][1]
            - m.m[0][3] * m.m[1][0] * m.m[2][1] * m.m[3][2]
            + m.m[0][3] * m.m[1][0] * m.m[2][2] * m.m[3][1]
            + m.m[0][2] * m.m[1][0] * m.m[2][1] * m.m[3][3]
            + m.m[0][1] * m.m[1][0] * m.m[2][3] * m.m[3][2]
            + m.m[0][1] * m.m[1][2] * m.m[2][0] * m.m[3][3]
            + m.m[0][2] * m.m[1][3] * m.m[2][0] * m.m[3][1]
            + m.m[0][3] * m.m[1][1] * m.m[2][0] * m.m[3][2]
            - m.m[0][3] * m.m[1][2] * m.m[2][0] * m.m[3][1]
            - m.m[0][2] * m.m[1][1] * m.m[2][0] * m.m[3][3]
            - m.m[0][1] * m.m[1][3] * m.m[2][0] * m.m[3][2]
            - m.m[0][1] * m.m[1][2] * m.m[2][3] * m.m[3][0]
            - m.m[0][2] * m.m[1][3] * m.m[2][1] * m.m[3][0]
            - m.m[0][3] * m.m[1][1] * m.m[2][2] * m.m[3][0]
            + m.m[0][3] * m.m[1][2] * m.m[2][1] * m.m[3][0]
            + m.m[0][2] * m.m[1][1] * m.m[2][3] * m.m[3][0]
            + m.m[0][1] * m.m[1][3] * m.m[2][2] * m.m[3][0];

        Matrix4x4 result = {};
        float recpDeterminant = 1.0f / determinant;

        result.m[0][0] = (m.m[1][1] * m.m[2][2] * m.m[3][3] + m.m[1][2] * m.m[2][3] * m.m[3][1] +
            m.m[1][3] * m.m[2][1] * m.m[3][2] - m.m[1][3] * m.m[2][2] * m.m[3][1] -
            m.m[1][2] * m.m[2][1] * m.m[3][3] - m.m[1][1] * m.m[2][3] * m.m[3][2]) * recpDeterminant;
        result.m[0][1] = (-m.m[0][1] * m.m[2][2] * m.m[3][3] - m.m[0][2] * m.m[2][3] * m.m[3][1] -
            m.m[0][3] * m.m[2][1] * m.m[3][2] + m.m[0][3] * m.m[2][2] * m.m[3][1] +
            m.m[0][2] * m.m[2][1] * m.m[3][3] + m.m[0][1] * m.m[2][3] * m.m[3][2]) * recpDeterminant;
        result.m[0][2] = (m.m[0][1] * m.m[1][2] * m.m[3][3] + m.m[0][2] * m.m[1][3] * m.m[3][1] +
            m.m[0][3] * m.m[1][1] * m.m[3][2] - m.m[0][3] * m.m[1][2] * m.m[3][1] -
            m.m[0][2] * m.m[1][1] * m.m[3][3] - m.m[0][1] * m.m[1][3] * m.m[3][2]) * recpDeterminant;
        result.m[0][3] = (-m.m[0][1] * m.m[1][2] * m.m[2][3] - m.m[0][2] * m.m[1][3] * m.m[2][1] -
            m.m[0][3] * m.m[1][1] * m.m[2][2] + m.m[0][3] * m.m[1][2] * m.m[2][1] +
            m.m[0][2] * m.m[1][1] * m.m[2][3] + m.m[0][1] * m.m[1][3] * m.m[2][2]) * recpDeterminant;

        result.m[1][0] = (-m.m[1][0] * m.m[2][2] * m.m[3][3] - m.m[1][2] * m.m[2][3] * m.m[3][0] -
            m.m[1][3] * m.m[2][0] * m.m[3][2] + m.m[1][3] * m.m[2][2] * m.m[3][0] +
            m.m[1][2] * m.m[2][0] * m.m[3][3] + m.m[1][0] * m.m[2][3] * m.m[3][2]) * recpDeterminant;
        result.m[1][1] = (m.m[0][0] * m.m[2][2] * m.m[3][3] + m.m[0][2] * m.m[2][3] * m.m[3][0] +
            m.m[0][3] * m.m[2][0] * m.m[3][2] - m.m[0][3] * m.m[2][2] * m.m[3][0] -
            m.m[0][2] * m.m[2][0] * m.m[3][3] - m.m[0][0] * m.m[2][3] * m.m[3][2]) * recpDeterminant;
        result.m[1][2] = (-m.m[0][0] * m.m[1][2] * m.m[3][3] - m.m[0][2] * m.m[1][3] * m.m[3][0] -
            m.m[0][3] * m.m[1][0] * m.m[3][2] + m.m[0][3] * m.m[1][2] * m.m[3][0] +
            m.m[0][2] * m.m[1][0] * m.m[3][3] + m.m[0][0] * m.m[1][3] * m.m[3][2]) * recpDeterminant;
        result.m[1][3] = (m.m[0][0] * m.m[1][2] * m.m[2][3] + m.m[0][2] * m.m[1][3] * m.m[2][0] +
            m.m[0][3] * m.m[1][0] * m.m[2][2] - m.m[0][3] * m.m[1][2] * m.m[2][0] -
            m.m[0][2] * m.m[1][0] * m.m[2][3] - m.m[0][0] * m.m[1][3] * m.m[2][2]) * recpDeterminant;

        result.m[2][0] = (m.m[1][0] * m.m[2][1] * m.m[3][3] + m.m[1][1] * m.m[2][3] * m.m[3][0] +
            m.m[1][3] * m.m[2][0] * m.m[3][1] - m.m[1][3] * m.m[2][1] * m.m[3][0] -
            m.m[1][1] * m.m[2][0] * m.m[3][3] - m.m[1][0] * m.m[2][3] * m.m[3][1]) * recpDeterminant;
        result.m[2][1] = (-m.m[0][0] * m.m[2][1] * m.m[3][3] - m.m[0][1] * m.m[2][3] * m.m[3][0] -
            m.m[0][3] * m.m[2][0] * m.m[3][1] + m.m[0][3] * m.m[2][1] * m.m[3][0] +
            m.m[0][1] * m.m[2][0] * m.m[3][3] + m.m[0][0] * m.m[2][3] * m.m[3][1]) * recpDeterminant;
        result.m[2][2] = (m.m[0][0] * m.m[1][1] * m.m[3][3] + m.m[0][1] * m.m[1][3] * m.m[3][0] +
            m.m[0][3] * m.m[1][0] * m.m[3][1] - m.m[0][3] * m.m[1][1] * m.m[3][0] -
            m.m[0][1] * m.m[1][0] * m.m[3][3] - m.m[0][0] * m.m[1][3] * m.m[3][1]) * recpDeterminant;
        result.m[2][3] = (-m.m[0][0] * m.m[1][1] * m.m[2][3] - m.m[0][1] * m.m[1][3] * m.m[2][0] -
            m.m[0][3] * m.m[1][0] * m.m[2][1] + m.m[0][3] * m.m[1][1] * m.m[2][0] +
            m.m[0][1] * m.m[1][0] * m.m[2][3] + m.m[0][0] * m.m[1][3] * m.m[2][1]) * recpDeterminant;

        result.m[3][0] = (-m.m[1][0] * m.m[2][1] * m.m[3][2] - m.m[1][1] * m.m[2][2] * m.m[3][0] -
            m.m[1][2] * m.m[2][0] * m.m[3][1] + m.m[1][2] * m.m[2][1] * m.m[3][0] +
            m.m[1][1] * m.m[2][0] * m.m[3][2] + m.m[1][0] * m.m[2][2] * m.m[3][1]) * recpDeterminant;
        result.m[3][1] = (m.m[0][0] * m.m[2][1] * m.m[3][2] + m.m[0][1] * m.m[2][2] * m.m[3][0] +
            m.m[0][2] * m.m[2][0] * m.m[3][1] - m.m[0][2] * m.m[2][1] * m.m[3][0] -
            m.m[0][1] * m.m[2][0] * m.m[3][2] - m.m[0][0] * m.m[2][2] * m.m[3][1]) * recpDeterminant;
        result.m[3][2] = (-m.m[0][0] * m.m[1][1] * m.m[3][2] - m.m[0][1] * m.m[1][2] * m.m[3][0] -
            m.m[0][2] * m.m[1][0] * m.m[3][1] + m.m[0][2] * m.m[1][1] * m.m[3][0] +
            m.m[0][1] * m.m[1][0] * m.m[3][2] + m.m[0][0] * m.m[1][2] * m.m[3][1]) * recpDeterminant;
        result.m[3][3] = (m.m[0][0] * m.m[1][1] * m.m[2][2] + m.m[0][1] * m.m[1][2] * m.m[2][0] +
            m.m[0][2] * m.m[1][0] * m.m[2][1] - m.m[0][2] * m.m[1][1] * m.m[2][0] -
            m.m[0][1] * m.m[1][0] * m.m[2][2] - m.m[0][0] * m.m[1][2] * m.m[2][1]) * recpDeterminant;

        return result;
    }

    Vector3 Math::Transform(const Vector3& vector, const Matrix4x4& matrix) {
        Vector3 result;
        result.x = vector.x * matrix.m[0][0] + vector.y * matrix.m[1][0] + vector.z * matrix.m[2][0] + matrix.m[3][0];
        result.y = vector.x * matrix.m[0][1] + vector.y * matrix.m[1][1] + vector.z * matrix.m[2][1] + matrix.m[3][1];
        result.z = vector.x * matrix.m[0][2] + vector.y * matrix.m[1][2] + vector.z * matrix.m[2][2] + matrix.m[3][2];
        float w = vector.x * matrix.m[0][3] + vector.y * matrix.m[1][3] + vector.z * matrix.m[2][3] + matrix.m[3][3];

        assert(w != 0.0f);

        result.x /= w;
        result.y /= w;
        result.z /= w;

        return result;
    }

    Matrix4x4 Math::MakePerspectiveFovMatrix(float fovY, float aspectRatio, float nearClip, float farClip) {
        Matrix4x4 result = {};

        float tanHalfFovY = tanf(fovY * 0.5f);
        float cot = 1.0f / tanHalfFovY;

        result.m[0][0] = cot / aspectRatio;
        result.m[0][1] = 0.0f;
        result.m[0][2] = 0.0f;
        result.m[0][3] = 0.0f;

        result.m[1][0] = 0.0f;
        result.m[1][1] = cot;
        result.m[1][2] = 0.0f;
        result.m[1][3] = 0.0f;

        result.m[2][0] = 0.0f;
        result.m[2][1] = 0.0f;
        result.m[2][2] = farClip / (farClip - nearClip);
        result.m[2][3] = 1.0f;

        result.m[3][0] = 0.0f;
        result.m[3][1] = 0.0f;
        result.m[3][2] = -(nearClip * farClip) / (farClip - nearClip);
        result.m[3][3] = 0.0f;

        return result;
    }

    Matrix4x4 Math::MakeOrthographicMatrix(float left, float right, float top, float bottom, float nearClip, float farClip) {
        Matrix4x4 result;


        result.m[0][0] = 2.0f / (right - left);
        result.m[0][1] = 0.0f;
        result.m[0][2] = 0.0f;
        result.m[0][3] = 0.0f;

        result.m[1][0] = 0.0f;
        result.m[1][1] = 2.0f / (top - bottom);
        result.m[1][2] = 0.0f;
        result.m[1][3] = 0.0f;

        result.m[2][0] = 0.0f;
        result.m[2][1] = 0.0f;
        result.m[2][2] = 1.0f / (farClip - nearClip);
        result.m[2][3] = 0.0f;

        result.m[3][0] = (right + left) / (left - right);
        result.m[3][1] = (top + bottom) / (bottom - top);
        result.m[3][2] = nearClip / (nearClip - farClip);
        result.m[3][3] = 1.0f;

        return result;
    }

    Matrix4x4 Math::MakeViewportMatrix(float left, float top, float width, float height, float minDepth, float maxDepth) {
        Matrix4x4 result;

        result.m[0][0] = width / 2.0f;
        result.m[0][1] = 0.0f;
        result.m[0][2] = 0.0f;
        result.m[0][3] = 0.0f;

        result.m[1][0] = 0.0f;
        result.m[1][1] = -height / 2.0f;
        result.m[1][2] = 0.0f;
        result.m[1][3] = 0.0f;

        result.m[2][0] = 0.0f;
        result.m[2][1] = 0.0f;
        result.m[2][2] = maxDepth - minDepth;
        result.m[2][3] = 0.0f;

        result.m[3][0] = left + width / 2.0f;
        result.m[3][1] = top + height / 2.0f;
        result.m[3][2] = minDepth;
        result.m[3][3] = 1.0f;

        return result;
    }

    Matrix4x4 Math::MakeIdentity4x4() {
        Matrix4x4 matrix = {};
        matrix.m[0][0] = 1.0f;
        matrix.m[1][1] = 1.0f;
        matrix.m[2][2] = 1.0f;
        matrix.m[3][3] = 1.0f;
        return matrix;
    }


    float Math::Length(const Vector3& vec) {
        return std::sqrt(vec.x * vec.x + vec.y * vec.y + vec.z * vec.z);
    }

    Vector3 Math::Normalize(const Vector3& vec) {
        float length = Length(vec);
        if (length != 0) {
            return { vec.x / length, vec.y / length, vec.z / length };
        }
        return { 0, 0, 0 };
    }

    Vector3 Math::Add(const Vector3& v1, const Vector3& v2) {
        return { v1.x + v2.x, v1.y + v2.y, v1.z + v2.z };
    }

    Vector3 Math::Subtract(const Vector3& v1, const Vector3& v2) {
        return { v1.x - v2.x, v1.y - v2.y, v1.z - v2.z };
    }
}
