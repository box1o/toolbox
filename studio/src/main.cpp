#include <ct/base/base.hpp>
#include <string>


ct::result<std::string> test(){
    if (true){
        return "Hello World";
    } else {
        return ct::err(ct::ErrorCode::UNKNOWN_ERROR, "An unknown error occurred");
    }
}

using namespace ct;

int main(int argc, char* argv[]) {
    log::Configure("studio", ct::log::Level::Trace);

    for (int i = 0; i < argc; ++i) {
        log::Info("Argument {} : {}\n", i, argv[i]);
    }

    log::Error("This is an error message with code {}", 404);
    log::Critical("Critical failure: {}", "Out of memory");


    if (auto result = test(); result.has_value()) {
        log::Info("Test succeeded with message: {}", result.value());
    }


    return 0;
}
