#include "DSDetection.h"
#include "DSTrack.h"
#include <functional>
#include <list>

class DSTracker
{
public:
    DSTracker(int n_init, int max_age,
              std::function<double(const DSTrack&, const DSDetection&)> feature_metric, double feature_th,
              std::function<double(const DSTrack&, const DSDetection&)> iou_metric, double iou_th);
    void predict();
    void update(std::list<DSDetection> detections);
    const std::list<DSTrack>& get_tracks() const;

private:
    std::list<DSTrack> _tracks;

    uint _next_id = 0;
    int _n_init;
    int _max_age;

    std::function<double(const DSTrack&, const DSDetection&)> _iou_metric;
    std::function<double(const DSTrack&, const DSDetection&)> _feature_metric;
    double _iou_th;
    double _feature_th;
};