#include <vector>
#include <utility>

namespace wzj {
// 笛卡尔树
// 1. 每个节点有(k,w)构成, k满足二叉树搜索树, w满足堆(这里用小根堆)
// 2. 如果k是数组下标, 那么每个子树都是以根的w为最小的连续区间, 适用于RMQ(范围最小值查询).
//    同时, 如果理论上这个区间内没有相同的w, 那么这个子树表示的区间一定是最大的
// 3. treap是它w随机化的特例
class cartesian {
 public:
 // O(n)建树. 对于treap每次添加1个新元素都可能进行多次旋转. 但这里利用下标的递增, 把旋转操作的结果1步完成
  void build(const std::vector<int>& data);
  void clear();

  // 返回以下标i为最小值的最大范围. 主要用于检验建树的正确性. i从0开始
  std::pair<int, int> range(int i) const;
 private:
  void _init_capacity(size_t s);

  int *lc_ = nullptr, *rc_ = nullptr, *w_ = nullptr;      // 左子树, 右子树, 权重. 下标从1开始
  int* stack_ = nullptr;  // stack_存放下标
  int top_ = 0;           // 栈顶. stack_中排在最后的是栈顶. top_==0是空栈
  int n_;

  int* same_ = nullptr; // same_[i]=j, 表示i和j的w一样. i>=j
};
}  // namespace wzj