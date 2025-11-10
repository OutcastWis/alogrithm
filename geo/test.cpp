#include <cassert>

#include <geo/segment2d.h>

namespace geo {
	void test_segment() {
		double tol = 1e-6;
		// 测试求交

		// 相交于端点
		geo::segment2d s1{ {0,0},{0,10} };
		geo::segment2d s2{ {0,10},{10,10} };
		auto ipt = s1.intersect(s2, tol);
		assert(ipt.second);
		assert((ipt.first - geo::point2d(0, 10)).is_zero(tol));
		ipt = s1.intersect(s2, tol, true);
		assert(ipt.second == false);
		// 一端在另一个线上
		s1 = { {1,1},{10,10} };
		s2 = { {8,12},{12,8} };
		ipt = s1.intersect(s2, tol); 
		assert(ipt.second);
		assert((ipt.first - geo::point2d(10, 10)).is_zero(tol));
		ipt = s1.intersect(s2, tol, true);
		assert(ipt.second == false);
		// 平行
		ipt = s1.intersect(s1, tol);
		assert(ipt.second == false);
		ipt = s1.intersect({ { 1,2 }, {10,11} }, tol);
		assert(ipt.second == false);
		// 在线段外
		ipt = s1.intersect({ {9,12},{13,8} }, tol);
		assert(ipt.second == false);
	}
}

int main() {
	geo::test_segment();
}