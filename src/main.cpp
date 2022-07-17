#include <dlib/cmd_line_parser.h>
#include <fmt/color.h>
#include <fmt/core.h>
#include <fmt/ranges.h>
#include <iostream>
#if USE_TBB
#include <execution>
#endif

auto main(const int argc, const char** argv) -> int
try
{
    dlib::command_line_parser parser;
    parser.set_group_name("Help Options");
    parser.add_option("h", "alias for --help");
    parser.add_option("help", "display this message and exit");
    parser.parse(argc, argv);

    if (parser.option("h") or parser.option("help"))
    {
        fmt::print(fmt::emphasis::bold | fmt::emphasis::underline, "C++ Project Template\n");
        fmt::print("Usage: {} <OPTIONS>\n", argv[0]);
        parser.print_options();
        return EXIT_SUCCESS;
    }

    // define a vector and sort it in parallel, if possible
    std::vector<int> nums{0, 2, 1, 3};
    std::sort(
#if USE_TBB
        std::execution::par_unseq,
#endif
        nums.begin(),
        nums.end());

    // print the sorted, comma-separated, vector
    fmt::print("{}", fmt::join(nums, ", "));
    return EXIT_SUCCESS;
}
catch (const std::exception& e)
{
    std::cerr << e.what() << '\n';
    return EXIT_FAILURE;
}
