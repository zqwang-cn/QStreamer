#include "EDeepsort.h"
#include "EStructures.h"
#include "../deepsort/DSMetrics.h"

QELEMENT_REGISTER(EDeepsort)

void EDeepsort::init(const QMap<std::any>& properties, const QMap<QInPad*>& in_pads, const QMap<QOutPad*>& out_pads)
{
    auto n_init = std::any_cast<int>(properties["n_init"]);
    auto max_age = std::any_cast<int>(properties["max_age"]);
    auto feature_metric = std::any_cast<std::string>(properties["feature_metric"]);
    auto feature_th = std::any_cast<float>(properties["feature_th"]);
    auto iou_metric = std::any_cast<std::string>(properties["iou_metric"]);
    auto iou_th = std::any_cast<float>(properties["iou_th"]);
    tracker = new DSTracker(n_init, max_age, ds_get_metric(feature_metric), feature_th, ds_get_metric(iou_metric), iou_th);
    out_pads["out"]->send_buffer(in_pads["in"]->get_buffer());
}

void EDeepsort::process(const QMap<QInPad*>& in_pads, const QMap<QOutPad*>& out_pads)
{
    auto buffer = in_pads["in"]->get_buffer();
    auto objects = std::any_cast<std::list<EObjectInfo>>(buffer["objects"]);

    std::list<DSDetection> detections;
    for (auto& obj : objects)
        detections.push_back(ds_create_detection(obj.bbox, obj.feature));
    tracker->predict();
    tracker->update(detections);

    auto obj_iter = objects.begin();
    auto det_iter = detections.begin();
    for (; obj_iter != objects.end(); obj_iter++, det_iter++)
        obj_iter->id = (*det_iter)->id;

    buffer["objects"] = objects;
    out_pads["out"]->send_buffer(std::move(buffer));
}

void EDeepsort::finalize()
{
    delete tracker;
}