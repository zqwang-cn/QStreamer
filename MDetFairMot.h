#include "MDetector.h"

class MDetFairMot : public MDetector
{
public:
    MDetFairMot(std::string config_file);

private:
    int hm_binding_index;
    int wh_binding_index;
    int id_binding_index;
    int reg_binding_index;
    int output_height;
    int output_width;
    int feature_length;
    virtual std::list<DetectionResult> postprocess();
};