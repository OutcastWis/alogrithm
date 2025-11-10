#pragma once

#include <vector>
#include <set>
#include <map>

#include <geo/segment2d.h>

namespace wzj {

	/// <summary>
	/// 事件类型
	/// </summary>
	enum segment_intersection_event_type {
		Start,
		Intersection,
		End,
	};

	/// <summary>
	/// 事件
	/// </summary>
	struct segment_intersection_event {

		segment_intersection_event(const geo::segment2d& seg, bool is_start) : seg_(&seg) {
			e_ = is_start ? segment_intersection_event_type::Start : segment_intersection_event_type::End;
		}
		segment_intersection_event() {
			e_ = segment_intersection_event_type::Intersection;
			seg_ = nullptr;
		}

		const geo::segment2d* seg_;
		segment_intersection_event_type e_;
	};

	
	/// <summary>
	/// Q的比较器
	/// </summary>
	struct segment_intersection_point_compare {
		bool operator()(const geo::point2d& a, const geo::point2d& b) const {
			if ((a - b).is_zero(tol_)) return false;
		
			if (a.y_ == b.y_) return a.x_ < b.x_;
			return a.y_ > b.y_;
		}
		double tol_;
	};

	/// <summary>
	/// 扫描线队列T
	/// </summary>
	class segment_intersection_T {
	public:
		// 最大负斜梁倒数, 1e9. 
		// 负斜梁倒数的作用, 见_seg_wrapper中neg_1_k_的注释
		static const double max_neg_1_k_; 
	private:
		struct _seg_wrapper {
			_seg_wrapper(const geo::segment2d* seg) : seg_(seg) {
			}
			double calc_x(double y) const {
				return (seg_->st_.y_ - y) * neg_1_k_ + seg_->st_.x_;
			}

			const geo::segment2d* seg_;
			// 负斜梁倒数, 即-1/k, k是斜梁. 对于竖直=0, 水平=max_neg_1_k_
			// 该值能够确保, 在扫面线上面交于o点的线, 当扫描线往o下移动时, 
			// 他们的顺序能按照neg_1_k_从小到大正确排序
			double neg_1_k_ = 0;
			bool is_hori_ = false;
		};

		struct _compare {
			bool operator()(const _seg_wrapper& a, const _seg_wrapper& b) const {
				// 水平线比较时, 使用当前的事件点. 
				double ax = a.neg_1_k_ == max_neg_1_k_ ? owner_->event_pt_.x_ : a.calc_x(owner_->event_pt_.y_);
				double bx = b.neg_1_k_ == max_neg_1_k_ ? owner_->event_pt_.x_ : b.calc_x(owner_->event_pt_.y_);
				if (std::fabs(ax - bx) > owner_->tol_) return ax < bx;
				// 再比较负斜梁倒数. 因为扫描线从上往下, 所以负斜梁倒数小的在前
				if (std::fabs(a.neg_1_k_ - b.neg_1_k_) > owner_->tol_) return a.neg_1_k_ < b.neg_1_k_;
				// 此时, 误差下认为重合, 随便顺序即可
				return a.seg_ < b.seg_;
			}

			segment_intersection_T* owner_;
		};

		using _Const_Iter = std::set<_seg_wrapper, _compare>::const_iterator;

	public:
		segment_intersection_T() {
			c_.owner_ = this;
			inner_ = std::set<_seg_wrapper, _compare>(c_);
		}

		double get_neg_1_k(const geo::segment2d& seg) const;


		void insert(const geo::segment2d& seg);
		void erase(const geo::segment2d& seg);
		void erase(_Const_Iter first, _Const_Iter last) {
			inner_.erase(first, last);
		}
		void clear() {
			inner_.clear();
		}
		_Const_Iter lower_bound(const geo::point2d& pt) const;
		_Const_Iter upper_bound(const geo::point2d& pt) const;
		void set_tol(double tol) {
			tol_ = tol;
		}
		void set_event_pt(const geo::point2d& pt) {
			event_pt_ = pt;
		}
		const geo::point2d& event_pt() const {
			return event_pt_;
		}


		_Const_Iter begin() const {
			return inner_.begin();
		}
		_Const_Iter end() const {
			return inner_.end();
		}

	private:

		_compare c_;
		std::set<_seg_wrapper, _compare> inner_;

		// 因为水平线在T内时, 和扫描线的交点一定可以看做是当前的事件节点.
		// 如果事件节点不在水平线上, 则水平线一定已经处理了End, 从T中删去
		geo::point2d event_pt_;
		double tol_ = 0;
	};

	/// <summary>
	/// 求2维线段交点. 重合交点认作一个
	/// 1. 线段可以重叠, 垂直, 水平, 但不能是点
	/// 2. 线段起点是y轴大的点. 如果水平线, 则是x小的点
	/// 3. 如果交点是一个线段的端点, 是否记录由keep_ends确定, 默认true
	/// 4. 存在重叠的线段之间, 不认为有交点, 除非它们端点相交. 即区间[0,5]与[1,2]无交点, 但[0,5]与[1,5]存在交点
	/// 5. 无法处理交点很密集的情况, 浮点数误差会导致结果不正确
	/// </summary>
	class segment_intersection {
	public:
		void work(const std::vector<geo::segment2d>& segs, bool keep_end = true);
		const std::vector<geo::point2d>& result() const;
#ifdef _DEBUG
		size_t max_Q_size_ = 0; // 用以查看Q的大小
#endif
	private:
		void _init(const std::vector<geo::segment2d>& segs);
		// 添加起点, 终点
		void _push_Q(const geo::segment2d& seg, bool is_start);
		// 添加交点
		void _push_Q(const geo::point2d& pt);

		void _handle_events(const geo::point2d& pt, const std::vector<segment_intersection_event>& events);
		void _find_new_event(const geo::segment2d* sl, const geo::segment2d* sr, const geo::point2d& pt);
	private:
		std::vector<geo::segment2d> segs_;
		double tol_ = 1e-6;
		bool keep_ends_ = true;

		std::map<geo::point2d, std::vector<segment_intersection_event>, 
			segment_intersection_point_compare> Q_;
		segment_intersection_T T_;

		std::vector<geo::point2d> intersect_pts_no_repeat_;
	};
}