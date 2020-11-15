#include "Element.h"
#include <queue>

class Pipeline
{
public:
    Pipeline(Json::Value config);
    ~Pipeline();
    void init();
    void run();
    void stop();
    void finalize();
    friend class Element;

private:
    bool _quit = false;
    std::map<std::string, Element*> _elements;
    std::queue<Element*> _input_elements;
    std::queue<Element*> _ready_elements;
    void element_ready(Element* element);
};