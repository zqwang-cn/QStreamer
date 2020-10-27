#include "Pipeline.h"
#include "utils.h"

int main(int argc, char* argv[])
{
    assert(argc == 2);
    auto config = load_config_file(argv[1]);
    Pipeline p(config);
    p.init();
    p.run();
}