#include <geo/segment2d.h>



namespace geo {
	std::pair<point2d, bool> segment2d::intersect(const segment2d& other, double tol, bool ignore_end) const {

		std::pair<point2d, bool> ans = {};

		auto d0 = en_ - st_, d1 = other.en_ - other.st_;
		if (!d0.is_parallel(d1, tol)) {
			auto v = other.st_ - st_;
			double cross = d0.cross(d1);
			double s = (v.x_ * d1.y_ - v.y_ * d1.x_) / cross;

			ans.first = st_ + s * d0;
			ans.second = (s > 0 && s < 1) ||
				(!ignore_end && ((ans.first - st_).is_zero(tol) || (ans.first - en_).is_zero(tol)));

			if (ans.second == true) {
				if (!ignore_end && ((ans.first - other.st_).is_zero(tol) || (ans.first - other.en_).is_zero(tol)))
					ans.second = true;
				else {
					ans.second = 
						(other.is_vert(tol) || (ans.first.x_ > other.st_.x_ != ans.first.x_ > other.en_.x_)) &&
						(other.is_hori(tol) || (ans.first.y_ > other.st_.y_ != ans.first.y_ > other.en_.y_));
				}
			}
		}

		return ans;
	}
}