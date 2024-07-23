#pragma once
#include <assert.h>
#include<cmath>
#define M_PI 3.14159265358979323846
#include"Struct.h"
Matrix4x4 MakeScaleMatrix(const Vector3& scale);

Matrix4x4 MakeRotateZMatrix(float radian);
Matrix4x4 MakeRotateYMatrix(float radian);

Matrix4x4 Multiply(const Matrix4x4& m1, const Matrix4x4& m2);

Matrix4x4 MakeTranslateMatrix(const Vector3& translate);

Matrix4x4 MakePerspectiveFovMatrix(float fovY, float aspectRatio, float nearClip, float farClip);

Matrix4x4 MakeOrthographicMatrix(float left, float right, float top, float bottom, float nearClip, float farClip);

Matrix4x4 MakeViewportMatrix(float left, float top, float width, float height, float minDepth, float maxDepth);

Matrix4x4 MakeAffineMatrix(const Vector3& scale, const Vector3& rotate, const Vector3& translate);

Matrix4x4 Inverse(const Matrix4x4& m);

Matrix4x4 MakeIdentity4x4();

float Length(const Vector3& vec);

Vector3 Normalize(const Vector3& vec);