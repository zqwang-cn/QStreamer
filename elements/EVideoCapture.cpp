#include "EVideoCapture.h"

QELEMENT_REGISTER(EVideoCapture)

void EVideoCapture::init(const QMap<std::any>& properties, const QMap<QInPad*>& in_pads, const QMap<QOutPad*>& out_pads)
{
    auto uri = std::any_cast<std::string>(properties["uri"]);
    cap.open(uri);
    std::map<std::string, int> input_meta;
    int width = int(cap.get(CV_CAP_PROP_FRAME_WIDTH));
    input_meta["width"] = width;
    input_meta["height"] = int(cap.get(CV_CAP_PROP_FRAME_HEIGHT));
    input_meta["fps"] = int(cap.get(CV_CAP_PROP_FPS));
    input_meta["stride"] = width * 3;
    QBuffer buffer;
    buffer["input_meta"] = input_meta;
    out_pads["out"]->send_buffer(std::move(buffer));
}

void EVideoCapture::process(const QMap<QInPad*>& in_pads, const QMap<QOutPad*>& out_pads)
{
    cv::Mat image;
    if (!cap.read(image))
    {
        quit();
        return;
    }
    QBuffer buffer;
    buffer["image"] = image;
    out_pads["out"]->send_buffer(std::move(buffer));
}

void EVideoCapture::finalize()
{
    cap.release();
}