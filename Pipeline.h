#include "Element.h"
#include <queue>

class _Pipeline
{
public:
    void set(std::map<std::string, Element*>&& elements, std::vector<Element*>&& input_elements);
    void init();
    void run();
    void stop();
    void finalize();
    void element_ready(Element* element);

private:
    bool _quit = false;
    std::map<std::string, Element*> _elements;
    std::vector<Element*> _input_elements;
    std::queue<Element*> _ready_elements;
};

using Pipeline = std::shared_ptr<_Pipeline>;

Pipeline create_pipeline(Json::Value config);