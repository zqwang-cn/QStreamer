#include "QElement.h"
#include <queue>

class QPipeline
{
public:
    QPipeline(Json::Value config);
    ~QPipeline();
    void init();
    void run();
    void stop();
    void finalize();
    friend class QElement;

private:
    bool _quit = false;
    std::map<std::string, QElement*> _elements;
    std::queue<QElement*> _input_elements;
    std::queue<QElement*> _ready_elements;
    void element_ready(QElement* element);
};