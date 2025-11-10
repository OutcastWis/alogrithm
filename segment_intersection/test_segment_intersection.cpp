#include <segment_intersection/test_segment_intersection.h>

#include <cassert>
#include <cstdlib>
#include <iostream>
#include <algorithm>
#include <cmath>
#include <chrono>

#include <segment_intersection/segment_intersection.h>


namespace wzj {

	void test_segment_intersection_T() {
		double tol = 1e-6;
		segment_intersection_T T;
		T.set_tol(tol);
		T.set_event_pt({2,0});

		std::vector<geo::segment2d> segs = {
			{{0,5},{0,-5}},
			{{2,5},{3,0}},
			{{3,5},{3,0}},
			{{4,5},{3,0}},
			{{6,5},{6,-5}},
			{{2,0},{7,0}},
		};

		for (auto& s : segs)
			T.insert(s);

		std::vector<geo::segment2d> ans = {
			{{0,5},{0,-5}},

			{{2,0},{7,0}},
			{{4,5},{3,0}},
			{{3,5},{3,0}},
			{{2,5},{3,0}},

			{{6,5},{6,-5}},
		};
		// test insert
		auto ans_it = ans.begin();
		for (auto it = T.begin(); it != T.end(); ++it, ++ans_it) {
			assert((it->seg_->st_ - ans_it->st_).is_zero(tol));
			assert((it->seg_->en_ - ans_it->en_).is_zero(tol));
		}

		// test lower_bound
		auto lb = T.lower_bound({0,0});
		assert((lb->seg_->st_ - geo::point2d(0, 5)).is_zero(tol));
		lb = T.lower_bound({ 3,0 });
		assert((lb->seg_->st_ - geo::point2d(4, 5)).is_zero(tol));
		lb = T.lower_bound({6,0});
		assert((lb->seg_->st_ - geo::point2d(6, 5)).is_zero(tol));
		// test upper_bound
		auto ub = T.upper_bound({ 0,0 });
		assert((ub->seg_->st_ - geo::point2d(2, 0)).is_zero(tol));
		ub = T.upper_bound({3,0});
		assert((ub->seg_->st_ - geo::point2d(6, 5)).is_zero(tol));
		ub = T.upper_bound({ 6,0 });
		assert(ub == T.end());

		// test erase
		for (auto& s : segs) {
			T.erase(s);
			for (auto it = T.begin(); it != T.end(); ++it) {
				assert(!(it->seg_->st_ - s.st_).is_zero(tol) || !(it->seg_->en_ - s.en_).is_zero(tol));
			}
		}
	}

	void test_segment_intersection() {
		double tol = 1e-6;
		segment_intersection si;
		// #型
		std::vector<geo::segment2d> segs = {
			{{-10,0},{20,0}},
			{{-10,10},{20,10}},
			{{0,-10},{0, 20}},
			{{10,-10},{10, 20}},
		};
		si.work(segs);
		auto ipts = si.result();
		assert(ipts.size() == 4);
		assert(std::find(ipts.begin(), ipts.end(), geo::point2d{ 0,0 }) != ipts.end());
		assert(std::find(ipts.begin(), ipts.end(), geo::point2d{ 0,10 }) != ipts.end());
		assert(std::find(ipts.begin(), ipts.end(), geo::point2d{ 10,10 }) != ipts.end());
		assert(std::find(ipts.begin(), ipts.end(), geo::point2d{ 10,0 }) != ipts.end());
		// 交于一点
		segs = {
			// 交于(0,0)
			{{-50,-50},{60,60}},
			{{-50,50},{50,-50}},
			{{-100,0},{50,0}},
			// 交于(100,0)
			{{60,-80}, {100,0}},
			{{100,-80},{100,80}},
		};
 		si.work(segs);
		ipts = si.result();
		assert(ipts.size() == 2);
		assert(std::find(ipts.begin(), ipts.end(), geo::point2d{ 0,0 }) != ipts.end());
		assert(std::find(ipts.begin(), ipts.end(), geo::point2d{ 100,0 }) != ipts.end());
		// 重合, 且交于端点
		segs = {
			// 首尾相连的矩形
			{{0,0},{10,0}},
			{{10,0},{10,10}},
			{{10,10},{0,10}},
			{{0,10},{0,0}},
			// 增加重合的线段
			{{0,0}, {20,0}},
			{{-6,-1},{1,6}},
			{{-1,4},{6,11}}
		};
		si.work(segs);
		ipts = si.result();
		assert(ipts.size() == 6);
		assert(std::find(ipts.begin(), ipts.end(), geo::point2d{ 0,0 }) != ipts.end());
		assert(std::find(ipts.begin(), ipts.end(), geo::point2d{ 0,10 }) != ipts.end());
		assert(std::find(ipts.begin(), ipts.end(), geo::point2d{ 10,10 }) != ipts.end());
		assert(std::find(ipts.begin(), ipts.end(), geo::point2d{ 10,0 }) != ipts.end());
		assert(std::find(ipts.begin(), ipts.end(), geo::point2d{ 0,5 }) != ipts.end());
		assert(std::find(ipts.begin(), ipts.end(), geo::point2d{ 5,10 }) != ipts.end());
		// 忽略端点
		si.work(segs, false);
		ipts = si.result();
		assert(ipts.size() == 2);
		assert(std::find(ipts.begin(), ipts.end(), geo::point2d{ 0,5 }) != ipts.end());
		assert(std::find(ipts.begin(), ipts.end(), geo::point2d{ 5,10 }) != ipts.end());
		// 随机相交
		int nums = 3000;
		segs.clear();
		auto rand_in_range = []() {
			return (std::rand() % 1000) / 1000.0 * 1000 - 1000;
		};
		for (int i = 0; i < nums; ++i) {
			geo::point2d st = { rand_in_range(), rand_in_range() };
			geo::point2d en = { rand_in_range(), rand_in_range() };
			while ((st - en).is_zero(tol)) {
				st = { rand_in_range(), rand_in_range() };
				en = { rand_in_range(), rand_in_range() };
			}
			segs.emplace_back(st, en); // 在[-1000, 1000]x[-1000,1000]之间的线段
		}

		std::map<geo::point2d, std::set<geo::segment2d*>, wzj::segment_intersection_point_compare> check;
		check = decltype(check)(wzj::segment_intersection_point_compare {tol});
		{
			auto time_st = std::chrono::high_resolution_clock::now();
			for (int i = 0; i < segs.size(); ++i) {
				for (int j = i + 1; j < segs.size(); ++j) {
					auto ipt = segs[i].intersect(segs[j], tol);
					if (ipt.second) { // 记录参与交点的所有线段
						check[ipt.first].insert(&segs[i]);
						check[ipt.first].insert(&segs[j]);
					}
				}
			}
			auto time_en = std::chrono::high_resolution_clock::now();
			std::cout << "Normal Work with " << nums << " segments, time cost: " <<
				std::chrono::duration_cast<std::chrono::milliseconds>(time_en - time_st).count()
				<< " ms" << std::endl;
		}

		{
			auto time_st = std::chrono::high_resolution_clock::now();
			si.work(segs);
			auto time_en = std::chrono::high_resolution_clock::now();
			std::cout << "Work with " << nums << " segments, time cost: " <<
				std::chrono::duration_cast<std::chrono::milliseconds>(time_en - time_st).count()
				<< " ms" << std::endl;
		}
		ipts = si.result();
		assert(ipts.size() == check.size());
	}
} 