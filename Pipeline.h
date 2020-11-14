#include "Element.h"
#include <queue>

class Pipeline
{
public:
    Pipeline(Json::Value config);
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
    Element* get_element(std::string name);
};