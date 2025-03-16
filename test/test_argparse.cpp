#include <cstdint>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>
#include "argparse.h"
#include "unittest_framework.h"

using namespace std;
using namespace zul;

INIT_UNIT_TEST_APP(argparse_unit_test_app)

ADD_UNIT_TEST_CASE(argparse, test_cannot_add_help_by_yourself)
{
    auto cmd = Command::new_command("my_command")->usage("the usage xxx");

    CHECK_THOW(Command::new_command("my_command")->arg(Arg::new_arg(ArgType::OPTIONAL)->long_name("help")),
               ParseArgsError);

    CHECK_THOW(Command::new_command("my_command")->arg(Arg::new_arg(ArgType::OPTIONAL)->short_name('h')),
               ParseArgsError);
}

ADD_UNIT_TEST_CASE(argparse, test_required_long_name_arg)
{
    auto cmd = Command::new_command("my_command")->arg(Arg::new_arg(ArgType::REQUIRED)->long_name("reqarg"));

    CHECK_NO_THOW(cmd->parse_args({"my_command", "--reqarg", "1"}));

    CHECK_THOW(cmd->parse_args({"my_command", "--reqarg"}), ParseArgsError);

    CHECK_THOW(cmd->parse_args({"my_command"}), ParseArgsError);

    cmd->parse_args({"my_command", "--reqarg", "1"});
    CHECK_EQ(cmd->get_one_value<int>("reqarg"), 1);

    cmd->parse_args({"my_command", "--reqarg", "1"});
    CHECK_EQ(cmd->get_one_value<std::string_view>("reqarg"), "1");
}

ADD_UNIT_TEST_CASE(argparse, test_required_short_name_arg)
{
    auto cmd = Command::new_command("my_command")->arg(Arg::new_arg(ArgType::REQUIRED)->short_name('r'));

    CHECK_NO_THOW(cmd->parse_args({"my_command", "-r", "1"}));

    CHECK_THOW(cmd->parse_args({"my_command", "-r"}), ParseArgsError);

    CHECK_THOW(cmd->parse_args({"my_command"}), ParseArgsError);

    cmd->parse_args({"my_command", "-r", "1"});
    CHECK_EQ(cmd->get_one_value<int>('r'), 1);

    cmd->parse_args({"my_command", "-r", "1"});
    CHECK_EQ(cmd->get_one_value<std::string_view>('r'), "1");
}

ADD_UNIT_TEST_CASE(argparse, test_optional_long_name_arg)
{
    auto cmd = Command::new_command("my_command")->arg(Arg::new_arg(ArgType::OPTIONAL)->long_name("optarg"));

    CHECK_NO_THOW(cmd->parse_args({"my_command", "--optarg", "1"}));

    CHECK_THOW(cmd->parse_args({"my_command", "--optarg"}), ParseArgsError);

    CHECK_NO_THOW(cmd->parse_args({
        "my_command",
    }));

    cmd->parse_args({"my_command", "--optarg", "1"});
    CHECK_EQ(cmd->get_one_value<int>("optarg"), 1);

    cmd->parse_args({"my_command", "--optarg", "1"});
    CHECK_EQ(cmd->get_one_value<std::string_view>("optarg"), "1");
}

ADD_UNIT_TEST_CASE(argparse, test_optional_short_name_arg)
{
    auto cmd = Command::new_command("my_command")->arg(Arg::new_arg(ArgType::OPTIONAL)->short_name('o'));

    CHECK_NO_THOW(cmd->parse_args({
        "my_command",
        "-o",
        "1",
    }));

    CHECK_THOW(cmd->parse_args({"my_command", "-o"}), ParseArgsError);

    CHECK_NO_THOW(cmd->parse_args({"my_command"}));

    cmd->parse_args({"my_command", "-o", "1"});
    CHECK_EQ(cmd->get_one_value<int>('o'), 1);

    cmd->parse_args({"my_command", "-o", "1"});
    CHECK_EQ(cmd->get_one_value<std::string_view>('o'), "1");
}

ADD_UNIT_TEST_CASE(argparse, test_range_required_long_name_arg)
{
    auto cmd = Command::new_command("my_command")
                   ->arg(Arg::new_arg(ArgType::REQUIRED)->long_name("rangearg")->range(NumType::INT, "5", "10"));

    CHECK_NO_THOW(cmd->parse_args({"my_command", "--rangearg", "5"}));
    CHECK_NO_THOW(cmd->parse_args({"my_command", "--rangearg", "8"}));
    CHECK_NO_THOW(cmd->parse_args({"my_command", "--rangearg", "10"}));
    CHECK_THOW(cmd->parse_args({"my_command", "--rangearg", "4"}), ParseArgsError);
    CHECK_THOW(cmd->parse_args({"my_command", "--rangearg", "11"}), ParseArgsError);
}

ADD_UNIT_TEST_CASE(argparse, test_range_required_short_name_arg)
{
    auto cmd = Command::new_command("my_command")
                   ->arg(Arg::new_arg(ArgType::REQUIRED)->short_name('r')->range(NumType::INT, "5", "10"));

    CHECK_NO_THOW(cmd->parse_args({"my_command", "-r", "5"}));
    CHECK_NO_THOW(cmd->parse_args({"my_command", "-r", "8"}));
    CHECK_NO_THOW(cmd->parse_args({"my_command", "-r", "10"}));
    CHECK_THOW(cmd->parse_args({"my_command", "-r", "4"}), ParseArgsError);
    CHECK_THOW(cmd->parse_args({"my_command", "-r", "11"}), ParseArgsError);
}

ADD_UNIT_TEST_CASE(argparse, test_range_optional_long_name_arg)
{
    auto cmd = Command::new_command("my_command")
                   ->arg(Arg::new_arg(ArgType::OPTIONAL)->long_name("rangearg")->range(NumType::INT, "5", "10"));

    CHECK_NO_THOW(cmd->parse_args({"my_command", "--rangearg", "5"}));
    CHECK_NO_THOW(cmd->parse_args({"my_command", "--rangearg", "8"}));
    CHECK_NO_THOW(cmd->parse_args({"my_command", "--rangearg", "10"}));
    CHECK_THOW(cmd->parse_args({"my_command", "--rangearg", "4"}), ParseArgsError);
    CHECK_THOW(cmd->parse_args({"my_command", "--rangearg", "11"}), ParseArgsError);
}

ADD_UNIT_TEST_CASE(argparse, test_range_optional_short_name_arg)
{
    auto cmd = Command::new_command("my_command")
                   ->arg(Arg::new_arg(ArgType::OPTIONAL)->short_name('r')->range(NumType::INT, "5", "10"));

    CHECK_NO_THOW(cmd->parse_args({"my_command", "-r", "5"}));
    CHECK_NO_THOW(cmd->parse_args({"my_command", "-r", "8"}));
    CHECK_NO_THOW(cmd->parse_args({"my_command", "-r", "10"}));
    CHECK_THOW(cmd->parse_args({"my_command", "-r", "4"}), ParseArgsError);
    CHECK_THOW(cmd->parse_args({"my_command", "-r", "11"}), ParseArgsError);
}

ADD_UNIT_TEST_CASE(argparse, test_range_position_arg)
{
    auto cmd = Command::new_command("my_command")->arg(Arg::new_arg(ArgType::POSITION)->range(NumType::INT, "5", "10"));

    CHECK_NO_THOW(cmd->parse_args({"my_command", "5"}));
    CHECK_NO_THOW(cmd->parse_args({"my_command", "8"}));
    CHECK_NO_THOW(cmd->parse_args({"my_command", "10"}));
    CHECK_THOW(cmd->parse_args({"my_command", "4"}), ParseArgsError);
    CHECK_THOW(cmd->parse_args({"my_command", "11"}), ParseArgsError);
}

ADD_UNIT_TEST_CASE(argparse, test_very_big_range_value)
{
    auto cmd = Command::new_command("my_command")
                   ->arg(Arg::new_arg(ArgType::REQUIRED)
                             ->long_name("rangearg")
                             ->range(NumType::INT, "5", NumLimit::INT32_MAX_S));

    CHECK_NO_THOW(cmd->parse_args({"my_command", "--rangearg", "5"}));
    CHECK_NO_THOW(cmd->parse_args({"my_command", "--rangearg", "8"}));
    CHECK_NO_THOW(cmd->parse_args({"my_command", "--rangearg", NumLimit::INT32_MAX_S}));
    CHECK_THOW(cmd->parse_args({"my_command", "--rangearg", "4"}), ParseArgsError);
    CHECK_THOW(cmd->parse_args({"my_command", "--rangearg", "2147483648"}), ParseArgsError);
}

ADD_UNIT_TEST_CASE(argparse, test_choices_required_long_name_arg)
{
    auto cmd = Command::new_command("my_command")
                   ->arg(Arg::new_arg(ArgType::REQUIRED)->long_name("choarg")->choices(std::vector{"1", "2", "3"}));

    CHECK_NO_THOW(cmd->parse_args({"my_command", "--choarg", "1"}));
    CHECK_NO_THOW(cmd->parse_args({"my_command", "--choarg", "2"}));
    CHECK_NO_THOW(cmd->parse_args({"my_command", "--choarg", "3"}));
    CHECK_THOW(cmd->parse_args({"my_command", "--choarg", "4"}), ParseArgsError);
}

ADD_UNIT_TEST_CASE(argparse, test_choices_required_short_name_arg)
{
    auto cmd = Command::new_command("my_command")
                   ->arg(Arg::new_arg(ArgType::REQUIRED)->short_name('c')->choices(std::vector{"1", "2", "3"}));

    CHECK_NO_THOW(cmd->parse_args({"my_command", "-c", "1"}));
    CHECK_NO_THOW(cmd->parse_args({"my_command", "-c", "2"}));
    CHECK_NO_THOW(cmd->parse_args({"my_command", "-c", "3"}));
    CHECK_THOW(cmd->parse_args({"my_command", "-c", "4"}), ParseArgsError);
}

ADD_UNIT_TEST_CASE(argparse, test_choices_optional_long_name_arg)
{
    auto cmd = Command::new_command("my_command")
                   ->arg(Arg::new_arg(ArgType::OPTIONAL)->long_name("choarg")->choices(std::vector{"1", "2", "3"}));

    CHECK_NO_THOW(cmd->parse_args({"my_command", "--choarg", "1"}));
    CHECK_NO_THOW(cmd->parse_args({"my_command", "--choarg", "2"}));
    CHECK_NO_THOW(cmd->parse_args({"my_command", "--choarg", "3"}));
    CHECK_THOW(cmd->parse_args({"my_command", "--choarg", "4"}), ParseArgsError);
}

ADD_UNIT_TEST_CASE(argparse, test_choices_optional_short_name_arg)
{
    auto cmd = Command::new_command("my_command")
                   ->arg(Arg::new_arg(ArgType::OPTIONAL)->short_name('c')->choices(std::vector{"1", "2", "3"}));

    CHECK_NO_THOW(cmd->parse_args({"my_command", "-c", "1"}));
    CHECK_NO_THOW(cmd->parse_args({"my_command", "-c", "2"}));
    CHECK_NO_THOW(cmd->parse_args({"my_command", "-c", "3"}));
    CHECK_THOW(cmd->parse_args({"my_command", "-c", "4"}), ParseArgsError);
}

ADD_UNIT_TEST_CASE(argparse, test_choices_position_arg)
{
    auto cmd =
        Command::new_command("my_command")->arg(Arg::new_arg(ArgType::POSITION)->choices(std::vector{"1", "2", "3"}));

    CHECK_NO_THOW(cmd->parse_args({"my_command", "1"}));
    CHECK_NO_THOW(cmd->parse_args({"my_command", "2"}));
    CHECK_NO_THOW(cmd->parse_args({"my_command", "3"}));
    CHECK_THOW(cmd->parse_args({"my_command", "4"}), ParseArgsError);
}

ADD_UNIT_TEST_CASE(argparse, test_optional_long_name_arg_with_default_value)
{
    auto cmd = Command::new_command("my_command")
                   ->arg(Arg::new_arg(ArgType::OPTIONAL)->long_name("optional_arg")->default_value("1000"));

    cmd->parse_args({"my_command", "--optional_arg", "1"});
    CHECK_EQ(cmd->get_one_value<int>("optional_arg"), 1);

    cmd->parse_args({"my_command", "--optional_arg", "1"});
    CHECK_EQ(cmd->get_one_value<std::string_view>("optional_arg"), "1");

    cmd->parse_args({"my_command"});
    CHECK_EQ(cmd->get_one_value<int>("optional_arg"), 1000);

    cmd->parse_args({"my_command"});
    CHECK_EQ(cmd->get_one_value<std::string_view>("optional_arg"), "1000");

    CHECK_THOW(cmd->parse_args({"my_command", "--optional_arg"}), ParseArgsError);
}

ADD_UNIT_TEST_CASE(argparse, test_optional_short_name_arg_with_default_value)
{
    auto cmd = Command::new_command("my_command")
                   ->arg(Arg::new_arg(ArgType::OPTIONAL)->short_name('o')->default_value("1000"));

    cmd->parse_args({"my_command", "-o", "1"});
    CHECK_EQ(cmd->get_one_value<int>('o'), 1);

    cmd->parse_args({"my_command", "-o", "1"});
    CHECK_EQ(cmd->get_one_value<std::string_view>('o'), "1");

    cmd->parse_args({"my_command"});
    CHECK_EQ(cmd->get_one_value<int>('o'), 1000);

    cmd->parse_args({"my_command"});
    CHECK_EQ(cmd->get_one_value<std::string_view>('o'), "1000");

    CHECK_THOW(cmd->parse_args({"my_command", "-o"}), ParseArgsError);
}

ADD_UNIT_TEST_CASE(argparse, test_optional_long_name_arg_with_default_values)
{
    auto cmd =
        Command::new_command("my_command")
            ->arg(Arg::new_arg(ArgType::OPTIONAL)->long_name("optional_arg")->default_values({"100", "200", "300"}));

    cmd->parse_args({"my_command", "--optional_arg", "1", "--optional_arg", "2", "--optional_arg", "3"});
    CHECK_ARRAY_EQ(cmd->get_many_values<int>("optional_arg"), (std::vector<int>{1, 2, 3}));

    cmd->parse_args({"my_command", "--optional_arg", "1", "--optional_arg", "2", "--optional_arg", "3"});
    CHECK_ARRAY_EQ(cmd->get_many_values<std::string_view>("optional_arg"),
                   (std::vector<std::string_view>{"1", "2", "3"}));

    cmd->parse_args({"my_command"});
    CHECK_ARRAY_EQ(cmd->get_many_values<int>("optional_arg"), (std::vector<int>{100, 200, 300}));

    cmd->parse_args({"my_command"});
    CHECK_ARRAY_EQ(cmd->get_many_values<std::string_view>("optional_arg"),
                   (std::vector<std::string_view>{"100", "200", "300"}));

    CHECK_THOW(cmd->parse_args({"my_command", "--optional_arg"}), ParseArgsError);
}

ADD_UNIT_TEST_CASE(argparse, test_optional_short_name_arg_with_default_values)
{
    auto cmd = Command::new_command("my_command")
                   ->arg(Arg::new_arg(ArgType::OPTIONAL)->short_name('o')->default_values({"100", "200", "300"}));

    cmd->parse_args({"my_command", "-o", "1", "-o", "2", "-o", "3"});
    CHECK_ARRAY_EQ(cmd->get_many_values<int>('o'), (std::vector<int>{1, 2, 3}));

    cmd->parse_args({"my_command", "-o", "1", "-o", "2", "-o", "3"});
    CHECK_ARRAY_EQ(cmd->get_many_values<std::string_view>('o'), (std::vector<std::string_view>{"1", "2", "3"}));

    cmd->parse_args({"my_command"});
    CHECK_ARRAY_EQ(cmd->get_many_values<int>('o'), (std::vector<int>{100, 200, 300}));

    cmd->parse_args({"my_command"});
    CHECK_ARRAY_EQ(cmd->get_many_values<std::string_view>('o'), (std::vector<std::string_view>{"100", "200", "300"}));

    CHECK_THOW(cmd->parse_args({"my_command", "-o"}), ParseArgsError);
}

ADD_UNIT_TEST_CASE(argparse, test_flag_long_name_arg)
{
    auto cmd = Command::new_command("my_command")->arg(Arg::new_arg(ArgType::FLAG)->long_name("flag_arg"));

    cmd->parse_args({"my_command"});
    CHECK_EQ(cmd->has_arg("flag_arg"), false);
    CHECK_EQ(cmd->get_one_value<int>("flag_arg"), 0);

    cmd->parse_args({"my_command", "--flag_arg"});
    CHECK_EQ(cmd->has_arg("flag_arg"), true);
    CHECK_EQ(cmd->get_one_value<int>("flag_arg"), 1);
}

ADD_UNIT_TEST_CASE(argparse, test_flag_short_name_arg)
{
    auto cmd = Command::new_command("my_command")->arg(Arg::new_arg(ArgType::FLAG)->short_name('f'));

    cmd->parse_args({"my_command"});
    CHECK_EQ(cmd->has_arg('f'), false);
    CHECK_EQ(cmd->get_one_value<int>('f'), 0);

    cmd->parse_args({"my_command", "-f"});
    CHECK_EQ(cmd->has_arg('f'), true);
    CHECK_EQ(cmd->get_one_value<int>('f'), 1);
}

ADD_UNIT_TEST_CASE(argparse, test_position_arg)
{
    auto cmd = Command::new_command("my_command")
                   ->arg(Arg::new_arg(ArgType::POSITION))
                   ->arg(Arg::new_arg(ArgType::POSITION))
                   ->arg(Arg::new_arg(ArgType::POSITION));

    cmd->parse_args({"my_command", "1", "2", "3"});
    CHECK_THOW(cmd->get_one_position_value<int>(3), ParseArgsError);
    CHECK_EQ(cmd->get_one_position_value<int>(0), 1);
    CHECK_EQ(cmd->get_one_position_value<int>(1), 2);
    CHECK_EQ(cmd->get_one_position_value<int>(2), 3);
    CHECK_ARRAY_EQ(cmd->get_all_position_values<int>(), (std::vector<int>{1, 2, 3}));

    cmd->parse_args({"my_command", "1", "2", "name"});
    CHECK_THOW(cmd->get_one_position_value<int>(3), ParseArgsError);
    CHECK_EQ(cmd->get_one_position_value<int>(0), 1);
    CHECK_EQ(cmd->get_one_position_value<int>(1), 2);
    CHECK_EQ(cmd->get_one_position_value<std::string_view>(2), "name");
}

ADD_UNIT_TEST_CASE(argparse, test_position_arg_you_do_not_know_how_many_beforehand)
{
    auto cmd = Command::new_command("my_command");

    cmd->parse_args({"my_command", "1", "2", "3"});
    CHECK_THOW(cmd->get_one_position_value<int>(3), ParseArgsError);
    CHECK_EQ(cmd->get_one_position_value<int>(0), 1);
    CHECK_EQ(cmd->get_one_position_value<int>(1), 2);
    CHECK_EQ(cmd->get_one_position_value<int>(2), 3);
    CHECK_ARRAY_EQ(cmd->get_all_position_values<int>(), (std::vector<int>{1, 2, 3}));

    cmd->parse_args({"my_command", "1", "2", "name"});
    CHECK_THOW(cmd->get_one_position_value<int>(3), ParseArgsError);
    CHECK_EQ(cmd->get_one_position_value<int>(0), 1);
    CHECK_EQ(cmd->get_one_position_value<int>(1), 2);
    CHECK_EQ(cmd->get_one_position_value<std::string_view>(2), "name");
}

ADD_UNIT_TEST_CASE(argparse, test_long_name_conflicts_with_all_flag_arg)
{
    auto cmd = Command::new_command("my_command")
                   ->arg(Arg::new_arg(ArgType::FLAG)->long_name("aa")->conflicts_with_all())
                   ->arg(Arg::new_arg(ArgType::FLAG)->long_name("bb"))
                   ->arg(Arg::new_arg(ArgType::FLAG)->long_name("cc"));

    CHECK_NO_THOW(cmd->parse_args({"my_command", "--aa"}));
    CHECK_THOW(cmd->parse_args({"my_command", "--aa", "--bb"}), ParseArgsError);
    CHECK_THOW(cmd->parse_args({"my_command", "--aa", "--cc"}), ParseArgsError);
    CHECK_THOW(cmd->parse_args({"my_command", "--aa", "--bb", "--cc"}), ParseArgsError);
    CHECK_NO_THOW(cmd->parse_args({"my_command", "--bb", "--cc"}));
}

ADD_UNIT_TEST_CASE(argparse, test_short_name_conflicts_with_all_flag_arg)
{
    auto cmd = Command::new_command("my_command")
                   ->arg(Arg::new_arg(ArgType::FLAG)->short_name('a')->conflicts_with_all())
                   ->arg(Arg::new_arg(ArgType::FLAG)->short_name('b'))
                   ->arg(Arg::new_arg(ArgType::FLAG)->short_name('c'));

    CHECK_NO_THOW(cmd->parse_args({"my_command", "-a"}));
    CHECK_THOW(cmd->parse_args({"my_command", "-a", "-b"}), ParseArgsError);
    CHECK_THOW(cmd->parse_args({"my_command", "-a", "-c"}), ParseArgsError);
    CHECK_THOW(cmd->parse_args({"my_command", "-a", "-b", "-c"}), ParseArgsError);
    CHECK_NO_THOW(cmd->parse_args({"my_command", "-b", "-c"}));
}

ADD_UNIT_TEST_CASE(argparse, test_mixed_long_short_name_conflicts_with_all_flag_arg)
{
    auto cmd = Command::new_command("my_command")
                   ->arg(Arg::new_arg(ArgType::FLAG)->long_name("aa")->conflicts_with_all())
                   ->arg(Arg::new_arg(ArgType::FLAG)->short_name('b'))
                   ->arg(Arg::new_arg(ArgType::FLAG)->short_name('c'));

    CHECK_NO_THOW(cmd->parse_args({"my_command", "--aa"}));
    CHECK_THOW(cmd->parse_args({"my_command", "--aa", "-b"}), ParseArgsError);
    CHECK_THOW(cmd->parse_args({"my_command", "--aa", "-c"}), ParseArgsError);
    CHECK_THOW(cmd->parse_args({"my_command", "--aa", "-b", "-c"}), ParseArgsError);
    CHECK_NO_THOW(cmd->parse_args({"my_command", "-b", "-c"}));
}

ADD_UNIT_TEST_CASE(argparse, test_mixed_short_long_name_conflicts_with_all_flag_arg)
{
    auto cmd = Command::new_command("my_command")
                   ->arg(Arg::new_arg(ArgType::FLAG)->short_name('a')->conflicts_with_all())
                   ->arg(Arg::new_arg(ArgType::FLAG)->long_name("bb"))
                   ->arg(Arg::new_arg(ArgType::FLAG)->long_name("cc"));

    CHECK_NO_THOW(cmd->parse_args({"my_command", "-a"}));
    CHECK_THOW(cmd->parse_args({"my_command", "-a", "--bb"}), ParseArgsError);
    CHECK_THOW(cmd->parse_args({"my_command", "-a", "--cc"}), ParseArgsError);
    CHECK_THOW(cmd->parse_args({"my_command", "-a", "--bb", "--cc"}), ParseArgsError);
    CHECK_NO_THOW(cmd->parse_args({"my_command", "--bb", "--cc"}));
}

ADD_UNIT_TEST_CASE(argparse, test_long_name_conflicts_with_all_optional_arg)
{
    auto cmd = Command::new_command("my_command")
                   ->arg(Arg::new_arg(ArgType::OPTIONAL)->long_name("aa")->conflicts_with_all())
                   ->arg(Arg::new_arg(ArgType::OPTIONAL)->long_name("bb"))
                   ->arg(Arg::new_arg(ArgType::OPTIONAL)->long_name("cc"));

    CHECK_NO_THOW(cmd->parse_args({"my_command", "--aa", "1"}));
    CHECK_THOW(cmd->parse_args({"my_command", "--aa", "1", "--bb", "2"}), ParseArgsError);
    CHECK_THOW(cmd->parse_args({"my_command", "--aa", "1", "--cc", "3"}), ParseArgsError);
    CHECK_THOW(cmd->parse_args({"my_command", "--aa", "1", "--bb", "2", "--cc", "3"}), ParseArgsError);
    CHECK_NO_THOW(cmd->parse_args({"my_command", "--bb", "2", "--cc", "3"}));
}

ADD_UNIT_TEST_CASE(argparse, test_short_name_conflicts_with_all_optional_arg)
{
    auto cmd = Command::new_command("my_command")
                   ->arg(Arg::new_arg(ArgType::FLAG)->short_name('a')->conflicts_with_all())
                   ->arg(Arg::new_arg(ArgType::FLAG)->short_name('b'))
                   ->arg(Arg::new_arg(ArgType::FLAG)->short_name('c'));

    CHECK_NO_THOW(cmd->parse_args({"my_command", "-a", "1"}));
    CHECK_THOW(cmd->parse_args({"my_command", "-a", "1", "-b", "2"}), ParseArgsError);
    CHECK_THOW(cmd->parse_args({"my_command", "-a", "1", "-c", "3"}), ParseArgsError);
    CHECK_THOW(cmd->parse_args({"my_command", "-a", "1", "-b", "2", "-c", "3"}), ParseArgsError);
    CHECK_NO_THOW(cmd->parse_args({"my_command", "-b", "2", "-c", "3"}));
}

ADD_UNIT_TEST_CASE(argparse, test_mixed_long_short_name_conflicts_with_all_optional_arg)
{
    auto cmd = Command::new_command("my_command")
                   ->arg(Arg::new_arg(ArgType::OPTIONAL)->long_name("aa")->conflicts_with_all())
                   ->arg(Arg::new_arg(ArgType::OPTIONAL)->short_name('b'))
                   ->arg(Arg::new_arg(ArgType::OPTIONAL)->short_name('c'));

    CHECK_NO_THOW(cmd->parse_args({"my_command", "-aa", "1"}));
    CHECK_THOW(cmd->parse_args({"my_command", "-aa", "1", "-b", "2"}), ParseArgsError);
    CHECK_THOW(cmd->parse_args({"my_command", "-aa", "1", "-c", "3"}), ParseArgsError);
    CHECK_THOW(cmd->parse_args({"my_command", "-aa", "1", "-b", "2", "-c", "3"}), ParseArgsError);
    CHECK_NO_THOW(cmd->parse_args({"my_command", "-b", "2", "-c", "3"}));
}

ADD_UNIT_TEST_CASE(argparse, test_mixed_short_long_name_conflicts_with_all_optional_arg)
{
    auto cmd = Command::new_command("my_command")
                   ->arg(Arg::new_arg(ArgType::OPTIONAL)->short_name('a')->conflicts_with_all())
                   ->arg(Arg::new_arg(ArgType::OPTIONAL)->long_name("bb"))
                   ->arg(Arg::new_arg(ArgType::OPTIONAL)->long_name("cc"));

    CHECK_NO_THOW(cmd->parse_args({"my_command", "-a", "1"}));
    CHECK_THOW(cmd->parse_args({"my_command", "-a", "1", "--bb", "2"}), ParseArgsError);
    CHECK_THOW(cmd->parse_args({"my_command", "-a", "1", "--cc", "3"}), ParseArgsError);
    CHECK_THOW(cmd->parse_args({"my_command", "-a", "1", "--bb", "2", "--cc", "3"}), ParseArgsError);
    CHECK_NO_THOW(cmd->parse_args({"my_command", "--bb", "2", "--cc", "3"}));
}

ADD_UNIT_TEST_CASE(argparse, test_long_name_related_group)
{
    auto cmd = Command::new_command("my_command")
                   ->arg(Arg::new_arg(ArgType::OPTIONAL)->long_name("aa"))
                   ->arg(Arg::new_arg(ArgType::OPTIONAL)->long_name("bb"))
                   ->arg(Arg::new_arg(ArgType::OPTIONAL)->long_name("cc"))
                   ->related_group({"aa", "bb", "cc"});

    CHECK_NO_THOW(cmd->parse_args({"my_command"}));
    CHECK_NO_THOW(cmd->parse_args({"my_command", "--aa", "1", "--bb", "2", "--cc", "3"}));
    CHECK_THOW(cmd->parse_args({"my_command", "--aa", "1"}), ParseArgsError);
    CHECK_THOW(cmd->parse_args({"my_command", "--aa", "1", "--bb", "2"}), ParseArgsError);
    CHECK_THOW(cmd->parse_args({"my_command", "--aa", "1", "--cc", "3"}), ParseArgsError);
}

ADD_UNIT_TEST_CASE(argparse, test_short_name_related_group)
{
    auto cmd = Command::new_command("my_command")
                   ->arg(Arg::new_arg(ArgType::OPTIONAL)->short_name('a'))
                   ->arg(Arg::new_arg(ArgType::OPTIONAL)->short_name('b'))
                   ->arg(Arg::new_arg(ArgType::OPTIONAL)->short_name('c'))
                   ->related_group({"a", "b", "c"});

    CHECK_NO_THOW(cmd->parse_args({"my_command"}));
    CHECK_NO_THOW(cmd->parse_args({"my_command", "-a", "1", "-b", "2", "-c", "3"}));
    CHECK_THOW(cmd->parse_args({"my_command", "-a", "1"}), ParseArgsError);
    CHECK_THOW(cmd->parse_args({"my_command", "-a", "1", "-b", "2"}), ParseArgsError);
    CHECK_THOW(cmd->parse_args({"my_command", "-a", "1", "-c", "3"}), ParseArgsError);
}

ADD_UNIT_TEST_CASE(argparse, test_mixed_long_short_name_related_group)
{
    auto cmd = Command::new_command("my_command")
                   ->arg(Arg::new_arg(ArgType::OPTIONAL)->long_name("aa"))
                   ->arg(Arg::new_arg(ArgType::OPTIONAL)->short_name('b'))
                   ->arg(Arg::new_arg(ArgType::OPTIONAL)->short_name('c'))
                   ->related_group({"aa", "b", "c"});

    CHECK_NO_THOW(cmd->parse_args({"my_command"}));
    CHECK_NO_THOW(cmd->parse_args({"my_command", "-aa", "1", "-b", "2", "-c", "3"}));
    CHECK_THOW(cmd->parse_args({"my_command", "-aa", "1"}), ParseArgsError);
    CHECK_THOW(cmd->parse_args({"my_command", "-aa", "1", "-b", "2"}), ParseArgsError);
    CHECK_THOW(cmd->parse_args({"my_command", "-aa", "1", "-c", "3"}), ParseArgsError);
}

ADD_UNIT_TEST_CASE(argparse, test_long_name_conflict_group)
{
    auto cmd = Command::new_command("my_command")
                   ->arg(Arg::new_arg(ArgType::OPTIONAL)->long_name("aa"))
                   ->arg(Arg::new_arg(ArgType::OPTIONAL)->long_name("bb"))
                   ->arg(Arg::new_arg(ArgType::OPTIONAL)->long_name("cc"))
                   ->conflict_group({"aa", "bb", "cc"});

    CHECK_NO_THOW(cmd->parse_args({"my_command"}));
    CHECK_NO_THOW(cmd->parse_args({"my_command", "--aa", "1"}));
    CHECK_NO_THOW(cmd->parse_args({"my_command", "--bb", "1"}));
    CHECK_NO_THOW(cmd->parse_args({"my_command", "--cc", "1"}));
    CHECK_THOW(cmd->parse_args({"my_command", "--aa", "1", "--bb", "2"}), ParseArgsError);
    CHECK_THOW(cmd->parse_args({"my_command", "--aa", "1", "--cc", "3"}), ParseArgsError);
    CHECK_THOW(cmd->parse_args({"my_command", "--bb", "1", "--cc", "3"}), ParseArgsError);
    CHECK_THOW(cmd->parse_args({"my_command", "--aa", "1", "--bb", "2", "--cc", "3"}), ParseArgsError);
}

ADD_UNIT_TEST_CASE(argparse, test_short_name_conflict_group)
{
    auto cmd = Command::new_command("my_command")
                   ->arg(Arg::new_arg(ArgType::OPTIONAL)->short_name('a'))
                   ->arg(Arg::new_arg(ArgType::OPTIONAL)->short_name('b'))
                   ->arg(Arg::new_arg(ArgType::OPTIONAL)->short_name('c'))
                   ->conflict_group({"a", "b", "c"});

    CHECK_NO_THOW(cmd->parse_args({"my_command"}));
    CHECK_NO_THOW(cmd->parse_args({"my_command", "-a", "1"}));
    CHECK_NO_THOW(cmd->parse_args({"my_command", "-b", "1"}));
    CHECK_NO_THOW(cmd->parse_args({"my_command", "-c", "1"}));
    CHECK_THOW(cmd->parse_args({"my_command", "-a", "1", "-b", "2"}), ParseArgsError);
    CHECK_THOW(cmd->parse_args({"my_command", "-a", "1", "-c", "3"}), ParseArgsError);
    CHECK_THOW(cmd->parse_args({"my_command", "-b", "1", "-c", "3"}), ParseArgsError);
    CHECK_THOW(cmd->parse_args({"my_command", "-a", "1", "-b", "2", "-c", "3"}), ParseArgsError);
}

ADD_UNIT_TEST_CASE(argparse, test_mixed_long_short_name_conflict_group)
{
    auto cmd = Command::new_command("my_command")
                   ->arg(Arg::new_arg(ArgType::OPTIONAL)->long_name("aa"))
                   ->arg(Arg::new_arg(ArgType::OPTIONAL)->short_name('b'))
                   ->arg(Arg::new_arg(ArgType::OPTIONAL)->short_name('c'))
                   ->conflict_group({"aa", "b", "c"});

    CHECK_NO_THOW(cmd->parse_args({"my_command"}));
    CHECK_NO_THOW(cmd->parse_args({"my_command", "--aa", "1"}));
    CHECK_NO_THOW(cmd->parse_args({"my_command", "-b", "1"}));
    CHECK_NO_THOW(cmd->parse_args({"my_command", "-c", "1"}));
    CHECK_THOW(cmd->parse_args({"my_command", "--aa", "1", "-b", "2"}), ParseArgsError);
    CHECK_THOW(cmd->parse_args({"my_command", "--aa", "1", "-c", "3"}), ParseArgsError);
    CHECK_THOW(cmd->parse_args({"my_command", "-b", "1", "-c", "3"}), ParseArgsError);
    CHECK_THOW(cmd->parse_args({"my_command", "--aa", "1", "-b", "2", "-c", "3"}), ParseArgsError);
}

ADD_UNIT_TEST_CASE(argparse, test_long_name_one_required_group)
{
    auto cmd = Command::new_command("my_command")
                   ->arg(Arg::new_arg(ArgType::OPTIONAL)->long_name("aa"))
                   ->arg(Arg::new_arg(ArgType::OPTIONAL)->long_name("bb"))
                   ->arg(Arg::new_arg(ArgType::OPTIONAL)->long_name("cc"))
                   ->one_required_group({"aa", "bb", "cc"});

    CHECK_THOW(cmd->parse_args({"my_command"}), ParseArgsError);
    CHECK_NO_THOW(cmd->parse_args({"my_command", "--aa", "1"}));
    CHECK_NO_THOW(cmd->parse_args({"my_command", "--bb", "1"}));
    CHECK_NO_THOW(cmd->parse_args({"my_command", "--cc", "1"}));
    CHECK_NO_THOW(cmd->parse_args({"my_command", "--aa", "1", "--bb", "2"}));
    CHECK_NO_THOW(cmd->parse_args({"my_command", "--aa", "1", "--cc", "3"}));
    CHECK_NO_THOW(cmd->parse_args({"my_command", "--bb", "1", "--cc", "3"}));
    CHECK_NO_THOW(cmd->parse_args({"my_command", "--aa", "1", "--bb", "2", "--cc", "3"}));
}

ADD_UNIT_TEST_CASE(argparse, test_short_name_one_required_group)
{
    auto cmd = Command::new_command("my_command")
                   ->arg(Arg::new_arg(ArgType::OPTIONAL)->short_name('a'))
                   ->arg(Arg::new_arg(ArgType::OPTIONAL)->short_name('b'))
                   ->arg(Arg::new_arg(ArgType::OPTIONAL)->short_name('c'))
                   ->one_required_group({"a", "b", "c"});

    CHECK_THOW(cmd->parse_args({"my_command"}), ParseArgsError);
    CHECK_NO_THOW(cmd->parse_args({"my_command", "-a", "1"}));
    CHECK_NO_THOW(cmd->parse_args({"my_command", "-b", "1"}));
    CHECK_NO_THOW(cmd->parse_args({"my_command", "-c", "1"}));
    CHECK_NO_THOW(cmd->parse_args({"my_command", "-a", "1", "-b", "2"}));
    CHECK_NO_THOW(cmd->parse_args({"my_command", "-a", "1", "-c", "3"}));
    CHECK_NO_THOW(cmd->parse_args({"my_command", "-b", "1", "-c", "3"}));
    CHECK_NO_THOW(cmd->parse_args({"my_command", "-a", "1", "-b", "2", "-c", "3"}));
}

ADD_UNIT_TEST_CASE(argparse, test_mixed_long_short_name_one_required_group)
{
    auto cmd = Command::new_command("my_command")
                   ->arg(Arg::new_arg(ArgType::OPTIONAL)->long_name("aa"))
                   ->arg(Arg::new_arg(ArgType::OPTIONAL)->short_name('b'))
                   ->arg(Arg::new_arg(ArgType::OPTIONAL)->short_name('c'))
                   ->one_required_group({"aa", "b", "c"});

    CHECK_THOW(cmd->parse_args({"my_command"}), ParseArgsError);
    CHECK_NO_THOW(cmd->parse_args({"my_command", "--aa", "1"}));
    CHECK_NO_THOW(cmd->parse_args({"my_command", "-b", "1"}));
    CHECK_NO_THOW(cmd->parse_args({"my_command", "-c", "1"}));
    CHECK_NO_THOW(cmd->parse_args({"my_command", "--aa", "1", "-b", "2"}));
    CHECK_NO_THOW(cmd->parse_args({"my_command", "--aa", "1", "-c", "3"}));
    CHECK_NO_THOW(cmd->parse_args({"my_command", "-b", "1", "-c", "3"}));
    CHECK_NO_THOW(cmd->parse_args({"my_command", "--aa", "1", "-b", "2", "-c", "3"}));
}

ADD_UNIT_TEST_CASE(argparse, test_subcommand)
{
    auto cmd = Command::new_command("my_command")
                   ->arg(Arg::new_arg(ArgType::FLAG)->long_name("detail"))
                   ->arg(Arg::new_arg(ArgType::OPTIONAL)->long_name("debug"))
                   ->subcommand(Command::new_command("myfind")
                                    ->subcommand(Command::new_command("mydirfind")
                                                     ->arg(Arg::new_arg(ArgType::REQUIRED)->long_name("dirpath"))
                                                     ->arg(Arg::new_arg(ArgType::REQUIRED)->long_name("pattern")))
                                    ->subcommand(Command::new_command("myfilefind")
                                                     ->arg(Arg::new_arg(ArgType::REQUIRED)->long_name("filepath"))
                                                     ->arg(Arg::new_arg(ArgType::REQUIRED)->long_name("pattern"))))
                   ->subcommand(Command::new_command("mygrep")
                                    ->arg(Arg::new_arg(ArgType::OPTIONAL)->long_name("dirpath"))
                                    ->arg(Arg::new_arg(ArgType::OPTIONAL)->long_name("pattern")));

    CHECK_THOW(cmd->parse_args({"my_command", "--detail"}), ParseArgsError);
    CHECK_THOW(cmd->parse_args({"my_command", "--detail", "myfind"}), ParseArgsError);
    CHECK_THOW(cmd->parse_args({"my_command", "--detail", "myfind", "mydirfind"}), ParseArgsError);
    CHECK_NO_THOW(
        cmd->parse_args({"my_command", "--detail", "myfind", "mydirfind", "--dirpath", "xx1", "--pattern", "yy2"}));

    cmd->parse_args({"my_command", "--detail", "myfind", "mydirfind", "--dirpath", "xx1", "--pattern", "yy2"});
    auto myfind = cmd->get_subcommand();
    auto mydirfind = myfind->get_subcommand();
    CHECK_EQ(mydirfind->get_one_value<std::string_view>("dirpath"), "xx1");
    CHECK_EQ(mydirfind->get_one_value<std::string_view>("pattern"), "yy2");

    CHECK_NO_THOW(cmd->parse_args({"my_command", "--detail", "mygrep"}));

    cmd->parse_args({"my_command", "--detail", "mygrep", "--dirpath", "uu1", "--pattern", "vv2"});
    auto mygrep = cmd->get_subcommand();
    CHECK_EQ(mygrep->get_one_value<std::string_view>("dirpath"), "uu1");
    CHECK_EQ(mygrep->get_one_value<std::string_view>("pattern"), "vv2");

    cmd->parse_args({"my_command", "--detail", "myfind", "mydirfind", "--dirpath", "xx1", "--pattern", "yy2"});
    auto myfind_or_mygrep = cmd->get_subcommand();
    if (myfind_or_mygrep->command_name_sv() == "myfind") {
        auto inner_command = myfind_or_mygrep->get_subcommand();
        if (inner_command->command_name_sv() == "mydirfind") {
            [[maybe_unused]] std::string_view dirpath = inner_command->get_one_value<std::string_view>("dirpath");
            [[maybe_unused]] std::string_view pattern = inner_command->get_one_value<std::string_view>("pattern");
            // TODO 你自己的业务逻辑
        } else if (inner_command->command_name_sv() == "myfilefind") {
            [[maybe_unused]] std::string_view dirpath = inner_command->get_one_value<std::string_view>("filepath");
            [[maybe_unused]] std::string_view pattern = inner_command->get_one_value<std::string_view>("pattern");
            // TODO 你自己的业务逻辑
        }
    } else if (myfind_or_mygrep->command_name_sv() == "mygrep") {
        [[maybe_unused]] std::string_view dirpath = myfind_or_mygrep->get_one_value<std::string_view>("dirpath");
        [[maybe_unused]] std::string_view pattern = myfind_or_mygrep->get_one_value<std::string_view>("pattern");
        // TODO 你自己的业务逻辑
    }
}
