#include "Pipeline.h"
#include "utils.h"
#include <signal.h>

Pipeline* p;

void quit(int s)
{
    p->stop();
    p->finalize();
}

int main(int argc, char* argv[])
{
    assert(argc == 2);

    signal(SIGINT, quit);

    auto config = load_config_file(argv[1]);
    p = new Pipeline(config);
    p->init();
    p->run();
}