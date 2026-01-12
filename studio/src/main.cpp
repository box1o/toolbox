
#include <print>


int main(int argc, char* argv[]) {

    for (int i = 0; i < argc; ++i) {
        std::println("Argument {} : {}\n", i, argv[i]);
    }


    return 0;
}
