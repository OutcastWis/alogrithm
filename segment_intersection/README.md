# segment_intersection
## BRIEF
bentley-ottmann algorithm

## REQUIRE
You only need c++11

## USE
```c++
std::vector<geo::segment2d> segs = {
    {{-10,0},{20,0}},
    {{-10,10},{20,10}},
    /*... other segments*/
}; // all line segments
wzj::segment_intersection si;
si.work(segs);
auto& result = si.result();
```
