# 1. 前言

作者本人热衷于根据工作和生活中的需求，开发一些小工具来解决问题。对于效率要求不高的场景，Python 足以胜任；但在需要更高性能时，C++ 或 Rust 则是更好的选择。无论是哪种语言，命令行参数传递与解析都是工具开发中不可或缺的一部分。Python 提供了内置的 `argparse` 模块，Rust 则有强大的 `clap` crate，但 C++ 在这方面缺乏统一的标准，导致 GitHub 上出现了许多参数解析框架。然而，作者在使用这些框架时，总觉得它们不够完美，难以满足自己的需求。

为了提高自己的 C++ 编程能力，并结合日常开发中的实际需求，作者参考了 `argparse` 和 `clap` 的设计理念，打造了一个 C++ 参数解析库。经过多次迭代，目前已经演进到第 6 个版本。作者认为这个框架设计简洁、功能实用，因此决定将其开源，供大家学习和参考。

无版权限制，可任意使用，但请标明来源。

# 2. 框架优点

- **简洁的语法**：参考 Rust `clap` 的设计，轻松添加参数和子命令。
- **嵌套子命令**：支持子命令的无限嵌套，满足复杂命令行工具的需求。
- **参数校验**：支持参数取值范围的校验，确保输入合法性。
- **参数枚举**：限制参数只能从特定值中选取，避免无效输入。
- **参数关系控制**：支持参数之间的关联关系和互斥关系，增强逻辑严谨性。
- **便捷的值获取**：轻松获取参数的值，支持字符串到所需类型的自动转换。
- **多值参数**：支持传递多个值，最终可获得一个数组。
- **位置参数**：可方便获取指定位置的值，以及所有的值。
- **底层存储**：数据以 `const char *` 存储，取出时按需转换为目标类型（如字符串转数字）。
- **中文注释**：大量中文注释，便于理解代码和接口使用方法。
- **单元测试**：包含大量单元测试用例，确保框架的可靠性和稳定性。
- **接口示例**：每个对外功能均有对应的单元测试，方便学习和掌握接口用法。
- **轻量单元测试框架**：附带一个仅包含头文件的单元测试框架，无需安装 gtest 即可进行测试。
- **极简集成**：仅包含一个 `.h` 文件和一个 `.cpp` 文件，无额外依赖，可轻松集成到项目中。

# 3. 注意事项

- **分支策略**：仅在主分支（`master`）上进行开发和演进，不额外打标签。
- **平台支持**：已在 Linux 上测试通过，未在 Windows 上进行测试（作者专注于 Linux 平台应用开发）。

# 4. 构建环境

本项目使用 C++17 构建，用到了一些 C++17 中才支持的语法，因此 C++11 的用户可以绕过了。作者认为都已经 2025 年了，用 C++17 不过分吧。

作者的编译和测试环境如下：

- Ubuntu 22.04 LTS
- C++17，gcc 11.4.0，clang 14.0.0

在此环境下一定能编译和测试通过。

# 5. 快速开始

## 5.1. 唯一配置

```cpp
// 唯一配置项（根据实际需求进行配置）
// - 如果 `EXIT_WHEN_ERROR == true`，则解析到非法参数时直接退出程序，一般应该用这种模式。
// - 如果 `EXIT_WHEN_ERROR == false`，则解析到非法参数时抛出异常。
//   在交互模式下进行参数解析，可用这种方式，这样就不用退出当前程序，只用捕获异常并处理即可，然后就可以进入下一轮参数解析。
//   本项目的单元测试程序 `test_argparse.cpp`就是用的这种方式。因为单元测试程序本来就要测试异常场景，
//   而如果遇到错误直接退出程序，那么单元测试就无法继续。
constexpr bool EXIT_WHEN_ERROR = false;
```

## 5.2. 快速构建

首先，在工程的根目录下执行 `./scripts/build_x86_version.sh` 即可编译构建。

**使用 clang++ 编译：**

```bash
[11:07:43  argparse]# ll
total 8
-rwxrwxrwx 1 root root 3171 Mar 16 11:05 CMakeLists.txt
-rwxrwxrwx 1 root root 2315 Mar 16 11:07 ReadMe.md
drwxrwxrwx 1 root root 4096 Mar 16 10:41 include
drwxrwxrwx 1 root root 4096 Mar 16 11:07 my_release
drwxrwxrwx 1 root root 4096 Mar 14 14:12 scripts
drwxrwxrwx 1 root root 4096 Mar 14 14:15 src
drwxrwxrwx 1 root root 4096 Mar 15 16:14 test
[11:07:44  argparse]# ./scripts/build_x86_version.sh
Build x86 version.
Build Release version.
Build dynamically linked version.
-- The C compiler identification is Clang 14.0.0
-- The CXX compiler identification is Clang 14.0.0
-- Detecting C compiler ABI info
-- Detecting C compiler ABI info - done
-- Check for working C compiler: /usr/bin/clang - skipped
-- Detecting C compile features
-- Detecting C compile features - done
-- Detecting CXX compiler ABI info
-- Detecting CXX compiler ABI info - done
-- Check for working CXX compiler: /usr/bin/clang++ - skipped
-- Detecting CXX compile features
-- Detecting CXX compile features - done
-- Configuring done
-- Generating done
-- Build files have been written to: /root/zdy-repo/c_plus_plus/argparse/tmp_build_dir
[ 33%] Building CXX object CMakeFiles/argparse_obj.dir/src/argparse.cpp.o
[ 33%] Built target argparse_obj
[ 66%] Building CXX object CMakeFiles/test_argparse.dir/test/test_argparse.cpp.o
[100%] Linking CXX executable ../my_release/x86/test_argparse
[100%] Built target test_argparse
Build completed!
[11:07:52  argparse]#
```

**使用 g++ 编译：**

```bash
[11:52:26  argparse]# ./scripts/build_x86_version.sh
Build x86 version.
Build Release version.
Build dynamically linked version.
-- The C compiler identification is GNU 11.4.0
-- The CXX compiler identification is GNU 11.4.0
-- Detecting C compiler ABI info
-- Detecting C compiler ABI info - done
-- Check for working C compiler: /usr/bin/cc - skipped
-- Detecting C compile features
-- Detecting C compile features - done
-- Detecting CXX compiler ABI info
-- Detecting CXX compiler ABI info - done
-- Check for working CXX compiler: /usr/bin/c++ - skipped
-- Detecting CXX compile features
-- Detecting CXX compile features - done
-- Configuring done
-- Generating done
-- Build files have been written to: /root/zdy-repo/c_plus_plus/argparse/tmp_build_dir
[ 33%] Building CXX object CMakeFiles/argparse_obj.dir/src/argparse.cpp.o
[ 33%] Built target argparse_obj
[ 66%] Building CXX object CMakeFiles/test_argparse.dir/test/test_argparse.cpp.o
[100%] Linking CXX executable ../my_release/x86/test_argparse
[100%] Built target test_argparse
Build completed!
```

## 5.3. 运行单元测试

编译完毕后，继续在工程的根目录下执行 `./scripts/run_all_test.sh` 即可运行所有的单元测试用例。

```bash
[11:08:18  argparse]# ./scripts/run_all_test.sh

Running ***********************[test_argparse]***********************

argparse_unit_test_app

----> Test PASSED - [argparse] [test_cannot_add_help_by_yourself]
my_command: option '--reqarg' requires an argument
----> Test PASSED - [argparse] [test_required_long_name_arg]
my_command: option requires an argument -- 'r'
----> Test PASSED - [argparse] [test_required_short_name_arg]
my_command: option '--optarg' requires an argument
----> Test PASSED - [argparse] [test_optional_long_name_arg]
my_command: option requires an argument -- 'o'
----> Test PASSED - [argparse] [test_optional_short_name_arg]
----> Test PASSED - [argparse] [test_range_required_long_name_arg]
----> Test PASSED - [argparse] [test_range_required_short_name_arg]
----> Test PASSED - [argparse] [test_range_optional_long_name_arg]
----> Test PASSED - [argparse] [test_range_optional_short_name_arg]
----> Test PASSED - [argparse] [test_range_position_arg]
----> Test PASSED - [argparse] [test_very_big_range_value]
----> Test PASSED - [argparse] [test_choices_required_long_name_arg]
----> Test PASSED - [argparse] [test_choices_required_short_name_arg]
----> Test PASSED - [argparse] [test_choices_optional_long_name_arg]
----> Test PASSED - [argparse] [test_choices_optional_short_name_arg]
----> Test PASSED - [argparse] [test_choices_position_arg]
my_command: option '--optional_arg' requires an argument
----> Test PASSED - [argparse] [test_optional_long_name_arg_with_default_value]
my_command: option requires an argument -- 'o'
----> Test PASSED - [argparse] [test_optional_short_name_arg_with_default_value]
my_command: option '--optional_arg' requires an argument
----> Test PASSED - [argparse] [test_optional_long_name_arg_with_default_values]
my_command: option requires an argument -- 'o'
----> Test PASSED - [argparse] [test_optional_short_name_arg_with_default_values]
----> Test PASSED - [argparse] [test_flag_long_name_arg]
----> Test PASSED - [argparse] [test_flag_short_name_arg]
----> Test PASSED - [argparse] [test_position_arg]
----> Test PASSED - [argparse] [test_position_arg_you_do_not_know_how_many_beforehand]
----> Test PASSED - [argparse] [test_long_name_conflicts_with_all_flag_arg]
----> Test PASSED - [argparse] [test_short_name_conflicts_with_all_flag_arg]
----> Test PASSED - [argparse] [test_mixed_long_short_name_conflicts_with_all_flag_arg]
----> Test PASSED - [argparse] [test_mixed_short_long_name_conflicts_with_all_flag_arg]
----> Test PASSED - [argparse] [test_long_name_conflicts_with_all_optional_arg]
----> Test PASSED - [argparse] [test_short_name_conflicts_with_all_optional_arg]
----> Test PASSED - [argparse] [test_mixed_long_short_name_conflicts_with_all_optional_arg]
----> Test PASSED - [argparse] [test_mixed_short_long_name_conflicts_with_all_optional_arg]
----> Test PASSED - [argparse] [test_long_name_related_group]
----> Test PASSED - [argparse] [test_short_name_related_group]
----> Test PASSED - [argparse] [test_mixed_long_short_name_related_group]
----> Test PASSED - [argparse] [test_long_name_conflict_group]
----> Test PASSED - [argparse] [test_short_name_conflict_group]
----> Test PASSED - [argparse] [test_mixed_long_short_name_conflict_group]
----> Test PASSED - [argparse] [test_long_name_one_required_group]
----> Test PASSED - [argparse] [test_short_name_one_required_group]
----> Test PASSED - [argparse] [test_mixed_long_short_name_one_required_group]
----> Test PASSED - [argparse] [test_subcommand]

0 tests failed

All test cases have been run completely!
[11:08:22  argparse]#
```

忽略诸如 `my_command: option '--reqarg' requires an argument` 这样的打印。这是测试异常场景时内置参数解析函数 `getopt_long_only` 自己打印的，作者无法控制。毕竟异常场景也是测试场景的一种。

正常情况下，应该都可以通过。

# 6. 接口使用教程

参看单元测试程序 `test/test_argparse.cpp`，里边有各个功能的测试。另外，还可参看源码的中文注释。

`test/test_argparse.cpp`：

```cpp
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
```
