// 一套简单的单元测试用例框架
//
// 编译环境：
// - Ubuntu 22.04 LTS
// - C++17，gcc 11.4.0，clang 14.0.0
//
// 版本信息：v1.0.0

#ifndef UNITTEST_FRAMEWORK_HEADER_
#define UNITTEST_FRAMEWORK_HEADER_

#include <cmath>
#include <cstdint>
#include <functional>
#include <iostream>
#include <memory>
#include <vector>

/**************************************************************************************************** */
// 这些是内部定义，用户不应该访问它们
extern uint64_t g_unit_test_failure_count;

using UnitTestFun = std::function<void()>;

struct UnitTest {
    UnitTest(std::string unit_test_group, std::string unit_test_name, UnitTestFun unit_test_fun)
        : group(unit_test_group), name(unit_test_name), function(unit_test_fun){};
    std::string group, name;
    UnitTestFun function;
};

struct AddUnitTest {
    AddUnitTest(std::unique_ptr<UnitTest> unit_test);
};

std::vector<std::unique_ptr<UnitTest>>& GetAllUnitTests();

#define UNIT_TEST_STATISTICAL_INFORMATION_INIT() uint64_t g_unit_test_failure_count = 0
/**************************************************************************************************** */

// 初始化单元测试用例程序
#define INIT_UNIT_TEST_APP(app)                                                                              \
    std::vector<std::unique_ptr<UnitTest>>& GetAllUnitTests()                                                \
    {                                                                                                        \
        static std::vector<std::unique_ptr<UnitTest>> all_unit_tests;                                        \
        return all_unit_tests;                                                                               \
    }                                                                                                        \
    AddUnitTest::AddUnitTest(std::unique_ptr<UnitTest> unit_test)                                            \
    {                                                                                                        \
        GetAllUnitTests().push_back(std::move(unit_test));                                                   \
    };                                                                                                       \
    UNIT_TEST_STATISTICAL_INFORMATION_INIT();                                                                \
    int main()                                                                                               \
    {                                                                                                        \
        std::cout << "\n" << #app << "\n\n";                                                                 \
        for (size_t i = 0; i < GetAllUnitTests().size(); ++i) {                                              \
            uint64_t _tmp = g_unit_test_failure_count;                                                       \
            GetAllUnitTests()[i]->function();                                                                \
            std::cout << "----> Test ";                                                                      \
            std::cout << (_tmp == g_unit_test_failure_count ? "PASSED" : "FAILED");                          \
            std::cout << " - [" << GetAllUnitTests()[i]->group << "] [" << GetAllUnitTests()[i]->name << "]" \
                      << "\n";                                                                               \
        }                                                                                                    \
        std::cout << "\n" << g_unit_test_failure_count << " tests failed\n\n";                               \
        return 0;                                                                                            \
    }

// 添加一个单元测试用例
#define ADD_UNIT_TEST_CASE(group, name)                                                      \
    static void unit_test_##group##_##name();                                                \
    AddUnitTest add_unit_test_##group##_##name(                                              \
        std::unique_ptr<UnitTest>(new UnitTest(#group, #name, unit_test_##group##_##name))); \
    static void unit_test_##group##_##name()

// CHECK_* 系列宏用于检查条件是否满足。
// 若条件不满足，它会打印一条错误消息，指明出错的模块和行号，但不会终止程序。
// 这在单元测试中非常有用，因为我们不希望一个测试用例失败就导致整个测试中断。
#define CHECK_ASSERT(x)                                                   \
    ((x) ? (0)                                                            \
         : ((std::cout << "  ↑  " << __FILE__ << ":" << __LINE__ << ":\n" \
                       << "  ↑  FAILED: " << #x << "\n"),                 \
            ++g_unit_test_failure_count))

#define CHECK_EQ(x, y)                                                                        \
    (((x) == (y)) ? (0)                                                                       \
                  : ((std::cout << "  ↑  " << __FILE__ << ":" << __LINE__ << ":\n"            \
                                << "  ↑  FAILED: " << #x << " <= " << #y << "\n"              \
                                << "  ↑  values were '" << (x) << "' and '" << (y) << "'\n"), \
                     ++g_unit_test_failure_count))

#define CHECK_ARRAY_EQ(x, y)                                         \
    if ((x) != (y)) {                                                \
        std::cout << "  ↑  " << __FILE__ << ":" << __LINE__ << ":\n" \
                  << "  ↑  FAILED: " << #x << " == " << #y << "\n";  \
        std::cout << "  ↑  values were [ ";                          \
        for (const auto& e : x) {                                    \
            std::cout << e << " ";                                   \
        };                                                           \
        (std::cout << "] and [ ");                                   \
        for (const auto& e : y) {                                    \
            std::cout << e << " ";                                   \
        };                                                           \
        std::cout << "]\n";                                          \
        ++g_unit_test_failure_count;                                 \
    }

#define CHECK_NE(x, y)                                                                        \
    (((x) != (y)) ? (0)                                                                       \
                  : ((std::cout << "  ↑  " << __FILE__ << ":" << __LINE__ << ":\n"            \
                                << "  ↑  FAILED: " << #x << " <= " << #y << "\n"              \
                                << "  ↑  values were '" << (x) << "' and '" << (y) << "'\n"), \
                     ++g_unit_test_failure_count))

#define CHECK_LT(x, y)                                                                       \
    (((x) < (y)) ? (0)                                                                       \
                 : ((std::cout << "  ↑  " << __FILE__ << ":" << __LINE__ << ":\n"            \
                               << "  ↑  FAILED: " << #x << " <= " << #y << "\n"              \
                               << "  ↑  values were '" << (x) << "' and '" << (y) << "'\n"), \
                    ++g_unit_test_failure_count))

#define CHECK_GT(x, y)                                                                       \
    (((x) > (y)) ? (0)                                                                       \
                 : ((std::cout << "  ↑  " << __FILE__ << ":" << __LINE__ << ":\n"            \
                               << "  ↑  FAILED: " << #x << " <= " << #y << "\n"              \
                               << "  ↑  values were '" << (x) << "' and '" << (y) << "'\n"), \
                    ++g_unit_test_failure_count))

#define CHECK_LE(x, y)                                                                        \
    (((x) <= (y)) ? (0)                                                                       \
                  : ((std::cout << "  ↑  " << __FILE__ << ":" << __LINE__ << ":\n"            \
                                << "  ↑  FAILED: " << #x << " <= " << #y << "\n"              \
                                << "  ↑  values were '" << (x) << "' and '" << (y) << "'\n"), \
                     ++g_unit_test_failure_count))

#define CHECK_GE(x, y)                                                                        \
    (((x) >= (y)) ? (0)                                                                       \
                  : ((std::cout << "  ↑  " << __FILE__ << ":" << __LINE__ << ":\n"            \
                                << "  ↑  FAILED: " << #x << " >= " << #y << "\n"              \
                                << "  ↑  values were '" << (x) << "' and '" << (y) << "'\n"), \
                     ++g_unit_test_failure_count))

#define CHECK_CLOSE(x, y, tol)                                                                          \
    ((std::abs((x) - (y)) < tol)                                                                        \
         ? (0)                                                                                          \
         : ((std::cout << "  ↑  " << __FILE__ << ":" << __LINE__ << ":\n"                               \
                       << "  ↑  FAILED: abs(" << #x << " - " << #y << ") < " << #tol << "\n"            \
                       << "  ↑  values were '" << (x) << "', '" << (y) << "' and '" << (tol) << "'\n"), \
            ++g_unit_test_failure_count))

#define CHECK_THOW(S, E)                                                     \
    try {                                                                    \
        S;                                                                   \
        throw "throwanything";                                               \
    } catch (E const& e) {                                                   \
    } catch (...) {                                                          \
        std::cout << "  ↑  " << __FILE__ << ":" << __LINE__ << ":\n"         \
                  << "  ↑  FAILED: " << #E << " is expected to be thrown\n"; \
        ++g_unit_test_failure_count;                                         \
    }

#define CHECK_NO_THOW(S)                                                  \
    try {                                                                 \
        S;                                                                \
    } catch (...) {                                                       \
        std::cout << "  ↑  " << __FILE__ << ":" << __LINE__ << ":\n"      \
                  << "  ↑  FAILED: exception thrown from " << #S << "\n"; \
        ++g_unit_test_failure_count;                                      \
    }

// CHECK_*_EXIT 宏用于检查条件是否满足。
// 如果条件不满足，它会打印一条错误消息，指出错误发生的模块和行号，然后退出程序。
#define CHECK_ASSERT_EXIT(x)                                              \
    ((x) ? (0)                                                            \
         : ((std::cout << "  ↑  " << __FILE__ << ":" << __LINE__ << ":\n" \
                       << "  ↑  FAILED: " << #x << "\n"),                 \
            exit(-1)))

#define CHECK_EQ_EXIT(x, y)                                                                   \
    (((x) == (y)) ? (0)                                                                       \
                  : ((std::cout << "  ↑  " << __FILE__ << ":" << __LINE__ << ":\n"            \
                                << "  ↑  FAILED: " << #x << " == " << #y << "\n"              \
                                << "  ↑  values were '" << (x) << "' and '" << (y) << "'\n"), \
                     exit(-1)))

#define CHECK_ARRAY_EQ_EXIT(x, y)                                    \
    if ((x) != (y)) {                                                \
        std::cout << "  ↑  " << __FILE__ << ":" << __LINE__ << ":\n" \
                  << "  ↑  FAILED: " << #x << " == " << #y << "\n";  \
        std::cout << "  ↑  values were [ ";                          \
        for (const auto& e : x) {                                    \
            std::cout << e << " ";                                   \
        };                                                           \
        (std::cout << "] and [ ");                                   \
        for (const auto& e : y) {                                    \
            std::cout << e << " ";                                   \
        };                                                           \
        std::cout << "]\n";                                          \
        exit(-1);                                                    \
    }

#define CHECK_NE_EXIT(x, y)                                                                   \
    (((x) != (y)) ? (0)                                                                       \
                  : ((std::cout << "  ↑  " << __FILE__ << ":" << __LINE__ << ":\n"            \
                                << "  ↑  FAILED: " << #x << " == " << #y << "\n"              \
                                << "  ↑  values were '" << (x) << "' and '" << (y) << "'\n"), \
                     exit(-1)))

#define CHECK_LT_EXIT(x, y)                                                                  \
    (((x) < (y)) ? (0)                                                                       \
                 : ((std::cout << "  ↑  " << __FILE__ << ":" << __LINE__ << ":\n"            \
                               << "  ↑  FAILED: " << #x << " == " << #y << "\n"              \
                               << "  ↑  values were '" << (x) << "' and '" << (y) << "'\n"), \
                    exit(-1)))

#define CHECK_GT_EXIT(x, y)                                                                  \
    (((x) > (y)) ? (0)                                                                       \
                 : ((std::cout << "  ↑  " << __FILE__ << ":" << __LINE__ << ":\n"            \
                               << "  ↑  FAILED: " << #x << " == " << #y << "\n"              \
                               << "  ↑  values were '" << (x) << "' and '" << (y) << "'\n"), \
                    exit(-1)))

#define CHECK_LE_EXIT(x, y)                                                                   \
    (((x) <= (y)) ? (0)                                                                       \
                  : ((std::cout << "  ↑  " << __FILE__ << ":" << __LINE__ << ":\n"            \
                                << "  ↑  FAILED: " << #x << " == " << #y << "\n"              \
                                << "  ↑  values were '" << (x) << "' and '" << (y) << "'\n"), \
                     exit(-1)))

#define CHECK_GE_EXIT(x, y)                                                                   \
    (((x) >= (y)) ? (0)                                                                       \
                  : ((std::cout << "  ↑  " << __FILE__ << ":" << __LINE__ << ":\n"            \
                                << "  ↑  FAILED: " << #x << " == " << #y << "\n"              \
                                << "  ↑  values were '" << (x) << "' and '" << (y) << "'\n"), \
                     exit(-1)))

#define CHECK_CLOSE_EXIT(x, y, tol)                                                                     \
    ((std::abs((x) - (y)) < tol)                                                                        \
         ? (0)                                                                                          \
         : ((std::cout << "  ↑  " << __FILE__ << ":" << __LINE__ << ":\n"                               \
                       << "  ↑  FAILED: abs(" << #x << " - " << #y << ") < " << #tol << "\n"            \
                       << "  ↑  values were '" << (x) << "', '" << (y) << "' and '" << (tol) << "'\n"), \
            exit(-1)))

#define CHECK_THOW_EXIT(S, E)                                                \
    try {                                                                    \
        S;                                                                   \
        throw "throwanything";                                               \
    } catch (E const& e) {                                                   \
    } catch (...) {                                                          \
        std::cout << "  ↑  " << __FILE__ << ":" << __LINE__ << ":\n"         \
                  << "  ↑  FAILED: " << #E << " is expected to be thrown\n"; \
        exit(-1);                                                            \
    }

#define CHECK_NO_THOW_EXIT(S)                                             \
    try {                                                                 \
        S;                                                                \
    } catch (...) {                                                       \
        std::cout << "  ↑  " << __FILE__ << ":" << __LINE__ << ":\n"      \
                  << "  ↑  FAILED: exception thrown from " << #S << "\n"; \
        exit(-1);                                                         \
    }

#endif  // UNITTEST_FRAMEWORK_HEADER_
