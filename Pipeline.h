#include "Element.h"
#include <queue>

class Pipeline
{
public:
    Pipeline(Json::Value config);
    void init();
    void run();
    void element_ready(Element* element);

private:
    std::map<std::string, Element*> _elements;
    std::queue<Element*> _ready_elements;
    Element* find_element(std::string name);
    void link(Json::Value config);
};