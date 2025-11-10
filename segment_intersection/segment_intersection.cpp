#include <segment_intersection/segment_intersection.h>

#include <queue>
#include <set>
#include <cmath>
#include <cassert>
#include <algorithm>


// segment_intersection_T
namespace wzj {

	const double segment_intersection_T::max_neg_1_k_ = 1e9;

	double segment_intersection_T::get_neg_1_k(const geo::segment2d& seg) const {
		return seg.is_vert(tol_) ? 0 :
			(seg.is_hori(tol_) ? max_neg_1_k_ : std::min(-1 / seg.k(), max_neg_1_k_));
	}

	void segment_intersection_T::insert(const geo::segment2d& seg) {
		auto wrapper = _seg_wrapper(&seg);
		wrapper.neg_1_k_ = get_neg_1_k(seg);

		// 如果比-max_neg_1_k_小, 意味着这个线段在tol_下不是水平的, 但斜梁又趋于0+.
		// 这种情况, 认为改线段是畸形的
		assert(wrapper.neg_1_k_ > -max_neg_1_k_);

		inner_.insert(wrapper);
	}

	void segment_intersection_T::erase(const geo::segment2d& seg) {
		auto wrapper = _seg_wrapper(&seg);
		wrapper.neg_1_k_ = get_neg_1_k(seg);

		// 如果比-max_neg_1_k_小, 意味着这个线段在tol_下不是水平的, 但斜梁又趋于0+.
		// 这种情况, 认为改线段是畸形的
		assert(wrapper.neg_1_k_ > -max_neg_1_k_);

#ifdef _DEBUG
		auto to_del = inner_.find(wrapper);
		assert( to_del->seg_ == &seg);
#endif
		inner_.erase(wrapper);
	}

	segment_intersection_T::_Const_Iter segment_intersection_T::lower_bound(const geo::point2d& pt) const {
		auto tmp_seg = geo::segment2d{ pt, {pt.x_, pt.y_ - 1} }; // 保证经过pt, 且不是is_hori
		auto wrapper = _seg_wrapper(&tmp_seg); 
		wrapper.neg_1_k_ = -max_neg_1_k_;

		return inner_.lower_bound(wrapper);
	}

	segment_intersection_T::_Const_Iter segment_intersection_T::upper_bound(const geo::point2d& pt) const {
		auto tmp_seg = geo::segment2d{ pt, {pt.x_, pt.y_ - 1} };  // 保证经过pt, 且不是is_hori
		auto wrapper = _seg_wrapper(&tmp_seg);
		wrapper.neg_1_k_ = max_neg_1_k_ + 1;

		return inner_.upper_bound(wrapper);
	}
}


// segment_intersection
namespace wzj {

	void segment_intersection::work(const std::vector<geo::segment2d>& segs, bool keep_end) {
		keep_ends_ = keep_end;
		_init(segs);
		// 将所有线段的（上、下）端点插入 Q 中
		for (auto& seg : segs_) {
			_push_Q(seg, true);
			_push_Q(seg, false);
		}

		int epoch = 0;
		while (Q_.size() > 0) {
#ifdef _DEBUG
			max_Q_size_ = std::max(Q_.size(), max_Q_size_);
#endif

			++epoch;
			auto& e = *Q_.begin();

			_handle_events(e.first, e.second);
			Q_.erase(Q_.begin());
		}
	}

	const std::vector<geo::point2d>& segment_intersection::result() const {
#ifdef _DEBUG
		// 检测intersect_pts_no_repeat_确实无重复
		auto ss = std::set<geo::point2d, segment_intersection_point_compare>(intersect_pts_no_repeat_.begin(), intersect_pts_no_repeat_.end());
		assert(ss.size() == intersect_pts_no_repeat_.size());
#endif

		return intersect_pts_no_repeat_;
	}

	void segment_intersection::_init(const std::vector<geo::segment2d>& segs) {
		// reset
		Q_.clear();
		T_.clear();
		segs_.clear();
		intersect_pts_no_repeat_.clear();
		//
		Q_ = decltype(Q_){segment_intersection_point_compare{tol_}};
		T_.set_tol(tol_);
		// 1. 去除点
		// 2. 使seg.st是y高的一侧. 对于水平seg,则st=左侧.
		for (auto& seg : segs) {
			if (seg.is_point(tol_)) continue;

			if (seg.is_hori(tol_)) {
				if (seg.st_.x_ > seg.en_.x_)
					segs_.emplace_back(seg.en_, seg.st_);
				else segs_.emplace_back(seg.st_, seg.en_);
			}
			else if (seg.st_.y_ < seg.en_.y_)
				segs_.emplace_back(seg.en_, seg.st_);
			else 
				segs_.emplace_back(seg.st_, seg.en_);
		}
	}

	void segment_intersection::_push_Q(const geo::segment2d& seg, bool is_start) {
		auto& pt = is_start ? seg.st_ : seg.en_;
		auto it = Q_.find(pt);
		if (it == Q_.end()) {
			it = Q_.emplace(pt, std::vector<segment_intersection_event>()).first;
		}
		it->second.push_back({ seg, is_start });
	}
	void segment_intersection::_push_Q(const geo::point2d& pt) {
		auto it = Q_.lower_bound(pt);
		if (it == Q_.end() || !(it->first - pt).is_zero(tol_)) {
			// 记录交点. 这里的交点一定不存在于Q中, 即不同于已有交点和端点
			intersect_pts_no_repeat_.push_back(pt); 
			Q_.emplace_hint(it, pt, std::vector<segment_intersection_event>());
		}

		// 不需要把交点事件放入Q[pt]的队列中, 因为交点事件仅用作交点的记录,
	}


	void segment_intersection::_handle_events(const geo::point2d& pt, const std::vector<segment_intersection_event>& events) {
		std::vector<segment_intersection_event> U, L; // up, low
		for (auto& e : events) {
			if (e.e_ == segment_intersection_event_type::Start)
				U.push_back(e);
			/*else if (e.e_ == segment_intersection_event_type::Intersection)
				intersect_pts_.insert(pt);*/
			else L.push_back(e);
		}

		// U(p)∪C(p)可能是全部平行的线, 即U和C部分重叠, 此时不够成交点. 
		// 所以需要判断U和C存在没有平行的线段. has_diff==true表示找到了.
		// 仅需要|U|==1判断即可. 因为如果记录端点, 则|U|>1时, 它们端点相交.
		// L(p)∪C(p)同理
		bool u_has_diff = false, l_has_diff = false;
		double u_neg_1_k = U.size() != 1 ? 0 : T_.get_neg_1_k(*U[0].seg_);
		double l_neg_1_k = L.size() != 1 ? 0 : T_.get_neg_1_k(*L[0].seg_);

		int cnt = 0;
		auto old_pt = T_.event_pt();
		T_.set_event_pt(pt);
		auto C_it1 = T_.lower_bound(pt), C_it2 = T_.upper_bound(pt);
		std::vector<const geo::segment2d*> C; // contains
		for (auto it = C_it1; it != C_it2; ++it) {
			++cnt;
			if (!(it->seg_->en_ - pt).is_zero(tol_)) {
				// 忽略it->seg_->en_ == pt, 因为它们已经在L中
				C.push_back(it->seg_);
				// 检查[C_it1,C_it2)中, 是否存在和U[0]不同斜梁的线段, 如果存在, 则可以计入交点.
				// 否则表示U[0]和[C_it1,C_it2)中的线段是平行的, 即部分重叠, 不计入交点
				if (keep_ends_ && !u_has_diff && U.size() == 1 && std::fabs(it->neg_1_k_ - u_neg_1_k) > tol_)
					u_has_diff = true;
				if (keep_ends_ && !l_has_diff && L.size() == 1 && std::fabs(it->neg_1_k_ - l_neg_1_k) > tol_)
					l_has_diff = true;
			}
		}
		assert(cnt == C.size() + L.size());

		if (keep_ends_) {
			// 分别对于端点连接, 连接中存在start, 连接中存在end
			// _push_Q中记录了所有不同于端点的交点, 这里记录作为端点的交点.
			// 因为pt本身不会重复, 所以这里记录也不会重复
			if (U.size() + L.size() > 1 || u_has_diff || l_has_diff) 
				intersect_pts_no_repeat_.push_back(pt);	
		}
		// 因为不影响和T中pt无关的左右俩区间, 所以可以提前计算sl, sr
		auto left_bound = C_it1;
		auto sl = left_bound == T_.begin() ? nullptr : (--left_bound)->seg_;
		auto sr = C_it2 == T_.end() ? nullptr : C_it2->seg_;

		// 将 L(p)∪C(p)中的线段从 T 中删除
		T_.erase(C_it1, C_it2); 
		// 将U(p)∪C(p)中的线段插入到T中
		for (auto& seg : U) T_.insert(*seg.seg_);
		for (auto& seg : C) T_.insert(*seg);

		// 尝试更新交点事件
 		if (U.size() == 0 && C.size() == 0) {
			// 没有新的线段加入, 在 T 中，找出 p 的左右邻居 sl 和 sr
			if (sl != nullptr && sr != nullptr)
				_find_new_event(sl, sr, pt);
		}
		else {
			// 在 T 中，找出 U(p)∪C(p)里最左边的线段 s'=(++left_bound)->seg_
			// 在 T 中，找出与 s'紧邻于左侧的线段 sl
			if (sl != nullptr)
				_find_new_event(sl, (++left_bound)->seg_, pt);
			// 在 T 中，找出 U(p)∪C(p)里最右边的线段s''=(--C_it2)->seg_
			// 在 T 中，找出与 s''紧邻于右侧的线段 sr
			if (sr != nullptr)
				_find_new_event((--C_it2)->seg_, sr, pt);
		}
	}

	void segment_intersection::_find_new_event(const geo::segment2d* sl, const geo::segment2d* sr, const geo::point2d& pt) {
		auto ipt = sl->intersect(*sr, tol_, !keep_ends_);
		if (ipt.second == false || // 无交点
			(ipt.first - pt).is_zero(tol_) || // 和pt是同一个点
			ipt.first.y_ > pt.y_  // 在扫面线上方
			)
			return;

		if (ipt.first.y_ < pt.y_ || // 交点位于当前扫描线的下方
			ipt.first.x_ > pt.x_) // 交点正好落在当前扫描线上并且在当前事件点的右侧
		{
			if (ipt.first.y_ >= pt.y_ && ipt.first.x_ > pt.x_)
				assert(ipt.first.x_ > pt.x_ + tol_);  // 交点一定比扫描线的点更大, 否则sl,sr与逻辑错误

			_push_Q(ipt.first);
		}
	}
}