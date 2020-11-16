#include "MDetector.h"

class MDetCenterNet : public MDetector
{
public:
    MDetCenterNet(std::string config_file);

private:
    int hm_binding_index;
    int hmax_binding_index;
    int wh_binding_index;
    int reg_binding_index;
    int output_height;
    int output_width;
    virtual std::vector<DetectionResult> postprocess();
};