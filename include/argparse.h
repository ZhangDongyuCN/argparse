// 基于 C++17 的一套简单易用的参数解析框架。
//
// 结合了 GitHub 上各种参数解析框架的优缺点，自己实现的一套参数解析框架。
//
// 唯一配置项（根据实际需求进行配置）
// - 如果 `config_exit_when_error == true`，则解析到非法参数时直接退出程序，一般应该用这种模式。
// - 如果 `config_exit_when_error == false`，则解析到非法参数时抛出异常。
//   在交互模式下进行参数解析，可用这种方式，这样就不用退出当前程序，只用捕获异常并处理即可，然后就可以进入下一轮参数解析。
//   本项目的单元测试程序 `test_argparse.cpp`就是用的这种方式。因为单元测试程序本来就要测试异常场景，
//   而如果遇到错误直接退出程序，那么单元测试就无法继续。
//
// 编译环境：
// - Ubuntu 22.04 LTS
// - C++17，gcc 11.4.0，clang 14.0.0
//
// 版本信息：v6.0.1

#ifndef ARGPARSE_HEADER_
#define ARGPARSE_HEADER_

#include <getopt.h>
#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <exception>
#include <iostream>
#include <limits>
#include <map>
#include <memory>
#include <set>
#include <sstream>
#include <string>
#include <string_view>
#include <type_traits>
#include <vector>

namespace zul {  // zul = Zhang Dongyu's utils library.

// `get_one_value` 等函数的模板参数约束
#define ARGPARSE_SUPPORTED_VALUE_TEMPLATE_CONSTRAINTS                                                              \
    template <typename T, typename std::enable_if<                                                                 \
                              std::is_same<int, T>::value || std::is_same<uint32_t, T>::value ||                   \
                                  std::is_same<int64_t, T>::value || std::is_same<uint64_t, T>::value ||           \
                                  std::is_same<float, T>::value || std::is_same<double, T>::value ||               \
                                  std::is_same<T, std::string>::value || std::is_same<T, std::string_view>::value, \
                              int>::type = 0>

// 参数解析框架内部使用，使用者不应该调用
namespace internel {

// 唯一配置项（根据实际需求进行配置）
// - 如果 `config_exit_when_error == true`，则解析到非法参数时直接退出程序，一般应该用这种模式。
// - 如果 `config_exit_when_error == false`，则解析到非法参数时抛出异常。
//   在交互模式下进行参数解析，可用这种方式，这样就不用退出当前程序，只用捕获异常并处理即可，然后就可以进入下一轮参数解析。
//   本项目的单元测试程序 `test_argparse.cpp`就是用的这种方式。因为单元测试程序本来就要测试异常场景，
//   而如果遇到错误直接退出程序，那么单元测试就无法继续。
constexpr bool config_exit_when_error = false;

// 解析失败时根据 `config_exit_when_error` 的值打印错误信息或抛出类型为 `ParseArgsError` 的异常
void exit_or_throw(std::stringstream &error_msg);

// 记录参数解析过程中遇到的错误信息，解析失败时会打印
extern std::stringstream error_msg;

struct CStrCmp {
    bool operator()(const char *s1, const char *s2) const { return strcmp(s1, s2) < 0; }
};

// 将字符串转换为值。由于布尔值的语义比较模糊，因此不支持将字符串转换为布尔值。
// 例如：对于字符串，字符串 `true` 应该被判定为布尔值吗？或者，字符串 `xxx`
// 能被判定为布尔值吗？对于数字，非零值会被判定为布尔值吗？
// 如果你确实想转换为布尔值，可以先转换为整数，然后根据你自己的需求进行判断。
ARGPARSE_SUPPORTED_VALUE_TEMPLATE_CONSTRAINTS
T to_value(const char *str)
{
    if constexpr (std::is_integral_v<T>) {
        if constexpr (std::is_same_v<T, int>) {
            return std::stoi(str);
        } else if constexpr (std::is_same_v<T, int64_t>) {
            return std::stol(str);
        } else if constexpr (std::is_same_v<T, uint32_t>) {
            return static_cast<uint32_t>(std::stoul(str));
        } else if constexpr (std::is_same_v<T, uint64_t>) {
            return std::stoul(str);
        } else {
            static_assert(sizeof(T) == 0, "Unsupported integral type");
        }
    } else if constexpr (std::is_floating_point_v<T>) {
        if constexpr (std::is_same_v<T, float>) {
            return std::stof(str);
        } else if constexpr (std::is_same_v<T, double>) {
            return std::stod(str);
        } else {
            static_assert(sizeof(T) == 0, "Unsupported floating point type");
        }
    } else if constexpr (std::is_same_v<T, std::string>) {
        return str;
    } else if constexpr (std::is_same_v<T, std::string_view>) {
        return str;
    } else {
        static_assert(sizeof(T) == 0, "Unsupported type");
    }
}

}  // namespace internel

class Arg;
class Command;

// 当 `config_exit_when_error == false` 时，参数解析错误时会抛出这个类型的异常
class ParseArgsError : public std::exception {
   public:
    explicit ParseArgsError(const std::string &error_msg) : error_msg_(error_msg) {}
    explicit ParseArgsError(std::string &&error_msg) : error_msg_(std::move(error_msg)) {}
    virtual char const *what() const noexcept override { return error_msg_.c_str(); }

   private:
    std::string error_msg_;
};

// 参数类型
enum class ArgType {
    // 标志参数
    // 例如：`xx.bin --flag`，`--flag` 只是一个标志，没有对应的值
    // 通常情况下，代码会在传递该参数时判定 `--flag` 为 `true`，在未传递时判定 `--flag` 为 `false`，从而控制代码流程
    FLAG,

    // 必选参数（用户必须传递）
    // 例如：`xx.bin --required=50` 或者 `xx.bin --required 50` 或者 `xx.bin -r 50`
    REQUIRED,

    // 可选参数（用户可以不传递）
    // 例如：`xx.bin --optional=100` 或 `xx.bin --optional 100` 或 `xx.bin -o 100`
    OPTIONAL,

    // 位置参数
    // 例如，`xx.bin --required=50 --optional=100 file1 file2`，其中 `file1` 和 `file2` 就是位置参数
    POSITION,
};

// 供 `range` 函数使用，指定数值类型
// 因为底层数据都是用 `const char *` 存储，当进行校验的时候要转换成对应类型的值
enum class NumType { INT, UINT, DOUBlE };

// 供 `range` 函数使用
// 因为底层数据都是 `const char *`，当左右范围需要取一个无限小或无限大数的时候需要用到这些定义
// 数值来源于 `stdint.h` 中的定义，并用 `const char *` 标时
// `_S` 后缀标识字符串 String 的意思
struct NumLimit {
    static constexpr const char *INT32_MAX_S = "2147483647";
    static constexpr const char *INT32_MIN_S = "-2147483648";
    static constexpr const char *UINT32_MAX_S = "4294967295";

    static constexpr const char *INT64_MAX_S = "9223372036854775807";
    static constexpr const char *INT64_MIN_S = "-9223372036854775808";
    static constexpr const char *UINT64_MAX_S = "18446744073709551615";
};

// 用于描述一个参数
class Arg : public std::enable_shared_from_this<Arg> {
   private:
    struct Private {
        explicit Private() = default;
    };

   public:
    friend class Command;

    Arg(Private, ArgType type);
    Arg(Private, const char *long_name, char short_name, ArgType type);
    Arg(const Arg &) = delete;
    Arg(Arg &&) = delete;
    Arg &operator=(const Arg &) = delete;
    Arg &operator=(Arg &&) = delete;

    // 只能使用此工厂函数构造一个新参数
    static std::shared_ptr<Arg> new_arg(ArgType type);

    // 设置参数的长名字
    // 除了位置参数外的其它类型参数，应该至少有一个长名字或短名字
    // 不能手动添加 `--help` 参数，此参数由程序自动添加
    // `name` 不能加前缀的 `--`，例如："file_path" --> OK，"--file_path" --> ERROR
    std::shared_ptr<Arg> long_name(const char *name);

    // 设置参数的短名字
    // 除了位置参数外的其它类型参数，应该至少有一个长名字或短名字
    // 不能手动添加 `-h` 参数，此参数由程序自动添加
    // `name` 不能加前缀的 `-`，例如："f" --> OK，"-f" --> ERROR
    std::shared_ptr<Arg> short_name(char name);

    // 设置此参数跟所有其它参数互斥，即如果用户传递了这个参数，就不能传递其它参数
    // 只有可选参数和标志参数可以设置
    // 参看单元测试用例 `test_long_name_conflicts_with_all_flag_arg`
    std::shared_ptr<Arg> conflicts_with_all();

    // 设置可选参数的默认值，即对于可选参数，如果用户不传递，则它的值就是这个默认值
    // 如果用户传递了，则用用户传递的值覆盖默认值
    // 数据的底层存储方式为 `const char *`，取出时转换成所需的类型（即字符串转数字）
    // 参看单元测试用例 `test_optional_long_name_arg_with_default_value`
    std::shared_ptr<Arg> default_value(const char *value);

    // 设置可选参数的默认值为一个 `vector` 数组，即对于可选参数，如果用户不传递，则它的值就是这个默认值
    // 如果用户传递了，则用用户传递的值覆盖默认值
    // 数据的底层存储方式为 `const char *`，取出时转换成所需的类型（即字符串转数字）
    // 参看单元测试用例 `test_optional_long_name_arg_with_default_values`
    std::shared_ptr<Arg> default_values(std::vector<const char *> &&values);

    // 设置参数的取值范围
    // 例如，你可以指定参数 `--aa` 的范围为 `[1, 100]`。如果实际传递的参数超出了这个范围，将会报错
    // 数据的底层存储方式为 `const char *`，校验时转换成所需的类型（即字符串转数字）
    // 参看单元测试用例 `test_range_required_long_name_arg`
    // 默认 `include_left` 和 `include_right` 都等于 `true`，即包含左区间和右区间：`1 <= aa <=100`
    std::shared_ptr<Arg> range(NumType type, const char *left, const char *right, bool include_left = true,
                               bool include_right = true);

    // 参数的值只能从给定的值中选取。
    // 例如，你可以指定参数 `--aa` 的值只能从 `["1", "2", "3"]` 中选取，如果用户传递了其它值则报错
    // 数据的底层存储方式为 `const char *`，取出时转换成所需的类型（即字符串转数字）
    // 参看单元测试用例 `test_choices_required_long_name_arg`
    std::shared_ptr<Arg> choices(std::vector<const char *> &&choices);

   private:
    static std::shared_ptr<Arg> new_help_arg();
    const char *get_long();
    char get_short();
    const std::vector<const char *> &get_values();
    int get_argid();
    int get_position_id();
    ArgType get_arg_type();
    std::string get_choice_description();
    std::string get_boundary_description();

    void set_command(Command *command);
    void set_value(const char *val);
    void set_hit();
    void set_argid(int opt_id);
    void set_position_id(int position_id);

    bool is_hit();
    bool is_conflict_with_all();

    void check_range();
    void check_choice();

    void reset_status_info();

    template <typename T>
    bool check_range(T value, T left, T right)
    {
        if (include_left_) {
            if (value < left) {
                return false;
            }
        } else {
            if (value <= left) {
                return false;
            }
        }
        if (include_right_) {
            if (value > right) {
                return false;
            }
        } else {
            if (value >= right) {
                return false;
            }
        }
        return true;
    }

   private:
    const char *long_name_ = nullptr;  // 参数的长名字
    char short_name_ = ' ';            // 参数的短名字
    int arg_id_ = INT32_MIN;           // 参数的唯一标识 ID
    int position_id_ = -1;  // 如果是位置参数，它用于标识位置索引（有效的索引是从 0 开始，依次递增）
    ArgType arg_type_ = ArgType::REQUIRED;  // 参数类型

    // 标识该参数是否被用户传递，如果用户传递了这个参数，则命中，否则未命中
    // 只要是针对可选参数和标志参数
    // 例如：可选参数用户可以传递，也可以不传递，传递时就为 `true`，否则就为 `false`
    // 诸如 `check_related_group` 等函数要使用此值
    bool is_hit_ = false;

    // 参数的值。参数可以有多个值，这些值会统一存储在一个字符串中，然后在取出时再转换为实际类型
    std::vector<const char *> values_;
    // 参数的默认值，通过 `default_value` 或 `default_values` 设置
    std::vector<const char *> default_values_;
    bool has_default_value_ = false;
    // 默认值是否被清空
    // 如果用户指定了默认值，则 `values_` 的初始值为默认值，
    // 此时，如果用户又传递了此参数，则使用用户传递的值覆盖此值，同时该变量置为 `false`
    bool is_default_value_cleared_ = false;

    // 通过 `range` 或 `choices` 设置的值的取值范围，二者为互斥关系，不能同时存在
    const char *left_ = nullptr;
    const char *right_ = nullptr;
    bool include_left_ = true;
    bool include_right_ = true;
    NumType num_type_ = NumType::INT;
    bool is_range_ = false;  // 用户是否使用 `range` 设置了值的取值范围
    std::set<const char *, internel::CStrCmp> choices_;
    bool is_choice_ = false;  // 用户是否使用 `choices` 设置了值的取值范围

    // 标识此参数是否跟所有其它参数互斥
    bool is_conflict_with_all_ = false;

    // 标识此参数属于哪一个命令
    Command *command_ = nullptr;
};

// 标识一个命令（Command 是 Arg 的集合，父子 Command 通过 map 链接）
class Command : public std::enable_shared_from_this<Command> {
   private:
    struct Private {
        explicit Private() = default;
    };

   public:
    Command(Private){};
    Command(const Command &) = delete;
    Command(Command &&) = delete;
    Command &operator=(const Command &) = delete;
    Command &operator=(Command &&) = delete;

    // 只能使用此工厂函数构造一个命令
    static std::shared_ptr<Command> new_command(const char *name);

    // 设置命令的使用说明
    // 一般的参数解析框架都会自动生成帮助文档，优点很明显，不用用户自己写。缺点是框架自动生成，想要添加自定义内容很难。
    // 作者衡量后，决定自己写帮助文档，不用框架生成，更灵活。
    //    const char* usage_help =
    //    "NAME\n"
    //    "    iotime - Test the internal bandwidth of the file system\n"
    //    "SYNOPSIS\n"
    //    "    iotime [OPTION]\n"
    //    "OPTION\n"
    //    "    -f, --file\n"
    //    "        The file to read / write\n"
    //    "    -a, --action\n"
    //    "        0: for write test\n"
    //    "        1: for read test\n"
    //    "    -t, --total\n"
    //    "        Total size (bytes) to read / write\n"
    //    "    -b, --bufsize\n"
    //    "        The buffer size (bytes) of each read / write action";
    //    auto cmd = Command::new_command("my_command")->usage(usage_help);
    std::shared_ptr<Command> usage(const char *usage);

    //    const char *usage_help[] = {
    //        "    Usage: iozone [-s filesize_kB] [-r record_size_kB] [-f [path]filename] [-h]",
    //        "                  [-i test] [-E] [-p] [-a] [-A] [-z] [-Z] [-m] [-M] [-t children]",
    //        "                  [-l min_number_procs] [-u max_number_procs] [-v] [-R] [-x] [-o]"};
    //    auto cmd = Command::new_command("my_command")->usage(usage_help, sizeof(usage_help) / sizeof(usage_help[0]));
    std::shared_ptr<Command> usage(const char *usage[], size_t size);

    // 添加一个新参数到命令中
    std::shared_ptr<Command> arg(std::shared_ptr<Arg> arg);

    // 确保多个可选参数或标志参数必须同时传递，或同时不传递
    // 参看单元测试用例 `test_long_name_related_group`
    std::shared_ptr<Command> related_group(std::vector<const char *> &&related_group);

    // 确保多个可选参数或标志参数之间不能共存，直能同时存在一个
    // 参看单元测试用例 `test_long_name_conflict_group`
    std::shared_ptr<Command> conflict_group(std::vector<const char *> &&conflict_group);

    // 确保多个可选参数或标志参数中至少有一个被传递
    // 参看单元测试用例 `test_long_name_one_required_group`
    std::shared_ptr<Command> one_required_group(std::vector<const char *> &&one_required_group);

    // 子命令相关
    // 参看单元测试用例 `test_subcommand`
    // 添加一个子命令
    std::shared_ptr<Command> subcommand(std::shared_ptr<Command> subcommand);
    // 获取当前实际的子命令
    const std::shared_ptr<Command> get_subcommand();
    // 此命令的名字
    std::string command_name();
    std::string_view command_name_sv();

    // 测试用户是否传递了参数，一般用于判断用户是否传递了标志参数，进而控制代码流程
    // 例如：`xx.bin --flag`，`--flag` 只是一个标志，没有对应的值
    // 通常情况下，代码会在传递该参数时判定 `--flag` 为 `true`，在未传递时判定 `--flag` 为 `false`，从而控制代码流程
    bool has_arg(const char *long_name);
    bool has_arg(char short_name);

    // 运行参数解析
    // 参数解析期间会进行各种预设条件的校验，如果不满足则出错
    void parse_args(int argc, char **argv);
    void parse_args(std::vector<const char *> &&args);

    // 根据参数的长名字获取参数的值
    // 由于布尔值的语义相对模糊，因此不支持将字符串转换为布尔值。
    // 例如：对于字符串而言，字符串 `true` 应该被判定为布尔值吗？或者，字符串 `xxx`
    // 能被判定为布尔值吗？对于数字，非零值应该被判定为布尔值吗？
    // 如果你确实想将其转换为布尔值，可以先将其转换为整数，然后再根据你自己的需求进行判断。
    ARGPARSE_SUPPORTED_VALUE_TEMPLATE_CONSTRAINTS
    T get_one_value(const char *long_name)
    {
        if (longname_2_arg_.count(long_name) == 0) {
            internel::error_msg << "Can not find --" << long_name << " option.";
            internel::exit_or_throw(internel::error_msg);
        }
        const std::shared_ptr<Arg> &arg = longname_2_arg_.at(long_name);
        if (arg->get_values().empty()) {
            internel::error_msg << "Option --" << long_name << " does not have a value.";
            internel::exit_or_throw(internel::error_msg);
        }
        return internel::to_value<T>(arg->get_values().at(0));
    }

    // 根据参数的短名字获取参数的值
    ARGPARSE_SUPPORTED_VALUE_TEMPLATE_CONSTRAINTS
    T get_one_value(char short_name)
    {
        if (shortname_2_arg_.count(short_name) == 0) {
            internel::error_msg << "Can not find -" << short_name << " option.";
            internel::exit_or_throw(internel::error_msg);
        }
        const std::shared_ptr<Arg> &arg = shortname_2_arg_.at(short_name);
        if (arg->get_values().empty()) {
            internel::error_msg << "Option -" << short_name << " does not have a value.";
            internel::exit_or_throw(internel::error_msg);
        }
        return internel::to_value<T>(arg->get_values().at(0));
    }

    // 根据参数的长名字获取参数的多个值（一个 `vector` 类型的数组）
    // 如何为参数传递多个值？参看单元测试用例 `test_optional_long_name_arg_with_default_values`
    // auto cmd = Command::new_command("my_command")
    //         ->arg(Arg::new_arg(ArgType::OPTIONAL)->long_name("optional_arg"));
    // cmd->parse_args({"my_command", "--optional_arg", "1", "--optional_arg", "2", "--optional_arg", "3"});
    // ZUL_CHECK_ARRAY_EQ(cmd->get_many_values<int>("optional_arg"), (std::vector<int>{1, 2, 3}));
    // 即，需要多次指定 `--optional_arg` 传递不同的值，这些值会被追加到一个数组中存储，后边的值不会覆盖前边的值
    ARGPARSE_SUPPORTED_VALUE_TEMPLATE_CONSTRAINTS
    std::vector<T> get_many_values(const char *long_name)
    {
        if (longname_2_arg_.count(long_name) == 0) {
            internel::error_msg << "Can not find --" << long_name << " option.";
            internel::exit_or_throw(internel::error_msg);
        }
        const std::shared_ptr<Arg> &arg = longname_2_arg_.at(long_name);
        std::vector<T> values;
        values.resize(arg->get_values().size());
        std::transform(arg->get_values().cbegin(), arg->get_values().cend(), values.begin(),
                       [](const char *value) { return internel::to_value<T>(value); });
        return values;
    }

    // 根据参数的短名字获取参数的多个值（一个 `vector` 类型的数组）
    // 如何为参数传递多个值？参看单元测试用例 `test_optional_short_name_arg_with_default_values`
    ARGPARSE_SUPPORTED_VALUE_TEMPLATE_CONSTRAINTS
    std::vector<T> get_many_values(char short_name)
    {
        if (shortname_2_arg_.count(short_name) == 0) {
            internel::error_msg << "Can not find -" << short_name << " option.";
            internel::exit_or_throw(internel::error_msg);
        }
        const std::shared_ptr<Arg> &arg = shortname_2_arg_.at(short_name);
        std::vector<T> values;
        values.resize(arg->get_values().size());
        std::transform(arg->get_values().cbegin(), arg->get_values().cend(), values.begin(),
                       [](const char *value) { return internel::to_value<T>(value); });
        return values;
    }

    // 获取位置参数的值，`position` 指定位置，从 0 开始
    // 参看单元测试用例 `test_position_arg`
    ARGPARSE_SUPPORTED_VALUE_TEMPLATE_CONSTRAINTS
    T get_one_position_value(size_t position)
    {
        if (position >= position_values_.size()) {
            internel::error_msg << "No corresponding position argument.";
            internel::exit_or_throw(internel::error_msg);
        }
        return internel::to_value<T>(position_values_.at(position));
    }

    // 获取所有位置参数的值，仅限期望的位置参数属于同一类型，例如可以传递多个位置参数的文件路径
    // 参看单元测试用例 `test_position_arg`
    ARGPARSE_SUPPORTED_VALUE_TEMPLATE_CONSTRAINTS
    std::vector<T> get_all_position_values()
    {
        std::vector<T> values;
        values.resize(position_values_.size());
        std::transform(position_values_.begin(), position_values_.end(), values.begin(),
                       [](const char *value) { return internel::to_value<T>(value); });
        return values;
    }

   private:
    void update_c_long_args(std::shared_ptr<Arg> opt);
    void update_c_short_args(std::shared_ptr<Arg> opt);

    void do_parse_args(int argc, char **argv);
    void do_parse_args_internel(int argc, char **argv);

    void check_required_args();
    void check_conflict_with_all_args();
    void check_related_groups();
    void check_conflict_groups();
    void check_one_required_group();

    std::string get_description(std::vector<const char *> &group);

    void add_help_arg();
    void print_usage_help();

    void reset_arg_status();

    // 用于标识参数的唯一 ID，这由 `getopt_long` 的特性决定。`getopt_long_only` 函数在匹配短参数时会返回一个正值。
    // 这里将其用作长参数的唯一标识符，为了将其与短参数区分开来，这里使用负值。
    // `getopt_long_only` 函数返回 `-1` 表示出错，所以这里从 `-2` 开始。
    int current_argid_ = -2;

    // 记录当前最新分配的位置参数的索引
    int current_position_id_ = 0;

    // 记录长参数名称、短参数名称、参数 ID 与 `Arg` 的映射关系
    std::map<const char *, std::shared_ptr<Arg>, internel::CStrCmp> longname_2_arg_;
    std::map<char, std::shared_ptr<Arg>> shortname_2_arg_;
    std::map<int, std::shared_ptr<Arg>> argid_2_arg_;

    // 记录所有已设置与其它所有参数冲突的参数的集合
    std::vector<std::shared_ptr<Arg>> conflict_with_all_args_;
    // 记录参数关联组、互斥组、至少选其一组
    std::vector<std::vector<const char *>> related_groups_, conflict_groups_, one_required_groups_;

    // 记录所有位置参数的集合
    std::vector<std::shared_ptr<Arg>> position_args_;
    // 记录所有位置参数值的集合
    std::vector<const char *> position_values_;

    // 用于底层 `getopt_long_only` 解析时使用的数据
    std::string c_short_opts_;
    std::vector<option> c_long_opts_;

    // 此命令的名字
    const char *command_name_ = nullptr;

    // 此命令的帮助文档（对应两种格式）
    const char *usage_format1_ = nullptr;
    const char **usage_format2_ = nullptr;
    size_t line_size_ = 0;

    // 此命令的子命令
    std::map<const char *, std::shared_ptr<Command>, internel::CStrCmp> subcommandname_2_subcommand_;
    char *current_subcommand_name_ = nullptr;
};

}  // namespace zul

#endif  // ARGPARSE_HEADER_
