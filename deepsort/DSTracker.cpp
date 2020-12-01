#include "DSTracker.h"
#include "Hungarian.h"

DSTracker::DSTracker(int n_init, int max_age,
                     std::function<double(const DSTrack&, const DSDetection&)> feature_metric, double feature_th,
                     std::function<double(const DSTrack&, const DSDetection&)> iou_metric, double iou_th)
    : _n_init(n_init), _max_age(max_age), _feature_metric(feature_metric), _feature_th(feature_th), _iou_metric(iou_metric), _iou_th(iou_th)
{
}

std::list<std::pair<DSTrack, DSDetection>> match_by_metric(std::list<DSTrack>& tracks, std::list<DSDetection>& detections, std::function<double(const DSTrack&, const DSDetection&)>& metric, double th)
{
    // create dist matrix
    std::vector<std::vector<double>> dists;
    dists.reserve(tracks.size());
    for (auto& track : tracks)
    {
        std::vector<double> dist_vec;
        dist_vec.reserve(detections.size());
        for (auto& detection : detections)
            dist_vec.push_back(metric(track, detection));
        dists.push_back(std::move(dist_vec));
    }

    // perform Hungarian algorithm
    std::vector<int> matches;
    HungarianAlgorithm::Solve(dists, matches);

    // detection index to iterator map
    std::vector<std::list<DSDetection>::iterator> detection_iters;
    detection_iters.reserve(detections.size());
    for (auto iter = detections.begin(); iter != detections.end(); iter++)
        detection_iters.push_back(iter);

    // parse results
    std::list<std::pair<DSTrack, DSDetection>> matched_pairs;
    int i = 0;
    for (auto track_iter = tracks.begin(); track_iter != tracks.end(); track_iter++)
    {
        int j = matches[i];
        if (j != -1 && dists[i][j] < th)
            matched_pairs.emplace_back(std::move(*track_iter), std::move(*detection_iters[j]));
        i++;
    }

    // remove matched tracks
    tracks.remove_if([](DSTrack& track) {
        return track == nullptr ? true : false;
    });
    // remove matched detections
    detections.remove_if([](DSDetection& detection) {
        return detection == nullptr ? true : false;
    });

    return matched_pairs;
}

std::list<std::pair<DSTrack, DSDetection>> match_iou(std::list<DSTrack>& tracks, std::list<DSDetection>& detections, std::function<double(const DSTrack&, const DSDetection&)>& metric, double th)
{
    return match_by_metric(tracks, detections, metric, th);
}

std::list<std::pair<DSTrack, DSDetection>> match_feature(std::list<DSTrack>& tracks, std::list<DSDetection>& detections, int max_age, std::function<double(const DSTrack&, const DSDetection&)>& metric, double th)
{
    std::list<std::pair<DSTrack, DSDetection>> matches;
    std::list<DSTrack> current_tracks;

    // match tracks cascadely
    for (int age = 1; age <= max_age; age++)
    {
        // return if no detections left
        if (detections.size() == 0)
            break;

        // select tracks with current age level and merge with remaining tracks of last level
        tracks.remove_if([&current_tracks, age](DSTrack& track) {
            if (track->time_since_update == age && track->status == DSTrackStatus::Confirmed)
            {
                current_tracks.push_back(std::move(track));
                return true;
            }
            return false;
        });

        // return if no tracks left, else continue
        if (current_tracks.size() == 0)
            if (tracks.size() == 0)
                break;
            else
                continue;

        // perform matching
        auto current_matches = match_by_metric(current_tracks, detections, metric, th);
        matches.splice(matches.end(), current_matches);
    }

    // move remaining tracks back to tracks
    tracks.splice(tracks.end(), current_tracks);
    return matches;
}

void DSTracker::predict()
{
    for (auto track : _tracks)
        track->predict();
}

void DSTracker::update(std::list<DSDetection> detections)
{
    auto tracks = _tracks;

    // match by feature and iou
    std::list<std::pair<DSTrack, DSDetection>> matches;
    if (_feature_metric != nullptr)
        matches.splice(matches.end(), match_feature(tracks, detections, _max_age, _feature_metric, _feature_th));
    matches.splice(matches.end(), match_iou(tracks, detections, _iou_metric, _iou_th));

    // update matched tracks
    for (auto& [track, detection] : matches)
    {
        track->update(detection);
        detection->id = track->id;
    }

    // mark missed tracks
    for (auto& track : tracks)
        track->miss();

    // remove lost tracks
    _tracks.remove_if([](DSTrack& track) {
        return track->status == DSTrackStatus::Deleted ? true : false;
    });

    // create new tracks for unmatched detections
    for (auto& detection : detections)
    {
        _tracks.push_back(ds_create_track(detection, _next_id++, _n_init, _max_age));
        detection->id = _tracks.back()->id;
    }
}

const std::list<DSTrack>& DSTracker::get_tracks() const
{
    return _tracks;
}