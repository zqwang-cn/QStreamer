#pragma once
#include "DSTrack.h"
#include <functional>

std::function<double(const DSTrack&, const DSDetection&)> ds_get_metric(std::string name);