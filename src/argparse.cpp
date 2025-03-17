#include "argparse.h"
#include <cerrno>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string_view>
#include <utility>

namespace zul {  // zul = Zhang Dongyu's utils library.

namespace internel {

std::stringstream error_msg;

void exit_or_throw(std::stringstream &error_msg)
{
    if (config_exit_when_error) {
        if (error_msg.str().length()) {
            std::cerr << error_msg.str() << std::endl;
        }
        exit(1);
    } else {
        std::string the_err_msg(error_msg.str());
        error_msg = std::stringstream();
        throw ParseArgsError(std::move(the_err_msg));
    }
}

}  // namespace internel

Arg::Arg(Private, ArgType type) { arg_type_ = type; }

Arg::Arg(Private, const char *long_name, char short_name, ArgType type)
{
    long_name_ = long_name;
    short_name_ = short_name;
    arg_type_ = type;
};

std::shared_ptr<Arg> Arg::new_arg(ArgType type) { return std::make_shared<Arg>(Private(), type); }

std::shared_ptr<Arg> Arg::long_name(const char *name)
{
    if (arg_type_ == ArgType::POSITION) {
        internel::error_msg << "Position argument can not set long name.";
        internel::exit_or_throw(internel::error_msg);
    }
    if (strncmp(name, "help", 4) == 0) {
        internel::error_msg << "The option of --help and -h have been automatically added. Just run: xx.bin "
                               "--help or xx.bin -h "
                               "to show the usage help.";
        internel::exit_or_throw(internel::error_msg);
    }
    if (strlen(name) < 2) {
        internel::error_msg << "The length of long option must be greater than 2.";
        internel::exit_or_throw(internel::error_msg);
    }
    if (strncmp(name, "-", 1) == 0) {
        internel::error_msg << "The name cannot start with -- or - .";
        internel::exit_or_throw(internel::error_msg);
    }
    if (std::any_of(name, name + strlen(name), [](const char elem) { return elem == ' '; })) {
        internel::error_msg << "The long option can not contain spaces.";
        internel::exit_or_throw(internel::error_msg);
    }
    long_name_ = name;
    return shared_from_this();
}

std::shared_ptr<Arg> Arg::short_name(char name)
{
    if (arg_type_ == ArgType::POSITION) {
        internel::error_msg << "Position argument can not set short name.";
        internel::exit_or_throw(internel::error_msg);
    }
    if (name == 'h') {
        internel::error_msg << "The option of --help and -h have been automatically added. Just run: xx.bin "
                               "--help or xx.bin -h "
                               "to show the usage help.";
        internel::exit_or_throw(internel::error_msg);
    }
    if (name == ' ') {
        internel::error_msg << "The short option can not be a space.";
        internel::exit_or_throw(internel::error_msg);
    }
    short_name_ = name;

    return shared_from_this();
}

std::shared_ptr<Arg> Arg::conflicts_with_all()
{
    if (arg_type_ == ArgType::REQUIRED || arg_type_ == ArgType::POSITION) {
        internel::error_msg << "The required argument or position argument can not set related options.";
        internel::exit_or_throw(internel::error_msg);
    }
    is_conflict_with_all_ = true;
    return shared_from_this();
}

std::shared_ptr<Arg> Arg::default_value(const char *value)
{
    if (arg_type_ != ArgType::OPTIONAL) {
        internel::error_msg << "Only optional argument can set default value.";
        internel::exit_or_throw(internel::error_msg);
    }
    values_.push_back(value);
    default_values_.push_back(std::move(value));
    has_default_value_ = true;
    check_range();
    check_choice();
    return shared_from_this();
}

std::shared_ptr<Arg> Arg::default_values(std::vector<const char *> &&values)
{
    if (arg_type_ != ArgType::OPTIONAL) {
        internel::error_msg << "Only optional argument can set default values.";
        internel::exit_or_throw(internel::error_msg);
    }
    if (values.empty()) {
        internel::error_msg << "The default values can not empty.";
        internel::exit_or_throw(internel::error_msg);
    }
    values_ = values;
    default_values_ = std::move(values);
    has_default_value_ = true;
    check_range();
    check_choice();
    return shared_from_this();
}

std::shared_ptr<Arg> Arg::range(NumType type, const char *left, const char *right, bool include_left,
                                bool include_right)
{
    if (arg_type_ == ArgType::FLAG) {
        internel::error_msg << "The flag option can not set value range.";
        internel::exit_or_throw(internel::error_msg);
    }
    if (is_choice_) {
        internel::error_msg << "The selection value has been set for the option, and the range value can not be "
                               "set again.";
        internel::exit_or_throw(internel::error_msg);
    }
    left_ = left;
    right_ = right;
    include_left_ = include_left;
    include_right_ = include_right;
    num_type_ = type;
    is_range_ = true;
    return shared_from_this();
}

std::shared_ptr<Arg> Arg::choices(std::vector<const char *> &&choices)
{
    if (arg_type_ == ArgType::FLAG) {
        internel::error_msg << "The flag option can not set value choices.";
        internel::exit_or_throw(internel::error_msg);
    }
    if (is_range_) {
        internel::error_msg << "The range value has been set for the option, and the selection value can not be "
                               "set again.";
        internel::exit_or_throw(internel::error_msg);
    }
    if (choices.empty()) {
        internel::error_msg << "The value choices vector can not empty.";
        internel::exit_or_throw(internel::error_msg);
    }
    choices_ = std::set<const char *, internel::CStrCmp>(choices.begin(), choices.end());
    if (choices_.empty()) {
        internel::error_msg << "The value choices set can not empty.";
        internel::exit_or_throw(internel::error_msg);
    }
    is_choice_ = true;
    return shared_from_this();
}

void Arg::set_command(Command *command) { command_ = command; }

void Arg::set_hit() { is_hit_ = true; }

void Arg::set_position_id(int position_id) { position_id_ = position_id; }

void Arg::set_value(const char *value)
{
    if (arg_type_ == ArgType::FLAG) {
        if (values_.empty()) {
            values_.resize(1);
        }
        values_.at(0) = value;
    } else {
        // 如果用户传递了，则用传递的值覆盖默认值
        if (has_default_value_ && !is_default_value_cleared_) {
            values_.clear();
            is_default_value_cleared_ = true;
        }
        // 存储用户传递的参数值并进行预设规则校验
        values_.push_back(std::move(value));
        check_range();
        check_choice();
    }
}

void Arg::reset_status_info()
{
    is_hit_ = false;
    if (has_default_value_) {
        values_ = default_values_;
        is_default_value_cleared_ = false;
    }
}

std::shared_ptr<Arg> Arg::new_help_arg() { return std::make_shared<Arg>(Private(), "help", 'h', ArgType::FLAG); }

const char *Arg::get_long() { return long_name_; }

char Arg::get_short() { return short_name_; }

int Arg::get_position_id() { return position_id_; }

void Arg::set_argid(int opt_id) { arg_id_ = opt_id; }

int Arg::get_argid() { return arg_id_; }

ArgType Arg::get_arg_type() { return arg_type_; }

const std::vector<const char *> &Arg::get_values() { return values_; }

std::string Arg::get_choice_description()
{
    std::string description;
    description.reserve(100);
    description.push_back('[');
    for (const auto &elem : choices_) {
        description.append(elem).append(", ");
    }
    description.pop_back();
    description.pop_back();
    description.push_back(']');
    return description;
}

std::string Arg::get_boundary_description()
{
    std::string description;
    description.reserve(100);
    if (include_left_) {
        description.push_back('[');
    } else {
        description.push_back('(');
    }
    description.append(left_).append(", ").append(right_);
    if (include_right_) {
        description.push_back(']');
    } else {
        description.push_back(')');
    }
    return description;
}

bool Arg::is_hit() { return is_hit_; }

bool Arg::is_conflict_with_all() { return is_conflict_with_all_; }

void Arg::check_range()
{
    if (is_range_) {
        bool check_ret = false;
        if (num_type_ == NumType::INT) {
            int64_t value = internel::to_value<int64_t>(values_.back());
            int64_t left = internel::to_value<int64_t>(left_);
            int64_t right = internel::to_value<int64_t>(right_);
            check_ret = check_range(value, left, right);
        } else if (num_type_ == NumType::UINT) {
            uint64_t value = internel::to_value<uint64_t>(values_.back());
            uint64_t left = internel::to_value<uint64_t>(left_);
            uint64_t right = internel::to_value<uint64_t>(right_);
            check_ret = check_range(value, left, right);
        } else if (num_type_ == NumType::DOUBlE) {
            double value = internel::to_value<double>(values_.back());
            double left = internel::to_value<double>(left_);
            double right = internel::to_value<double>(right_);
            check_ret = check_range(value, left, right);
        } else {
            internel::error_msg << "Unknown range type.";
            internel::exit_or_throw(internel::error_msg);
        }

        if (!check_ret) {
            if (get_position_id() != -1) {
                internel::error_msg << "The value of position argument (position index " << get_position_id()
                                    << ") is not within the range of " << get_boundary_description() << ".";
            } else if (get_long()) {
                internel::error_msg << "The value of option --" << get_long() << " is not in the range of "
                                    << get_boundary_description() << ".";
            } else if (get_short() != ' ') {
                internel::error_msg << "The value of option -" << get_short() << " is not in the range of "
                                    << get_boundary_description() << ".";
            } else {
                internel::error_msg << "A argument must have at least a long name or a short name or a position id.";
            }
            internel::exit_or_throw(internel::error_msg);
        }
    }
}

void Arg::check_choice()
{
    if (is_choice_) {
        if (choices_.find(values_.back()) == choices_.end()) {
            if (get_position_id() != -1) {
                internel::error_msg << "The value of position argument (position index " << get_position_id()
                                    << ") is not within " << get_choice_description() << ".";
                internel::exit_or_throw(internel::error_msg);
            } else {
                internel::error_msg << "The value of option --" << get_long() << " is not within "
                                    << get_choice_description() << ".";
                internel::exit_or_throw(internel::error_msg);
            }
        }
    }
}

std::shared_ptr<Command> Command::new_command(const char *name)
{
    auto command = std::make_shared<Command>(Private());
    command->command_name_ = name;
    return command;
}

std::shared_ptr<Command> Command::usage(const char *usage)
{
    usage_format1_ = usage;
    return shared_from_this();
}

std::shared_ptr<Command> Command::usage(const char *usage[], size_t size)
{
    usage_format2_ = usage;
    line_size_ = size;
    return shared_from_this();
}

std::shared_ptr<Command> Command::arg(std::shared_ptr<Arg> arg)
{
    if (arg->get_arg_type() != ArgType::POSITION && arg->get_long() == nullptr && arg->get_short() == ' ') {
        internel::error_msg << "The argument should have a long name or a short name.";
        internel::exit_or_throw(internel::error_msg);
    }

    int argid = current_argid_--;
    arg->set_argid(argid);
    arg->set_command(this);

    if (arg->get_long()) {
        longname_2_arg_[arg->get_long()] = arg;
        update_c_long_args(arg);
    }

    if (arg->get_short()) {
        shortname_2_arg_[arg->get_short()] = arg;
        update_c_short_args(arg);
    }

    argid_2_arg_[argid] = arg;

    if (arg->is_conflict_with_all()) {
        conflict_with_all_args_.push_back(arg);
    }

    // 如果是标志参数，其初始值被设为 0，也就是 `false`。若用户在参数解析过程中传递了该参数，就会将其设为
    // 1，即 `true`。
    if (arg->get_arg_type() == ArgType::FLAG) {
        arg->set_value("0");
    }

    // 如果是位置参数，则给位置参数分配标识位置的 ID（从 0 开始，依次递增），并记录位置参数
    if (arg->get_arg_type() == ArgType::POSITION) {
        arg->set_position_id(current_position_id_++);
        position_args_.push_back(arg);
    }

    return shared_from_this();
}

std::shared_ptr<Command> Command::subcommand(std::shared_ptr<Command> subcommand)
{
    subcommandname_2_subcommand_[subcommand->command_name_] = subcommand;
    return shared_from_this();
}

std::shared_ptr<Command> Command::related_group(std::vector<const char *> &&related_group)
{
    related_groups_.push_back(std::move(related_group));
    return shared_from_this();
}

std::shared_ptr<Command> Command::conflict_group(std::vector<const char *> &&conflict_group)
{
    conflict_groups_.push_back(std::move(conflict_group));
    return shared_from_this();
}

std::shared_ptr<Command> Command::one_required_group(std::vector<const char *> &&one_required_group)
{
    one_required_groups_.push_back(std::move(one_required_group));
    return shared_from_this();
}

const std::shared_ptr<Command> Command::get_subcommand()
{
    return subcommandname_2_subcommand_.at(current_subcommand_name_);
}

std::string Command::command_name() { return command_name_; }

std::string_view Command::command_name_sv() { return command_name_; }

void Command::parse_args(int argc, char **argv)
{
    reset_arg_status();
    do_parse_args(argc, argv);
}

void Command::parse_args(std::vector<const char *> &&args)
{
    reset_arg_status();
    int argc = static_cast<int>(args.size());
    char *argv[argc];
    for (int i = 0; i < argc; i++) {
        argv[i] = const_cast<char *>(args[i]);
    }
    do_parse_args(argc, argv);
}

bool Command::has_arg(const char *long_name)
{
    if (longname_2_arg_.count(long_name) == 0) {
        return false;
    }
    return longname_2_arg_.at(long_name)->is_hit();
}

bool Command::has_arg(char short_name)
{
    if (shortname_2_arg_.count(short_name) == 0) {
        return false;
    }
    return shortname_2_arg_.at(short_name)->is_hit();
}

void Command::update_c_long_args(std::shared_ptr<Arg> arg)
{
    // 如果一个参数被传递，那么该参数的值也应该被传递。因此在这里，除了标志参数外的其它类型参数都被设置为
    // `required_argument`（即需要传递值）
    c_long_opts_.emplace_back(option{arg->get_long(),
                                     arg->get_arg_type() == ArgType::FLAG ? no_argument : required_argument, NULL,
                                     arg->get_short() != ' ' ? arg->get_short() : arg->get_argid()});
}

void Command::update_c_short_args(std::shared_ptr<Arg> arg)
{
    // 如果一个参数被传递，那么该参数的值也应该被传递。因此在这里，除了标志参数外的其它类型参数都被设置为
    // `required_argument`（即需要传递值）
    if (arg->get_arg_type() == ArgType::FLAG) {
        c_short_opts_.push_back(arg->get_short());
    } else {
        c_short_opts_.push_back(arg->get_short());
        c_short_opts_.push_back(':');
    }
}

void Command::reset_arg_status()
{
    // 可以在主函数中多次调用 `parse_args`，但是需要清除上次调用 `parse_args` 设置的一些信息
    for (auto &iter : longname_2_arg_) {
        iter.second->reset_status_info();
    }

    for (auto &iter : shortname_2_arg_) {
        iter.second->reset_status_info();
    }

    position_values_.clear();

    internel::error_msg = std::stringstream();
}

void Command::do_parse_args(int argc, char **argv)
{
    if (subcommandname_2_subcommand_.empty()) {
        add_help_arg();
        c_long_opts_.emplace_back(option{NULL, 0, NULL, 0});
        do_parse_args_internel(argc, argv);
        check_conflict_with_all_args();
        check_related_groups();
        check_conflict_groups();
        check_one_required_group();
    } else {
        // 从 `argv` 中查找子命令对应的索引
        int idx = 0;
        for (; idx < argc; idx++) {
            if (subcommandname_2_subcommand_.count(argv[idx]) == 1) {
                break;
            }
        }

        // 首先，解析当前层级的参数（即父命令的参数）
        add_help_arg();
        c_long_opts_.emplace_back(option{NULL, 0, NULL, 0});
        do_parse_args_internel(idx, argv);
        check_conflict_with_all_args();
        check_related_groups();
        check_conflict_groups();
        check_one_required_group();

        // 然后判断是否找到了子命令
        // 这涉及到出错时信息显示顺序的问题。采用这种方式，会先显示父命令的错误信息，然后再显示子命令的错误信息。
        if (idx == argc) {
            internel::error_msg << command_name_ << ": Missing subcommand.";
            internel::exit_or_throw(internel::error_msg);
        }

        current_subcommand_name_ = argv[idx];

        // 进而，解析下一层级的参数（即子命令的参数）
        subcommandname_2_subcommand_.at(argv[idx])->do_parse_args(argc - idx, argv + idx);
    }
}

void Command::do_parse_args_internel(int argc, char **argv)
{
    // 注意，必须显式地将 `optind` 设置为 1，因为函数 `getopt_long_only` 在解析之前要求 `optind ==1`。
    // 因为当前支持子命令，并且 `optind` 是一个全局变量，当解析父命令时，`optind` 的值已经被更新了。
    // 因此，当解析子命令时，`optind` 不再是 1，这将导致解析错误。
    optind = 1;
    int ret = 0;
    int long_index = 0;
    while ((ret = getopt_long_only(argc, argv, c_short_opts_.c_str(), c_long_opts_.data(), &long_index)) != -1) {
        std::shared_ptr<Arg> arg;
        if (ret < 0) {  // 返回负数表示解析遇到了长参数，参看 `current_argid_` 和 `update_c_long_args` 函数
            if (ret == longname_2_arg_.at("help")->get_argid()) {
                print_usage_help();
            }
            arg = argid_2_arg_.at(ret);  // 正常情况下，这里不应该抛出异常
        } else {  //  返回正数表示解析遇到了短参数，参看 `current_argid_` 和 `update_c_short_args` 函数
            if (ret == '?' || ret == ':') {
                internel::error_msg << command_name_ << ": Function `getopt_long_only` returned an error.";
                internel::exit_or_throw(internel::error_msg);
            }
            if (ret == 'h') {
                print_usage_help();
            }
            arg = shortname_2_arg_.at(static_cast<char>(ret));  // 正常情况下，这里不应该抛出异常
        }
        arg->set_hit();
        if (optarg) {
            arg->set_value(optarg);
        } else {
            arg->set_value("1");
        }
    }
    // 验证所有必选参数是否都已被传递
    check_required_args();

    // 解析位置参数
    argc -= optind;
    argv += optind;
    if (static_cast<size_t>(argc) < position_args_.size()) {
        internel::error_msg << command_name_ << ": Missing required position arguments.";
        internel::exit_or_throw(internel::error_msg);
    }
    for (size_t i = 0; i < static_cast<size_t>(argc); i++) {
        position_values_.emplace_back(argv[i]);
        // 检查在命令中显式设置的位置参数，而不检查其它未显式设置的位置参数（用户可能仅设置了 3
        // 个位置参数，但是传递了大于 3 个的位置参数）
        if (i < position_args_.size()) {
            position_args_.at(i)->set_value(argv[i]);
        }
    }
}

void Command::check_required_args()
{
    for (const auto &iter : longname_2_arg_) {
        const std::shared_ptr<Arg> &arg = iter.second;
        if (arg->get_arg_type() == ArgType::REQUIRED && !arg->is_hit()) {
            internel::error_msg << command_name_ << ": Missing required option: --" << arg->get_long() << ".";
            internel::exit_or_throw(internel::error_msg);
        }
    }

    for (const auto &iter : shortname_2_arg_) {
        const std::shared_ptr<Arg> &arg = iter.second;
        if (arg->get_arg_type() == ArgType::REQUIRED && !arg->is_hit()) {
            internel::error_msg << command_name_ << ": Missing required option: -" << arg->get_short() << ".";
            internel::exit_or_throw(internel::error_msg);
        }
    }
}

void Command::check_conflict_with_all_args()
{
    for (const auto &arg : conflict_with_all_args_) {
        if (arg->is_hit()) {
            // 如果这个参数与所有其它参数都冲突，那么不能传递其它任何参数
            for (const auto &iter : longname_2_arg_) {
                const std::shared_ptr<Arg> &arg2 = iter.second;
                if (arg->get_long() == arg2->get_long()) {
                    continue;
                } else {
                    if (arg2->is_hit()) {
                        internel::error_msg << command_name_
                                            << ": The conflict relationship is not satisfied. Option --"
                                            << arg->get_long() << " is conflict with all other options.";
                        internel::exit_or_throw(internel::error_msg);
                    }
                }
            }

            for (const auto &iter : shortname_2_arg_) {
                const std::shared_ptr<Arg> &arg2 = iter.second;
                if (arg->get_short() == arg2->get_short()) {
                    continue;
                } else {
                    if (arg2->is_hit()) {
                        internel::error_msg << command_name_ << ": The conflict relationship is not satisfied. Option -"
                                            << arg->get_short() << " is conflict with all other options.";
                        internel::exit_or_throw(internel::error_msg);
                    }
                }
            }
        }
    }
}

void Command::check_related_groups()
{
    size_t idx = 0;
    for (const auto &group : related_groups_) {
        // 相关组用于确保指定的参数必须同时存在或同时不存在，即 `count == 0` 或 `count == related_group.size()`。
        // 否则，相关组的要求必定无法满足。这里的 `count` 是指相关组中实际传递的参数数量（is_hit() == true），
        // `related_group.size()` 是相关组中参数的总数。意思是在解析命令行参数时，相关组里的参数要么一个都不传递，
        // 要么全部传递，不然就不符合相关组的规则。
        size_t count = std::count_if(group.begin(), group.end(), [this](const char *name) {
            if (strlen(name) == 1) {
                if (shortname_2_arg_.count(name[0]) == 0) {
                    internel::error_msg << command_name_ << ": Can not find -" << name << " option.";
                    internel::exit_or_throw(internel::error_msg);
                }
                return shortname_2_arg_.at(name[0])->is_hit();
            } else {
                if (longname_2_arg_.count(name) == 0) {
                    internel::error_msg << command_name_ << ": Can not find --" << name << " option.";
                    internel::exit_or_throw(internel::error_msg);
                }
                return longname_2_arg_.at(name)->is_hit();
            }
        });
        if (count != 0 && count != group.size()) {
            internel::error_msg << command_name_ << ": The related relationship is not satisfied. "
                                << get_description(related_groups_.at(idx)) << ": is related with each other.";
            internel::exit_or_throw(internel::error_msg);
        }
        idx++;
    }
}

void Command::check_conflict_groups()
{
    size_t idx = 0;
    for (const auto &group : conflict_groups_) {
        // 冲突组确保其中最多只能有一个参数被传递，即 `count <= 1`。如果 `count > 1`，则必定不满足冲突组的要求。
        size_t count = std::count_if(group.begin(), group.end(), [this](const char *name) {
            if (strlen(name) == 1) {
                if (shortname_2_arg_.count(name[0]) == 0) {
                    internel::error_msg << command_name_ << ": Can not find -" << name << " option.";
                    internel::exit_or_throw(internel::error_msg);
                }
                return shortname_2_arg_.at(name[0])->is_hit();
            } else {
                if (longname_2_arg_.count(name) == 0) {
                    internel::error_msg << command_name_ << ": Can not find --" << name << " option.";
                    internel::exit_or_throw(internel::error_msg);
                }
                return longname_2_arg_.at(name)->is_hit();
            }
        });
        if (count > 1) {
            internel::error_msg << command_name_ << ": The conflict relationship is not satisfied. "
                                << get_description(conflict_groups_.at(idx)) << ": is conflict with each other.";
            internel::exit_or_throw(internel::error_msg);
        }
        idx++;
    }
}

void Command::check_one_required_group()
{
    size_t idx = 0;
    for (const auto &group : one_required_groups_) {
        // 至少选其一组确保该组中至少有一个参数存在，即 `count >= 1`。如果 `count < 1`，
        // 则必定不满足至少选其一组的要求。
        size_t count = std::count_if(group.begin(), group.end(), [this](const char *name) {
            if (strlen(name) == 1) {
                if (shortname_2_arg_.count(name[0]) == 0) {
                    internel::error_msg << command_name_ << ": Can not find -" << name << " option.";
                    internel::exit_or_throw(internel::error_msg);
                }
                return shortname_2_arg_.at(name[0])->is_hit();
            } else {
                if (longname_2_arg_.count(name) == 0) {
                    internel::error_msg << command_name_ << ": Can not find --" << name << " option.";
                    internel::exit_or_throw(internel::error_msg);
                }
                return longname_2_arg_.at(name)->is_hit();
            }
        });
        if (count < 1) {
            internel::error_msg << command_name_ << ": The one of require relationship is not satisfied. "
                                << get_description(one_required_groups_.at(idx))
                                << ": at least one option should exist.";
            internel::exit_or_throw(internel::error_msg);
        }
        idx++;
    }
}

std::string Command::get_description(std::vector<const char *> &group)
{
    std::string description;
    description.reserve(100);
    description.push_back('[');
    for (const auto &arg : group) {
        description.append(strlen(arg) == 1 ? "-" : "--");
        description.append(", ");
    }
    description.pop_back();
    description.pop_back();
    description.push_back(']');
    return description;
}

void Command::add_help_arg()
{
    std::shared_ptr<Arg> arg = Arg::new_help_arg();

    int argid = current_argid_--;
    arg->set_argid(argid);

    longname_2_arg_[arg->get_long()] = arg;
    update_c_long_args(arg);

    shortname_2_arg_[arg->get_short()] = arg;
    update_c_short_args(arg);

    argid_2_arg_[argid] = arg;

    arg->set_command(this);
    arg->set_value("0");
    arg->conflicts_with_all();
}

void Command::print_usage_help()
{
    if (usage_format1_ != nullptr) {
        std::cout << usage_format1_ << std::endl;
    } else {
        for (size_t i = 0; i < line_size_; i++) {
            std::cout << usage_format2_[i] << std::endl;
        }
    }
    internel::exit_or_throw(internel::error_msg);
}

}  // namespace zul
