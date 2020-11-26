// Modified based on https://github.com/mcximing/sort-cpp

#include "DSTrack.h"

_DSTrack::_DSTrack(DSDetection det, int id, int n_init, int max_age) : id(id), n_init(n_init), max_age(max_age)
{
    feature = det->feature;
    cv::Rect2f& rect = det->bbox;

    int stateNum = 7;
    int measureNum = 4;
    kf = cv::KalmanFilter(stateNum, measureNum, 0);

    kf.transitionMatrix = (cv::Mat_<float>(stateNum, stateNum) << 1, 0, 0, 0, 1, 0, 0,
                           0, 1, 0, 0, 0, 1, 0,
                           0, 0, 1, 0, 0, 0, 1,
                           0, 0, 0, 1, 0, 0, 0,
                           0, 0, 0, 0, 1, 0, 0,
                           0, 0, 0, 0, 0, 1, 0,
                           0, 0, 0, 0, 0, 0, 1);

    setIdentity(kf.measurementMatrix);
    setIdentity(kf.processNoiseCov, cv::Scalar::all(1e-2));
    setIdentity(kf.measurementNoiseCov, cv::Scalar::all(1e-1));
    setIdentity(kf.errorCovPost, cv::Scalar::all(1));

    // initialize state vector with bounding box in [cx,cy,s,r] style
    kf.statePost.at<float>(0, 0) = rect.x + rect.width / 2;
    kf.statePost.at<float>(1, 0) = rect.y + rect.height / 2;
    kf.statePost.at<float>(2, 0) = rect.area();
    kf.statePost.at<float>(3, 0) = rect.width / rect.height;
}

// Predict the estimated bounding box.
void _DSTrack::predict()
{
    kf.predict();
    age++;
    time_since_update++;
}

// Update the state vector with observed bounding box.
void _DSTrack::update(DSDetection detection)
{
    feature = std::move(detection->feature);
    auto& rect = detection->bbox;

    // measurement
    cv::Mat measurement(4, 1, CV_32F);
    measurement.at<float>(0, 0) = rect.x + rect.width / 2;
    measurement.at<float>(1, 0) = rect.y + rect.height / 2;
    measurement.at<float>(2, 0) = rect.area();
    measurement.at<float>(3, 0) = rect.width / rect.height;
    // update
    kf.correct(measurement);

    hits++;
    time_since_update = 0;
    if (status == DSTrackStatus::Tentative && hits > n_init)
        status = DSTrackStatus::Confirmed;
}

void _DSTrack::miss()
{
    if (status == DSTrackStatus::Tentative || time_since_update >= max_age)
        status = DSTrackStatus::Deleted;
}

cv::Rect2f _DSTrack::get_state()
{
    auto state = kf.statePost;

    float& cx = state.at<float>(0, 0);
    float& cy = state.at<float>(1, 0);
    float& s = state.at<float>(2, 0);
    float& r = state.at<float>(3, 0);

    float w = sqrt(s * r);
    float h = s / w;
    float x = (cx - w / 2);
    float y = (cy - h / 2);

    if (x < 0 && cx > 0)
        x = 0;
    if (y < 0 && cy > 0)
        y = 0;

    return cv::Rect2f(x, y, w, h);
}

DSTrack ds_create_track(DSDetection det, int id, int n_init, int max_age)
{
    return DSTrack(new _DSTrack(det, id, n_init, max_age));
}