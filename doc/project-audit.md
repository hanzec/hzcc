# HZCC 项目审查与优化结果

2026-11-25 在 macOS arm64、Apple Clang 21.0.0 和 Ninja 上复查了以 `fdb92c2` 为基线的工作区。审查覆盖前端、AST、语义访问器、构建、CI、测试和文档，以及一个独立的 JVM 与优化实验目标。以下结果启用了默认的 `RUNTIME_CHECK`。

## 从基线到本次实现

| 检查 | 修改前 | 本次结果 |
| --- | --- | --- |
| Debug 单元测试 | 117/117 | 246/246，通过 AddressSanitizer 构建运行 |
| lit 编译器信息与命令行 | 1/1 | 2/2，新增多输入和输出文件回归 |
| lit 词法 | 81/81 | 81/81，负例同时检查退出码 |
| lit 语法 | 0/85 | 85/85，保留原有诊断与成功消息预期 |
| Clang 静态分析 | 39 个前端翻译单元，8 处告警 | 相同范围 0 处，3 个实验目标生产翻译单元也无分析器告警 |
| 实验 JVM 与死代码消除 | 未编译 | 独立目标的 2 个测试通过 |
| 文档覆盖 | XML 1168/1168，源码 1585/1585 | XML 1222/1222，源码 1642/1642，检查器测试 14/14 |

基线中，58 个语法用例先在形参构造处终止，9 个先在记录类型构造处终止，17 个先遇到缺失的解析入口，1 个先遇到诊断文本不匹配。完整修复同时覆盖了解析这些节点后的语句、表达式、作用域和成功输出。[语法用例](../test/syntax/test_set_0/)

## 15 项修复和验收依据

| # | 原问题 | 实现与回归依据 |
| --- | --- | --- |
| 1 | 合法形参、记录类型和表达式触发致命断言 | 修正 AST 构造前置条件。命名形参、结构体、字面量和二元表达式在启用运行检查时可以构造。[AST 声明](../hzcc/ast/Stmt.h) · [类型](../hzcc/ast/type/Type.h) |
| 2 | 变量和表达式入口缺失，顶层声明被丢弃 | 接通声明、表达式、初始化器和指针解析。编译单元保留非函数声明，函数仍由原有函数表独占，声明访问器可遍历二者。回归检查指针、初始化值和原型与定义的合并。[编译单元](../hzcc/ast/CompilationUnit.cc) · [解析器测试](../hzcc/parser/syntax/parser/test/parser_factory_test.cc) |
| 3 | 输出路径被改名或意外清空，写失败仍成功 | 仅词法模式接收 `-o`，保留路径中的空格。全部输入通过后才打开目标，检查打开与写入结果，并拒绝将输入作为输出。[驱动回归](../test/compiler_info/frontend_driver.py) |
| 4 | 多输入只处理首个文件，错误参数可能成功退出 | 驱动逐个处理输入，汇总失败。CLI11 拒绝冲突和未知选项。用两种输入顺序验证了失败前后的合法文件均被处理。[命令行](../hzcc/main.cc) |
| 5 | 指数和前导小数点分词错误，尾数截断丢失指数 | 识别 `1e3`、`.5` 并保留截短尾数后的指数。整数进制和范围校验与 AST 共享。[词法回归](../hzcc/parser/lexical/test/parser_line_boundary_test.cc) · [整数转换](../hzcc/utils/integer_literal.h) |
| 6 | 取余、字符以及不同进制整数求值错误 | 常量表示区分字符、64 位整数和实数。回归检查 `7 % 4 == 3`、进制、字符及边界，不能安全折叠的算术表达式返回空值。[常量测试](../hzcc/ast/test/DeduceValue_test.cc) |
| 7 | 私有或只读源文件及行首诊断处理错误 | 词法诊断复用正在扫描的行，接受第 0 列，读取失败也会携带文件名。缺少上下文时仍能返回诊断。[诊断](../hzcc/utils/logging.cc) · [输入边界测试](../hzcc/parser/lexical/test/parser_line_boundary_test.cc) |
| 8 | 命名作用域选择反转，嵌套循环和错误返回破坏状态 | 修正命名与匿名作用域分派。循环深度按编译单元保存，使用作用域清理恢复进入前的状态。测试覆盖嵌套循环、错误返回和后续解析。[作用域](../hzcc/ast/CompilationUnit.cc) · [解析器测试](../hzcc/parser/syntax/parser/test/parser_factory_test.cc) |
| 9 | 函数原型参数比对越界，二元运算重复检查同一侧 | 先比较参数个数再按类型检查。语义访问器分别验证左右操作数，回归交换非法操作数所在位置。[声明解析](../hzcc/parser/syntax/parser/decl_parser.cc) · [语义测试](../hzcc/ast/test/DeduceValue_test.cc) |
| 10 | 转换失败后读取已移动节点，规则选择和目标类型错误 | 先检查源和目标类型，在转换成功时才替换子节点。注册工厂创建实际的规则实例，测试验证成功转换的结果类型和失败后两个操作数的所有权。[规则分派](../hzcc/ast/cast/Cast.cc) · [语义访问器](../hzcc/parser/semantic_analyzer.cc) |
| 11 | 102 个 StatusOr 测试未注册，管道可能掩盖编译器失败 | 将测试迁入现有 GoogleTest 收集路径并适配 HZCC 接口。lit 开启 `pipefail`，反例持续匹配诊断并断言词法退出码 4 或语法退出码 1。[StatusOr 测试](../hzcc/utils/status/test/statusor_test.cc) · [lit 配置](../test/lit.site.cfg.py.in) |
| 12 | 逐 token 刷新，字符串池随进程累积 | stdout 和文件使用相同格式化函数，按行输出、按目标刷新。词法分析每次输入单独管理拼写，共享拼写随持有它的 token 释放。[驱动](../hzcc/main.cc) · [Token](../hzcc/parser/common/Token.h) |
| 13 | 常量子树被反复求值 | 算术节点在一次折叠中只求值左右子树各一次，再复用结果。计数测试覆盖嵌套表达式。[算术节点](../hzcc/ast/expr/biop/ArithmeticExpr.cc) · [计数测试](../hzcc/ast/test/DeduceValue_test.cc) |
| 14 | 纯前端也安装和链接 RE2 | RE2 随 E2E 的 FileCheck 安装。用三个独立的 vcpkg 安装目录分别配置、编译纯前端、单测和集成测试，仅集成测试目录含 RE2。[依赖清单](../vcpkg.json) · [构建配置](../cmake/Dependencies.cmake) |
| 15 | 实验接口陈旧，死代码消除重复报告修改 | 保留 `PassRegister`、`PassManagerImpl`、`FunctionPass::RunOnFunction` 和成功／修改两项返回值。原型、首次删除和再次执行各有检查。独立目标将一个无参整数字面量返回函数的 JVM 方法与课程样例对照。[死代码消除](../hzcc/optimization/pass_impl/DeadCodeElimination.cc) · [实验测试](../hzcc/codegen/jvm/test/constant_return_test.cc) |

## 语法诊断回归

声明和表达式解析保留各自的错误返回。缺少标点时给出期望内容和实际 token，遇到文件结束时指出尚未完成的声明、运算或括号。函数原型冲突报告函数名、参数序号、先前类型和当前类型；不支持的枚举或联合体定义、文件作用域的变长数组、重复的 `else` 和已知为 `void` 的条件都有专门诊断。命令行显示文件名、行号和出错 token，源码可读时附原行和插入符。另有结构体回归覆盖先通过指针引用标签、再完成定义，以及失败后重试；再次定义已完整的标签会报错。回归测试同时检查返回状态和实际输出。[声明诊断测试](../hzcc/parser/syntax/parser/test/decl_diagnostic_test.cc) · [语句诊断测试](../hzcc/parser/syntax/parser/test/statement_diagnostic_test.cc) · [命令行测试](../test/compiler_info/frontend_driver.py)

## 词法输出测量

同一台 macOS arm64 主机上，使用修改前后的 RelWithDebInfo 可执行文件运行 `--flexical_only --fno_color -o`。输入为 25,000 条生成语句，共 1,502,780 字节，含重复和不同的标识符、整数与实数。两个程序各预热一次，再交替测量五次，表中为各自中位数。

| 指标 | 修改前 | 修改后 |
| --- | ---: | ---: |
| 用时 | 1.08 秒 | 0.14 秒 |
| 峰值 RSS | 30,474,240 字节 | 30,277,632 字节 |

两份输出均为 375,000 个 token、16,152,780 字节，`cmp` 逐字节一致。SHA-256 为 `9acb14e19e8a48a6da3bc672df8b63f1eae244a870396f1953cef4088cd2a539`。该测量覆盖单输入写文件。

## 复查与后续优化

- **复查入口** — 使用 README 的 Debug 和 E2E 预设。另运行 `cmake --build --preset e2e --target check_experimental_jvm`。Doxygen XML、原始声明审计和检查器自身的测试分别验证提取结果、源码声明与检查逻辑。[构建和测试](../README.md#tests) · [文档检查](../README.md#development)
- **多文件词法输出的峰值内存** — 指定 `-o` 时，为了在后续输入出错时保住原文件，驱动保存本次调用的各份 token 列表。应以多文件负载测量峰值，再评估临时文件分批写入的空间和 I/O 成本。[输出实现](../hzcc/main.cc)
- **递归结构体的类型生命周期** — 对 `struct S { struct S *next; };` 的运行检查确认，字段保存指针类型，而指针类型持有结构体类型。销毁编译单元后这一组共享引用仍然存在。后续需在保持字段查询语义的前提下解除循环持有，并以编译单元销毁后的生命周期测试验收。[结构体字段](../hzcc/ast/type/StructType.cc) · [指针类型](../hzcc/ast/type/PointerType.cc)
- **后续语义和后端范围** — 默认命令执行到语法分析。将完整语义阶段接入命令行前，还需为 Part 4 的课程输入建立端到端基线。JVM 目前只验证了一个方法及一个已注册 pass，其余生成器和优化 pass 仍需逐项移植验证。Doxygen 仍对旧 JVM visitor 的成员定义报告 22 条声明匹配警告。[Part 4 与 JVM 样例](../test/) · [实验目标](../hzcc/CMakeLists.txt)
