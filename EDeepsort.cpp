#include "EDeepsort.h"
#include "EStructures.h"
#include "deepsort/DSMetrics.h"

QELEMENT_REGISTER(EDeepsort)

void EDeepsort::init(const std::map<std::string, std::any>& properties, const std::map<std::string, QInPad*>& in_pads, const std::map<std::string, QOutPad*>& out_pads)
{
    auto n_init = std::any_cast<int>(properties.at("n_init"));
    auto max_age = std::any_cast<int>(properties.at("max_age"));
    auto feature_metric = std::any_cast<std::string>(properties.at("feature_metric"));
    auto feature_th = std::any_cast<float>(properties.at("feature_th"));
    auto iou_metric = std::any_cast<std::string>(properties.at("iou_metric"));
    auto iou_th = std::any_cast<float>(properties.at("iou_th"));
    tracker = new DSTracker(n_init, max_age, ds_get_metric(feature_metric), feature_th, ds_get_metric(iou_metric), iou_th);
    out_pads.at("out")->send_buffer(in_pads.at("in")->get_buffer());
}

void EDeepsort::process(const std::map<std::string, QInPad*>& in_pads, const std::map<std::string, QOutPad*>& out_pads)
{
    auto buffer = in_pads.at("in")->get_buffer();
    auto objects = std::any_cast<std::list<EObjectInfo>>(buffer.get_buffer("objects"));

    std::list<DSDetection> detections;
    for (auto& obj : objects)
        detections.push_back(ds_create_detection(obj.bbox, obj.feature));
    tracker->predict();
    tracker->update(detections);

    auto obj_iter = objects.begin();
    auto det_iter = detections.begin();
    for (; obj_iter != objects.end(); obj_iter++, det_iter++)
        obj_iter->id = (*det_iter)->id;

    buffer.set_buffer("objects", objects);
    out_pads.at("out")->send_buffer(std::move(buffer));
}

void EDeepsort::finalize()
{
    delete tracker;
}