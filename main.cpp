#include <Novice.h>
#include <cmath>
#include <assert.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include <imgui.h>

const char kWindowTitle[] = "LD2A_04_トヨダヤヤ_MT3_02_04";

//================================================================
// 構造体の宣言
//================================================================

struct Matrix4x4
{
	float m[4][4];
};

struct Vector3
{
	float x;
	float y;
	float z;
};

struct Segment
{
	Vector3 origin;
	Vector3 diff;
};

struct Triangle
{
	Vector3 vertices[3];
};

//================================================================
// 関数の宣言
//================================================================

// 透視投影行列
Matrix4x4 MakePerspectiveFovMatrix(float fovY, float aspectRatio, float nearClip, float farClip);

// 正射影行列
Matrix4x4 MakeOrthographicMatrix(float left, float top, float right, float bottom, float nearClip, float farClip);

// ビューポート変換行列
Matrix4x4 MakeViewportMatrix(float left, float top, float width, float height, float minDepth, float maxDepth);

// 三次元アフィン変換行列
Matrix4x4 MakeAffineMatrix(const Vector3& scale, const Vector3& rotate, const Vector3& translate);

// 行列の積
Matrix4x4 Multiply(const Matrix4x4& m1, const Matrix4x4& m2);

// 逆行列
Matrix4x4 Inverse(const Matrix4x4& m);

// 座標変換
Vector3 Transform(const Vector3& vector, const Matrix4x4& matrix);

// グリットの描画
void DrawGrid(const Matrix4x4& worldViewProjectionMatrix, const Matrix4x4& viewportMatrix);

// 三角形の描画
void DrawTriangle(const Triangle& triangle, const Matrix4x4& worldViewProjectionMatrix, const Matrix4x4& viewportMatrix, uint32_t color);

// 当たり判定
bool isCollision(const Segment& segment, const Triangle& triangle);

// 法線と垂直なベクトルを求める
Vector3 Perpendicular(const Vector3& vector);

// ノルン(長さ)の関数
float Length(const Vector3& v);

// 正規化の関数
Vector3 Normalize(const Vector3& v);

// 積(ベクトル)
Vector3 Vector3Multiply(const float& v1, const Vector3& v2);

// クロス積
Vector3 Cross(const Vector3& v1, const Vector3& v2);

// 加算関数
Vector3 Add(const Vector3& v1, const Vector3& v2);

// 減算関数
Vector3 Subtract(const Vector3& v1, const Vector3& v2);
Vector3 FloatSubtract(const Vector3& v1, float v2);

// 内積の関数
float Dot(const Vector3& v1, const Vector3& v2);

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	// ライブラリの初期化
	Novice::Initialize(kWindowTitle, 1280, 720);

	// キー入力結果を受け取る箱
	char keys[256] = { 0 };
	char preKeys[256] = { 0 };

	//================================================================
	// 変数の宣言と初期化
	//================================================================
	const int kWindowWidth = 1280;
	const int kWindowHeight = 720;

	// カメラの位置と角度
	Vector3 cameraTranslate{ 0.0f,1.9f,-6.49f };
	Vector3 cameraRotate{ 0.26f,0.0f,0.0f };
	Segment segment = { {-0.0f,0.5f,1.0f}, {0.0f,0.5f,2.0f} };
	Triangle triangle;
	triangle.vertices[0] = { -1.0f, 0.0f, 0.0f };
	triangle.vertices[1] = { 0.0f, 1.0f, 0.0f };
	triangle.vertices[2] = { 1.0f, 0.0f, 0.0f };
	uint32_t color = 0xFFFFFFFF;

	// ウィンドウの×ボタンが押されるまでループ
	while (Novice::ProcessMessage() == 0) {
		// フレームの開始
		Novice::BeginFrame();

		// キー入力を受け取る
		memcpy(preKeys, keys, 256);
		Novice::GetHitKeyStateAll(keys);

		///
		/// ↓更新処理ここから
		///

		//================================================================
		// グリットの計算処理
		//================================================================

		Matrix4x4 cameraMatrix = MakeAffineMatrix({ 1.0f,1.0f,1.0f }, cameraRotate, cameraTranslate);
		Matrix4x4 viewMatrix = Inverse(cameraMatrix);
		Matrix4x4 projectionMatrix = MakePerspectiveFovMatrix(0.45f, float(kWindowWidth) / float(kWindowHeight), 0.1f, 100.0f);
		Matrix4x4 worldViewProjectionMatrix = Multiply(viewMatrix, projectionMatrix);
		Matrix4x4 viewportMatrix = MakeViewportMatrix(0, 0, float(kWindowWidth), float(kWindowHeight), 0.0f, 1.0f);

		//================================================================
		// 球同士の衝突判定処理
		//================================================================

		if (isCollision(segment, triangle))
		{
			color = 0xe60033FF;
		}
		else
		{
			color = 0xFFFFFFFF;
		}

		///
		/// ↑更新処理ここまで
		///

		///
		/// ↓描画処理ここから
		///

		//================================================================
		// 描画処理
		//================================================================

		ImGui::Begin("Window");
		ImGui::DragFloat3("cameraTranslate", &cameraTranslate.x, 0.01f);
		ImGui::DragFloat3("cameraRotate", &cameraRotate.x, 0.01f);
		ImGui::DragFloat3("segment.origin", &segment.origin.x, 0.01f);
		ImGui::DragFloat("segment.diff", &segment.diff.x, 0.01f);
		ImGui::DragFloat3("Triangle.v1", &triangle.vertices[0].x, 0.01f);
		ImGui::DragFloat3("Triangle.v2", &triangle.vertices[1].x, 0.01f);
		ImGui::DragFloat3("Triangle.v3", &triangle.vertices[2].x, 0.01f);
		ImGui::End();

		DrawGrid(worldViewProjectionMatrix, viewportMatrix);
		Vector3 start = Transform(Transform(segment.origin, worldViewProjectionMatrix), viewportMatrix);
		Vector3 end = Transform(Transform(Add(segment.origin, segment.diff), worldViewProjectionMatrix), viewportMatrix);
		Novice::DrawLine(int(start.x), int(start.y), int(end.x), int(end.y), color);
		DrawTriangle(triangle, worldViewProjectionMatrix, viewportMatrix, WHITE);

		///
		/// ↑描画処理ここまで
		///

		// フレームの終了
		Novice::EndFrame();

		// ESCキーが押されたらループを抜ける
		if (preKeys[DIK_ESCAPE] == 0 && keys[DIK_ESCAPE] != 0) {
			break;
		}
	}

	// ライブラリの終了
	Novice::Finalize();
	return 0;
}

Matrix4x4 MakePerspectiveFovMatrix(float fovY, float aspectRatio, float nearClip, float farClip)
{
	Matrix4x4 ret;
	float cot = 1.0f / tanf(fovY / 2.0f);
	ret.m[0][0] = (1.0f / aspectRatio) * cot; ret.m[0][1] = 0.0f; ret.m[0][2] = 0.0f; ret.m[0][3] = 0.0f;
	ret.m[1][0] = 0.0f; ret.m[1][1] = cot; ret.m[1][2] = 0.0f; ret.m[1][3] = 0.0f;
	ret.m[2][0] = 0.0f; ret.m[2][1] = 0.0f; ret.m[2][2] = farClip / (farClip - nearClip); ret.m[2][3] = 1.0f;
	ret.m[3][0] = 0.0f; ret.m[3][1] = 0.0f; ret.m[3][2] = ((-1.0f * nearClip) * farClip) / (farClip - nearClip); ret.m[3][3] = 0.0f;

	return ret;
}

Matrix4x4 MakeOrthographicMatrix(float left, float top, float right, float bottom, float nearClip, float farClip)
{
	Matrix4x4 ret;
	ret.m[0][0] = 2.0f / (right - left); ret.m[0][1] = 0.0f; ret.m[0][2] = 0.0f; ret.m[0][3] = 0.0f;
	ret.m[1][0] = 0.0f; ret.m[1][1] = 2.0f / (top - bottom); ret.m[1][2] = 0.0f; ret.m[1][3] = 0.0f;
	ret.m[2][0] = 0.0f; ret.m[2][1] = 0.0f; ret.m[2][2] = 1.0f / (farClip - nearClip); ret.m[2][3] = 0.0f;
	ret.m[3][0] = (left + right) / (left - right); ret.m[3][1] = (top + bottom) / (bottom - top); ret.m[3][2] = nearClip / (nearClip - farClip); ret.m[3][3] = 1.0f;
	return ret;
}

Matrix4x4 MakeViewportMatrix(float left, float top, float width, float height, float minDepth, float maxDepth)
{
	Matrix4x4 ret;
	ret.m[0][0] = width / 2.0f; ret.m[0][1] = 0.0f; ret.m[0][2] = 0.0f; ret.m[0][3] = 0.0f;
	ret.m[1][0] = 0.0f; ret.m[1][1] = -1.0f * (height / 2); ret.m[1][2] = 0.0f; ret.m[1][3] = 0.0f;
	ret.m[2][0] = 0.0f; ret.m[2][1] = 0.0f; ret.m[2][2] = maxDepth - minDepth; ret.m[2][3] = 0.0f;
	ret.m[3][0] = left + (width / 2.0f); ret.m[3][1] = top + (height / 2.0f); ret.m[3][2] = minDepth; ret.m[3][3] = 1.0f;
	return ret;
}

Matrix4x4 MakeRotateXMatrix(float radian)
{
	Matrix4x4 ret;
	ret.m[0][0] = 1.0f; ret.m[0][1] = 0.0f; ret.m[0][2] = 0.0f; ret.m[0][3] = 0.0f;
	ret.m[1][0] = 0.0f; ret.m[1][1] = std::cos(radian); ret.m[1][2] = std::sin(radian); ret.m[1][3] = 0.0f;
	ret.m[2][0] = 0.0f; ret.m[2][1] = -std::sin(radian); ret.m[2][2] = std::cos(radian); ret.m[2][3] = 0.0f;
	ret.m[3][0] = 0.0f; ret.m[3][1] = 0.0f; ret.m[3][2] = 0.0f; ret.m[3][3] = 1.0f;
	return ret;
}

Matrix4x4 MakeRotateYMatrix(float radian)
{
	Matrix4x4 ret;
	ret.m[0][0] = std::cos(radian); ret.m[0][1] = 0.0f; ret.m[0][2] = -std::sin(radian); ret.m[0][3] = 0.0f;
	ret.m[1][0] = 0.0f; ret.m[1][1] = 1.0f; ret.m[1][2] = 0.0f; ret.m[1][3] = 0.0f;
	ret.m[2][0] = std::sin(radian); ret.m[2][1] = 0.0f; ret.m[2][2] = std::cos(radian); ret.m[2][3] = 0.0f;
	ret.m[3][0] = 0.0f; ret.m[3][1] = 0.0f; ret.m[3][2] = 0.0f; ret.m[3][3] = 1.0f;
	return ret;
}

Matrix4x4 MakeRotateZMatrix(float radian)
{
	Matrix4x4 ret;
	ret.m[0][0] = std::cos(radian); ret.m[0][1] = std::sin(radian); ret.m[0][2] = 0.0f; ret.m[0][3] = 0.0f;
	ret.m[1][0] = -std::sin(radian); ret.m[1][1] = std::cos(radian); ret.m[1][2] = 0.0f; ret.m[1][3] = 0.0f;
	ret.m[2][0] = 0.0f; ret.m[2][1] = 0.0f; ret.m[2][2] = 1.0f; ret.m[2][3] = 0.0f;
	ret.m[3][0] = 0.0f; ret.m[3][1] = 0.0f; ret.m[3][2] = 0.0f; ret.m[3][3] = 1.0f;
	return ret;
}

Matrix4x4 Multiply(const Matrix4x4& m1, const Matrix4x4& m2)
{
	Matrix4x4 ret;
	ret.m[0][0] = m1.m[0][0] * m2.m[0][0] + m1.m[0][1] * m2.m[1][0] + m1.m[0][2] * m2.m[2][0] + m1.m[0][3] * m2.m[3][0];
	ret.m[0][1] = m1.m[0][0] * m2.m[0][1] + m1.m[0][1] * m2.m[1][1] + m1.m[0][2] * m2.m[2][1] + m1.m[0][3] * m2.m[3][1];
	ret.m[0][2] = m1.m[0][0] * m2.m[0][2] + m1.m[0][1] * m2.m[1][2] + m1.m[0][2] * m2.m[2][2] + m1.m[0][3] * m2.m[3][2];
	ret.m[0][3] = m1.m[0][0] * m2.m[0][3] + m1.m[0][1] * m2.m[1][3] + m1.m[0][2] * m2.m[2][3] + m1.m[0][3] * m2.m[3][3];
	ret.m[1][0] = m1.m[1][0] * m2.m[0][0] + m1.m[1][1] * m2.m[1][0] + m1.m[1][2] * m2.m[2][0] + m1.m[1][3] * m2.m[3][0];
	ret.m[1][1] = m1.m[1][0] * m2.m[0][1] + m1.m[1][1] * m2.m[1][1] + m1.m[1][2] * m2.m[2][1] + m1.m[1][3] * m2.m[3][1];
	ret.m[1][2] = m1.m[1][0] * m2.m[0][2] + m1.m[1][1] * m2.m[1][2] + m1.m[1][2] * m2.m[2][2] + m1.m[1][3] * m2.m[3][2];
	ret.m[1][3] = m1.m[1][0] * m2.m[0][3] + m1.m[1][1] * m2.m[1][3] + m1.m[1][2] * m2.m[2][3] + m1.m[1][3] * m2.m[3][3];
	ret.m[2][0] = m1.m[2][0] * m2.m[0][0] + m1.m[2][1] * m2.m[1][0] + m1.m[2][2] * m2.m[2][0] + m1.m[2][3] * m2.m[3][0];
	ret.m[2][1] = m1.m[2][0] * m2.m[0][1] + m1.m[2][1] * m2.m[1][1] + m1.m[2][2] * m2.m[2][1] + m1.m[2][3] * m2.m[3][1];
	ret.m[2][2] = m1.m[2][0] * m2.m[0][2] + m1.m[2][1] * m2.m[1][2] + m1.m[2][2] * m2.m[2][2] + m1.m[2][3] * m2.m[3][2];
	ret.m[2][3] = m1.m[2][0] * m2.m[0][3] + m1.m[2][1] * m2.m[1][3] + m1.m[2][2] * m2.m[2][3] + m1.m[2][3] * m2.m[3][3];
	ret.m[3][0] = m1.m[3][0] * m2.m[0][0] + m1.m[3][1] * m2.m[1][0] + m1.m[3][2] * m2.m[2][0] + m1.m[3][3] * m2.m[3][0];
	ret.m[3][1] = m1.m[3][0] * m2.m[0][1] + m1.m[3][1] * m2.m[1][1] + m1.m[3][2] * m2.m[2][1] + m1.m[3][3] * m2.m[3][1];
	ret.m[3][2] = m1.m[3][0] * m2.m[0][2] + m1.m[3][1] * m2.m[1][2] + m1.m[3][2] * m2.m[2][2] + m1.m[3][3] * m2.m[3][2];
	ret.m[3][3] = m1.m[3][0] * m2.m[0][3] + m1.m[3][1] * m2.m[1][3] + m1.m[3][2] * m2.m[2][3] + m1.m[3][3] * m2.m[3][3];
	return ret;
}


Matrix4x4 MakeAffineMatrix(const Vector3& scale, const Vector3& rotate, const Vector3& translate)
{
	Matrix4x4 rotateX = MakeRotateXMatrix(rotate.x);
	Matrix4x4 rotateY = MakeRotateYMatrix(rotate.y);
	Matrix4x4 rotateZ = MakeRotateZMatrix(rotate.z);
	Matrix4x4 rotateXYZ = Multiply(rotateX, Multiply(rotateY, rotateZ));

	Matrix4x4 ret;
	ret.m[0][0] = scale.x * rotateXYZ.m[0][0]; ret.m[0][1] = scale.x * rotateXYZ.m[0][1]; ret.m[0][2] = scale.x * rotateXYZ.m[0][2]; ret.m[0][3] = 0.0f;
	ret.m[1][0] = scale.y * rotateXYZ.m[1][0]; ret.m[1][1] = scale.y * rotateXYZ.m[1][1]; ret.m[1][2] = scale.y * rotateXYZ.m[1][2]; ret.m[1][3] = 0.0f;
	ret.m[2][0] = scale.z * rotateXYZ.m[2][0]; ret.m[2][1] = scale.z * rotateXYZ.m[2][1]; ret.m[2][2] = scale.z * rotateXYZ.m[2][2]; ret.m[2][3] = 0.0f;
	ret.m[3][0] = translate.x; ret.m[3][1] = translate.y; ret.m[3][2] = translate.z; ret.m[3][3] = 1.0f;

	return ret;

}

Vector3 Transform(const Vector3& vector, const Matrix4x4& matrix)
{
	Vector3 result;
	result.x = vector.x * matrix.m[0][0] + vector.y * matrix.m[1][0] + vector.z * matrix.m[2][0] + 1.0f * matrix.m[3][0];
	result.y = vector.x * matrix.m[0][1] + vector.y * matrix.m[1][1] + vector.z * matrix.m[2][1] + 1.0f * matrix.m[3][1];
	result.z = vector.x * matrix.m[0][2] + vector.y * matrix.m[1][2] + vector.z * matrix.m[2][2] + 1.0f * matrix.m[3][2];
	float w = vector.x * matrix.m[0][3] + vector.y * matrix.m[1][3] + vector.z * matrix.m[2][3] + 1.0f * matrix.m[3][3];
	assert(w != 0);
	result.x /= w;
	result.y /= w;
	result.z /= w;
	return result;
}

bool isCollision(const Segment& segment, const Triangle& triangle)
{

	Vector3 p0 = segment.origin;
	Vector3 p1 = Add(segment.origin, segment.diff);

	Vector3 v0 = triangle.vertices[0];
	Vector3 v1 = triangle.vertices[1];
	Vector3 v2 = triangle.vertices[2];

	Vector3 edge1 = Subtract(v1, v0);
	Vector3 edge2 = Subtract(v2, v0);

	// 法線ベクトル
	Vector3 normal = Normalize(Cross(edge1, edge2));
	float d = -Dot(normal, v0);

	float dist0 = Dot(normal, p0) + d;
	float dist1 = Dot(normal, p1) + d;

	// 両端が同じ側にある場合、交差しない
	if (dist0 * dist1 > 0.0f) {
		return false;
	}

	// t: 線分と平面の交差割合
	float t = dist0 / (dist0 - dist1);
	if (t < 0.0f || t > 1.0f) {
		return false;
	}

	// 衝突点の計算
	Vector3 direction = Subtract(p1, p0);
	Vector3 intersect = Add(p0, Vector3Multiply(t, direction));

	// 各辺ベクトル
	Vector3 v01 = Subtract(v1, v0);
	Vector3 v12 = Subtract(v2, v1);
	Vector3 v20 = Subtract(v0, v2);

	// 各頂点から衝突点へのベクトル
	Vector3 v0p = Subtract(intersect, v0);
	Vector3 v1p = Subtract(intersect, v1);
	Vector3 v2p = Subtract(intersect, v2);

	// 外積と法線との符号判定
	Vector3 cross01 = Cross(v01, v0p);
	Vector3 cross12 = Cross(v12, v1p);
	Vector3 cross20 = Cross(v20, v2p);

	if (Dot(cross01, normal) >= 0.0f &&
		Dot(cross12, normal) >= 0.0f &&
		Dot(cross20, normal) >= 0.0f)
	{
		return true;
	}

	return false;
}

void DrawGrid(const Matrix4x4& worldViewProjectionMatrix, const Matrix4x4& viewportMatrix)
{
	const float kGridHalfWidth = 2.0f; // gridの半分の幅
	const uint32_t kSubdivision = 10; // 分割数
	const float kGridEvery = (kGridHalfWidth * 2.0f) / float(kSubdivision); // 一つ分の長さ

	// 奥から手前への線を順番に引いていく
	for (uint32_t xIndex = 0; xIndex <= kSubdivision; ++xIndex)
	{
		float x = -kGridHalfWidth + kGridEvery * float(xIndex);
		Vector3 start = { x, 0.0f, -kGridHalfWidth };
		Vector3 end = { x, 0.0f, kGridHalfWidth };
		Vector3 startNdc = Transform(start, worldViewProjectionMatrix);
		Vector3 startScreen = Transform(startNdc, viewportMatrix);
		Vector3 endNdc = Transform(end, worldViewProjectionMatrix);
		Vector3 endScreen = Transform(endNdc, viewportMatrix);
		Novice::DrawLine(int(startScreen.x), int(startScreen.y), int(endScreen.x), int(endScreen.y), 0xFFFFFFFF);
	}

	// 左から右への線を順番に引いていく
	for (uint32_t zIndex = 0; zIndex <= kSubdivision; ++zIndex)
	{
		float z = -kGridHalfWidth + kGridEvery * float(zIndex);

		Vector3 start = { -kGridHalfWidth, 0.0f, z };
		Vector3 end = { kGridHalfWidth, 0.0f, z };
		Vector3 startNdc = Transform(start, worldViewProjectionMatrix);
		Vector3 startScreen = Transform(startNdc, viewportMatrix);
		Vector3 endNdc = Transform(end, worldViewProjectionMatrix);
		Vector3 endScreen = Transform(endNdc, viewportMatrix);
		Novice::DrawLine(int(startScreen.x), int(startScreen.y), int(endScreen.x), int(endScreen.y), 0xFFFFFFFF);
	}
}

void DrawTriangle(const Triangle& triangle, const Matrix4x4& worldViewProjectionMatrix, const Matrix4x4& viewportMatrix, uint32_t color)
{
	Vector3 v1 = Transform(Transform(triangle.vertices[0], worldViewProjectionMatrix), viewportMatrix);
	Vector3 v2 = Transform(Transform(triangle.vertices[1], worldViewProjectionMatrix), viewportMatrix);
	Vector3 v3 = Transform(Transform(triangle.vertices[2], worldViewProjectionMatrix), viewportMatrix);

	Novice::DrawLine(int(v1.x), int(v1.y), int(v2.x), int(v2.y), color);
	Novice::DrawLine(int(v2.x), int(v2.y), int(v3.x), int(v3.y), color);
	Novice::DrawLine(int(v3.x), int(v3.y), int(v1.x), int(v1.y), color);
}

Vector3 Perpendicular(const Vector3& vector)
{
	if (vector.x != 0.0f || vector.y != 0.0f)
	{
		return { -vector.y,vector.x,0.0f };
	}

	return { 0.0f,-vector.z,vector.y };
}

float Length(const Vector3& v)
{
	float ret;
	ret = sqrtf(v.x * v.x + v.y * v.y + v.z * v.z);
	return ret;
}

Vector3 Normalize(const Vector3& v)
{
	Vector3 ret;
	float length = Length(v);

	if (length != 0)
	{
		ret.x = v.x / length;
		ret.y = v.y / length;
		ret.z = v.z / length;
	}
	else
	{
		ret.x = 0;
		ret.y = 0;
		ret.z = 0;
	}

	return ret;
}

Vector3 Vector3Multiply(const float& v1, const Vector3& v2)
{
	Vector3 ret;
	ret.x = v1 * v2.x;
	ret.y = v1 * v2.y;
	ret.z = v1 * v2.z;
	return ret;
}

Vector3 Cross(const Vector3& v1, const Vector3& v2)
{
	Vector3 ret;
	ret = { (v1.y * v2.z) - (v1.z * v2.y), (v1.z * v2.x) - (v1.x * v2.z), (v1.x * v2.y) - (v1.y * v2.x) };
	return ret;
}

float Dot(const Vector3& v1, const Vector3& v2)
{
	float ret;
	ret = v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
	return ret;
}

Vector3 Add(const Vector3& v1, const Vector3& v2)
{
	Vector3 ret;
	ret.x = v1.x + v2.x;
	ret.y = v1.y + v2.y;
	ret.z = v1.z + v2.z;
	return ret;
}

Vector3 FloatSubtract(const Vector3& v1,float v2)
{
	Vector3 ret;
	ret.x = v1.x - v2;
	ret.y = v1.y - v2;
	ret.z = v1.z - v2;
	return ret;
}

Vector3 Subtract(const Vector3& v1, const Vector3& v2)
{
	Vector3 ret;
	ret.x = v1.x - v2.x;
	ret.y = v1.y - v2.y;
	ret.z = v1.z - v2.z;
	return ret;
}

Matrix4x4 Inverse(const Matrix4x4& m)
{
	Matrix4x4 ret;
	float a = (m.m[0][0] * m.m[1][1] * m.m[2][2] * m.m[3][3]) + (m.m[0][0] * m.m[1][2] * m.m[2][3] * m.m[3][1]) + (m.m[0][0] * m.m[1][3] * m.m[2][1] * m.m[3][2])
		- (m.m[0][0] * m.m[1][3] * m.m[2][2] * m.m[3][1]) - (m.m[0][0] * m.m[1][2] * m.m[2][1] * m.m[3][3]) - (m.m[0][0] * m.m[1][1] * m.m[2][3] * m.m[3][2])
		- (m.m[0][1] * m.m[1][0] * m.m[2][2] * m.m[3][3]) - (m.m[0][2] * m.m[1][0] * m.m[2][3] * m.m[3][1]) - (m.m[0][3] * m.m[1][0] * m.m[2][1] * m.m[3][2])
		+ (m.m[0][3] * m.m[1][0] * m.m[2][2] * m.m[3][1]) + (m.m[0][2] * m.m[1][0] * m.m[2][1] * m.m[3][3]) + (m.m[0][1] * m.m[1][0] * m.m[2][3] * m.m[3][2])
		+ (m.m[0][1] * m.m[1][2] * m.m[2][0] * m.m[3][3]) + (m.m[0][2] * m.m[1][3] * m.m[2][0] * m.m[3][1]) + (m.m[0][3] * m.m[1][1] * m.m[2][0] * m.m[3][2])
		- (m.m[0][3] * m.m[1][2] * m.m[2][0] * m.m[3][1]) - (m.m[0][2] * m.m[1][1] * m.m[2][0] * m.m[3][3]) - (m.m[0][1] * m.m[1][3] * m.m[2][0] * m.m[3][2])
		- (m.m[0][1] * m.m[1][2] * m.m[2][3] * m.m[3][0]) - (m.m[0][2] * m.m[1][3] * m.m[2][1] * m.m[3][0]) - (m.m[0][3] * m.m[1][1] * m.m[2][2] * m.m[3][0])
		+ (m.m[0][3] * m.m[1][2] * m.m[2][1] * m.m[3][0]) + (m.m[0][2] * m.m[1][1] * m.m[2][3] * m.m[3][0]) + (m.m[0][1] * m.m[1][3] * m.m[2][2] * m.m[3][0]);

	ret.m[0][0] = (1.0f / a) * ((m.m[1][1] * m.m[2][2] * m.m[3][3]) + (m.m[1][2] * m.m[2][3] * m.m[3][1]) + (m.m[1][3] * m.m[2][1] * m.m[3][2])
		- (m.m[1][3] * m.m[2][2] * m.m[3][1]) - (m.m[1][2] * m.m[2][1] * m.m[3][3]) - (m.m[1][1] * m.m[2][3] * m.m[3][2]));

	ret.m[0][1] = (1.0f / a) * ((-1.0f * (m.m[0][1] * m.m[2][2] * m.m[3][3])) - (m.m[0][2] * m.m[2][3] * m.m[3][1]) - (m.m[0][3] * m.m[2][1] * m.m[3][2])
		+ (m.m[0][3] * m.m[2][2] * m.m[3][1]) + (m.m[0][2] * m.m[2][1] * m.m[3][3]) + (m.m[0][1] * m.m[2][3] * m.m[3][2]));

	ret.m[0][2] = (1.0f / a) * ((m.m[0][1] * m.m[1][2] * m.m[3][3]) + (m.m[0][2] * m.m[1][3] * m.m[3][1]) + (m.m[0][3] * m.m[1][1] * m.m[3][2])
		- (m.m[0][3] * m.m[1][2] * m.m[3][1]) - (m.m[0][2] * m.m[1][1] * m.m[3][3]) - (m.m[0][1] * m.m[1][3] * m.m[3][2]));

	ret.m[0][3] = (1.0f / a) * ((-1.0f * (m.m[0][1] * m.m[1][2] * m.m[2][3])) - (m.m[0][2] * m.m[1][3] * m.m[2][1]) - (m.m[0][3] * m.m[1][1] * m.m[2][2])
		+ (m.m[0][3] * m.m[1][2] * m.m[2][1]) + (m.m[0][2] * m.m[1][1] * m.m[2][3]) + (m.m[0][1] * m.m[1][3] * m.m[2][2]));

	ret.m[1][0] = (1.0f / a) * (-1.0f * ((m.m[1][0] * m.m[2][2] * m.m[3][3])) - (m.m[1][2] * m.m[2][3] * m.m[3][0]) - (m.m[1][3] * m.m[2][0] * m.m[3][2])
		+ (m.m[1][3] * m.m[2][2] * m.m[3][0]) + (m.m[1][2] * m.m[2][0] * m.m[3][3]) + (m.m[1][0] * m.m[2][3] * m.m[3][2]));

	ret.m[1][1] = (1.0f / a) * ((m.m[0][0] * m.m[2][2] * m.m[3][3]) + (m.m[0][2] * m.m[2][3] * m.m[3][0]) + (m.m[0][3] * m.m[2][0] * m.m[3][2])
		- (m.m[0][3] * m.m[2][2] * m.m[3][0]) - (m.m[0][2] * m.m[2][1] * m.m[3][3]) - (m.m[0][0] * m.m[2][3] * m.m[3][2]));

	ret.m[1][2] = (1.0f / a) * (-1.0f * ((m.m[0][0] * m.m[1][2] * m.m[3][3])) - (m.m[0][2] * m.m[1][3] * m.m[3][0]) - (m.m[0][3] * m.m[1][0] * m.m[3][2])
		+ (m.m[0][3] * m.m[1][2] * m.m[3][0]) + (m.m[0][2] * m.m[1][0] * m.m[3][3]) + (m.m[0][1] * m.m[1][3] * m.m[3][2]));

	ret.m[1][3] = (1.0f / a) * ((m.m[0][0] * m.m[1][2] * m.m[2][3]) + (m.m[0][2] * m.m[1][3] * m.m[2][0]) + (m.m[0][3] * m.m[1][0] * m.m[2][2])
		- (m.m[0][3] * m.m[1][2] * m.m[2][0]) - (m.m[0][2] * m.m[1][0] * m.m[2][3]) - (m.m[0][0] * m.m[1][3] * m.m[2][2]));

	ret.m[2][0] = (1.0f / a) * ((m.m[1][0] * m.m[2][1] * m.m[3][3]) + (m.m[1][1] * m.m[2][3] * m.m[3][0]) + (m.m[1][3] * m.m[2][0] * m.m[3][1])
		- (m.m[1][3] * m.m[2][1] * m.m[3][0]) - (m.m[1][1] * m.m[2][0] * m.m[3][3]) - (m.m[1][0] * m.m[2][3] * m.m[3][1]));

	ret.m[2][1] = (1.0f / a) * ((-1.0f * (m.m[0][0] * m.m[2][1] * m.m[3][3])) - (m.m[0][1] * m.m[2][3] * m.m[3][0]) - (m.m[0][3] * m.m[2][0] * m.m[3][1])
		+ (m.m[0][3] * m.m[2][1] * m.m[3][0]) + (m.m[0][1] * m.m[2][0] * m.m[3][3]) + (m.m[0][0] * m.m[2][3] * m.m[3][1]));

	ret.m[2][2] = (1.0f / a) * ((m.m[0][0] * m.m[1][1] * m.m[3][3]) + (m.m[0][1] * m.m[1][3] * m.m[3][0]) + (m.m[0][3] * m.m[1][0] * m.m[3][1])
		- (m.m[0][3] * m.m[1][1] * m.m[3][0]) - (m.m[0][1] * m.m[1][0] * m.m[3][3]) - (m.m[0][0] * m.m[1][3] * m.m[3][1]));

	ret.m[2][3] = (1.0f / a) * ((-1.0f * (m.m[0][0] * m.m[1][1] * m.m[2][3])) - (m.m[0][1] * m.m[1][3] * m.m[2][0]) - (m.m[0][3] * m.m[1][0] * m.m[2][1])
		+ (m.m[0][3] * m.m[1][1] * m.m[2][0]) + (m.m[0][1] * m.m[1][0] * m.m[2][3]) + (m.m[0][0] * m.m[1][3] * m.m[2][1]));

	ret.m[3][0] = (1.0f / a) * (-1.0f * ((m.m[1][0] * m.m[2][1] * m.m[3][2])) - (m.m[1][1] * m.m[2][2] * m.m[3][0]) - (m.m[1][2] * m.m[2][0] * m.m[3][1])
		+ (m.m[1][2] * m.m[2][1] * m.m[3][0]) + (m.m[1][1] * m.m[2][0] * m.m[3][2]) + (m.m[1][1] * m.m[2][2] * m.m[2][1]));

	ret.m[3][1] = (1.0f / a) * ((m.m[0][0] * m.m[2][1] * m.m[3][2]) + (m.m[0][1] * m.m[2][2] * m.m[3][0]) + (m.m[0][2] * m.m[2][0] * m.m[3][1])
		- (m.m[0][2] * m.m[2][1] * m.m[3][0]) - (m.m[0][1] * m.m[2][0] * m.m[3][2]) - (m.m[0][0] * m.m[2][2] * m.m[3][1]));

	ret.m[3][2] = (1.0f / a) * (-1.0f * ((m.m[0][0] * m.m[1][1] * m.m[3][2])) - (m.m[0][1] * m.m[1][2] * m.m[3][0]) - (m.m[0][2] * m.m[1][0] * m.m[3][1])
		+ (m.m[0][2] * m.m[1][1] * m.m[3][0]) + (m.m[0][1] * m.m[1][0] * m.m[3][2]) + (m.m[0][0] * m.m[1][2] * m.m[3][1]));

	ret.m[3][3] = (1.0f / a) * ((m.m[0][0] * m.m[1][1] * m.m[2][2]) + (m.m[0][1] * m.m[1][2] * m.m[2][0]) + (m.m[0][2] * m.m[1][0] * m.m[2][1])
		- (m.m[0][2] * m.m[1][1] * m.m[2][0]) - (m.m[0][1] * m.m[1][0] * m.m[2][2]) - (m.m[0][0] * m.m[1][2] * m.m[2][1]));

	return ret;
}