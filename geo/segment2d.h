#pragma once

#include <geo/point2d.h>

#include <utility>

namespace geo {
	struct segment2d {

		segment2d() {

		}
		segment2d(const point2d& st, const point2d& en) : st_(st), en_(en) {

		}

		// 斜率
		double k() const {
			return (en_.y_ - st_.y_) / (en_.x_ - st_.x_);
		}

		/// <summary>
		/// 根据x计算y. 如果垂直, 返回低y
		/// </summary>
		double calc_y(double x) const {
			return (x - st_.x_) / (en_.x_ - st_.x_) * (en_.y_ - st_.y_) + st_.y_;
		}
		/// <summary>
		/// 根据y计算x. 如果水平, 返回左x
		/// </summary>
		double calc_x(double y) const {
			return (y - st_.y_) / (en_.y_ - st_.y_) * (en_.x_ - st_.x_) + st_.x_;
		}

		bool is_point(double tol) const {
			return std::fabs(st_.x_ - en_.x_) < tol && std::fabs(st_.y_ - en_.y_) < tol;
		}
		bool is_hori(double tol) const {
			return std::fabs(st_.y_ - en_.y_) < tol;
		}
		bool is_vert(double tol) const {
			return std::fabs(st_.x_ - en_.x_) < tol;
		}

		// return.second == true表示在线段内(包括端点), 否则表示在线段外或平行无交点
		// ignore_end==true表示忽略交点是端点的情况
		std::pair<point2d, bool> intersect(const segment2d& other, double tol, bool ignore_end = false) const;

		point2d st_;
		point2d en_;
	};

}