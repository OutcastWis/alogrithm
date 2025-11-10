#pragma once

#include <cmath>

namespace geo {
	// 既是向量, 又是点
	struct point2d {

		point2d() {

		}
		point2d(double x, double y) : x_(x), y_(y) {

		}

		double length2() const {
			return x_ * x_ + y_ * y_;
		}

		double operator[](const int idx) const {
			return idx == 0 ? x_ : y_;
		}

		double dot(const point2d& other) const {
			return x_ * other.x_ + y_ * other.y_;
		}

		double cross(const point2d& v) const {
			return x_ * v.y_ - y_ * v.x_;
		}

		bool is_zero(double tol) const {
			return std::fabs(x_) < tol && std::fabs(y_) < tol;
		}

		bool is_parallel(const point2d& other, double tol) const {
			return std::fabs(cross(other)) < tol;
		}

		point2d operator*(double k) const {
			return { x_ * k, y_ * k };
		}
		point2d operator/(double k) const {
			return { x_ / k, y_ / k };
		}
		point2d operator+(const point2d& v) const {
			return { x_ + v.x_, y_ + v.y_ };
		}
		point2d operator-(const point2d& v) const {
			return { x_ - v.x_, y_ - v.y_ };
		}
		bool operator==(const point2d& other) const {
			return fabs(x_ - other.x_) < 1e-9 && fabs(y_ - other.y_) < 1e-9;
		}


		friend point2d operator*(double k, const point2d& pt) {
			return pt * k;
		}

		double x_ = 0;
		double y_ = 0;
	};
}