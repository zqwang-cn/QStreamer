#include "qstreamer/QPipeline.h"
#include "utils/utils.h"
#include <signal.h>

QPipeline* p;

void quit(int s)
{
    p->stop();
}

int main(int argc, char* argv[])
{
    assert(argc == 2);

    signal(SIGINT, quit);

    auto config = load_config_file(argv[1]);
    p = new QPipeline(config);
    p->init();
    p->run();
    p->finalize();
    delete p;
}