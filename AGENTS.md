# AGENTS.md - 项目约定文档

本文档定义了 hfutnet 项目的开发约定，包括代码规范、提交信息格式、PR 规范等。

## 1. Commit Message 格式约定

### 1.1 基本格式

```
action(scope): 中文介绍摘要
```

或带详细说明的完整格式：

```
action(scope): 中文介绍摘要

- 中文详细变更说明1
- 中文详细变更说明2
- ...
```

### 1.2 Action 类型

- `feat`: 新增功能
- `fix`: 修复错误
- `refactor`: 重构代码（不改变功能）
- `perf`: 性能优化
- `docs`: 文档更新
- `style`: 代码格式调整（不影响功能，如空格、格式化等）
- `test`: 测试相关
- `build`: 构建系统或依赖项更新
- `chore`: 其他杂项修改

### 1.3 Scope 范围

- `core`: 核心功能模块（login/logout 逻辑）
- `utils`: 工具函数（md5_hex、url_encode、gb2312_to_utf8 等）
- `consts`: 常量定义（HTTP 头、正则表达式等）
- `cli`: 命令行接口和参数解析
- `README`: README 文档
- `AGENTS`: 本文档
- `build`: 编译配置
- `*`: 影响多个模块时使用

### 1.4 示例

```
feat(core): 添加网络断开自动重连功能

- 增加连接状态检测机制
- 实现自动重试逻辑，最多重试3次
- 添加重试间隔配置参数
```

```
fix(utils): 修正 URL 编码对特殊字符的处理
```

```
docs(README): 更新编译步骤说明
```

## 2. PR Title 格式约定

PR 的 title 使用以下格式：

```
action: 变更内容概要介绍
```

### 2.1 示例

- `feat: 添加自动重连功能`
- `fix: 修复登录失败时的错误处理`
- `refactor: 重构代码结构，拆分模块`
- `docs: 完善使用文档和编译说明`

## 3. 代码规范约定

### 3.1 文件组织

- **模块化设计**：将功能按职责分离到不同文件
  - `hfutnet.cpp`: 主程序入口和核心逻辑
  - `hfutnet/consts.h`: 常量和正则表达式定义
  - `hfutnet/utils.hpp`: 工具函数实现

- **头文件保护**：使用 `#pragma once` 进行头文件保护

### 3.2 命名规范

- **常量命名**：使用全大写下划线分隔（UPPER_SNAKE_CASE）
  - 示例：`USER_AGENT`、`ACCEPT_LANGUAGE`、`COM_PID`
  
- **函数命名**：使用小写下划线分隔（snake_case）
  - 示例：`gb2312_to_utf8()`、`url_encode()`、`md5_hex()`、`print_usage()`
  
- **变量命名**：使用小写下划线分隔（snake_case）
  - 示例：`sid`、`password`、`debug_mode`、`msg_code`
  
- **枚举类型**：使用大写下划线分隔，枚举名使用 PascalCase
  - 示例：`Operation`、`LOGIN_OPERATION`、`LOGOUT_OPERATION`

### 3.3 类型定义

- **优先使用标准库类型**：
  - 字符串使用 `std::string`
  - 流操作使用 `std::ostringstream`
  
- **Windows API 类型**：使用 Windows 原生类型
  - 句柄：`HINTERNET`、`HCRYPTPROV`、`HCRYPTHASH`
  - 数据类型：`DWORD`、`BOOL`、`BYTE`

### 3.4 常量定义

- **使用 inline 常量**：C++17 风格，避免多重定义
  ```cpp
  inline const std::string CONSTANT_NAME = "value";
  inline const std::regex REGEX_NAME(R"(pattern)");
  ```

- **字符串常量使用原始字符串字面量**：正则表达式使用 `R"(...)"`

### 3.5 错误处理

- **异常处理**：使用 `try-catch` 捕获异常
  - 登录函数中捕获 `std::exception` 并输出中文错误信息
  - 使用 `throw std::runtime_error()` 抛出运行时错误
  
- **错误信息**：使用中文描述，便于用户理解
  - 示例：`"登录失败"`、`"账号不存在"`、`"密码错误"`

- **调试模式**：通过 `debug_mode` 标志输出详细信息

### 3.6 字符编码处理

- **控制台输出**：设置为 UTF-8 编码
  ```cpp
  SetConsoleOutputCP(CP_UTF8);
  ```

- **编码转换**：
  - 网络响应从 GB2312 转换为 UTF-8：`gb2312_to_utf8()`
  - URL 参数编码：`url_encode()`

### 3.7 HTTP 请求

- **请求头标准化**：所有 HTTP 头部常量集中定义在 `consts.h`
  
- **表单数据构造**：使用 `std::ostringstream` 构建
  
- **资源管理**：使用 RAII 原则，及时关闭句柄
  ```cpp
  InternetCloseHandle(hRequest);
  InternetCloseHandle(hConnect);
  InternetCloseHandle(hInternet);
  ```

### 3.8 正则表达式

- **预编译正则**：将正则表达式定义为全局常量，避免重复编译
  ```cpp
  inline const std::regex REGEX_LOGIN_SUCCESS(...);
  inline const std::regex REGEX_MSG_CODE(...);
  ```

- **匹配检查**：使用 `std::regex_search()` 和 `std::smatch`

### 3.9 代码注释

- **必要注释**：
  - 复杂逻辑需添加解释性注释
  - TODO 标记未来扩展项：`// TODO: Reserve for future expansion`
  - 不明确的业务逻辑添加疑问标记：`// what?!`
  
- **注释语言**：
  - 代码注释使用中文或英文均可
  - 面向用户的输出信息使用中文

### 3.10 编译相关

- **链接库声明**：使用 `#pragma comment` 声明依赖库
  ```cpp
  #pragma comment(lib, "wininet.lib")
  #pragma comment(lib, "crypt32.lib")
  ```

- **编译命令**：
  ```cmd
  g++ hfutnet.cpp hfutnet/consts.h hfutnet/utils.hpp -o hfutnet.exe -lwininet -lcrypt32
  ```

### 3.11 命令行参数解析

- **参数顺序**：位置参数优先，标志参数灵活
  
- **标志格式**：支持长格式（`--flag`）和短格式（`-f`）
  - 示例：`--login`/`-i`、`--logout`/`-o`、`--help`/`-h`
  
- **帮助信息**：使用原始字符串字面量 `R"(...)"` 定义多行帮助文本

- **参数验证**：在执行操作前验证必需参数

### 3.12 代码风格

- **缩进**：使用 4 个空格
  
- **大括号**：K&R 风格（开括号不换行）
  
- **行宽**：建议不超过 100 个字符，特殊情况例外
  
- **空行**：函数之间使用单空行分隔

### 3.13 安全实践

- **密码处理**：
  - 使用 MD5 哈希加密（符合 Dr.COM 协议要求）
  - 不在日志或错误信息中输出明文密码
  
- **输入验证**：
  - URL 编码用户输入（`url_encode()`）
  - 检查必需参数是否提供

## 4. 文档约定

### 4.1 README.md

- **语言**：使用中文编写
- **结构**：按照"简介 → 使用方法 → 更新日志 → 编译 → 示例"组织
- **示例**：提供具体的命令行示例和截图说明

### 4.2 更新日志

- 按时间倒序排列（最新在前）
- 格式：`- **YYYY-MM-DD**: 变更描述`

## 5. 版本管理

### 5.1 分支策略

- `main` 或 `master`: 主分支，保持稳定
- `feature/*`: 功能开发分支
- `fix/*`: 问题修复分支
- `docs/*`: 文档更新分支

### 5.2 Git 使用

- 提交前确保代码可编译
- 避免提交生成的二进制文件（已在 `.gitignore` 中配置）
- 每次提交专注于单一变更

## 6. 项目特定约定

### 6.1 Dr.COM 协议

- 协议常量（`COM_PID`、`COM_CALG`、`COM_0MKKEY`）集中定义在 `consts.h`
- 密码计算公式：`MD5(PID + password + CALG) + CALG + PID`

### 6.2 网络连接

- 默认服务器：`172.16.200.11`
- 备用服务器配置在 `HOSTS` 数组中（预留扩展）
- 端口：HTTP 默认端口 80

### 6.3 错误码映射

- 使用 `switch-case` 映射 Dr.COM 响应消息码到中文错误信息
- 响应消息码通过 `Msg=` 模式提取
- 详细错误信息通过 `msga=` 模式提取

---

**注意**：本文档会随着项目发展持续更新，请开发者及时参考最新版本。
