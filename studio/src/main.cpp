#include "core/application.hpp"

int main(int /*argc*/, char* /*argv*/[]) {

    ct::Application app({ .name = "studio", });
    app.Run();

    return 0;
}
