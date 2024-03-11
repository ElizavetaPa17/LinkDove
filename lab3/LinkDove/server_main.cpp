#include <memory>
#include <thread>
#include "linkdoveserver.h"

int main(int argc, char *argv[]) {
    std::shared_ptr<LinkDoveServer> server_ptr = std::make_shared<LinkDoveServer>();
    server_ptr->listen(8000);

    while(true) {
        std::this_thread::yield();
    }
}
