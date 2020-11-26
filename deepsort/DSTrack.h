// Modified based on https://github.com/mcximing/sort-cpp

#pragma once
#include <memory>
#include <opencv2/opencv.hpp>
#include "DSDetection.h"

enum class DSTrackStatus
{
    Tentative,
    Confirmed,
    Deleted
};

struct _DSTrack
{
    _DSTrack(DSDetection det, int id, int n_init, int max_age);
    void predict();
    void update(DSDetection detection);
    void miss();
    cv::Rect2f get_state();

    uint id;
    std::vector<float> feature;
    cv::KalmanFilter kf;

    DSTrackStatus status = DSTrackStatus::Tentative;
    int age = 1;
    int time_since_update = 0;
    int hits = 1;
    int n_init;
    int max_age;
};

using DSTrack = std::shared_ptr<_DSTrack>;

DSTrack ds_create_track(DSDetection det, int id, int n_init, int max_age);