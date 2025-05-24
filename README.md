# hfutnet
一个可以快速连接合肥工业大学校园网的工具

# 开发背景
由于工大校园网的连接方式比较原始，无法在插拔网线前后保存连接会话、实现自动登录。为了简化/自动化这一过程，我开发了这个工具。

# 使用方法
```cmd
hfutnet.exe -s/--sid <学号> -p/--password <密码>
```

## 使用示例
```cmd
hfutnet.exe -s 123456789 -p 114514
```

# 独立编译

## 环境依赖
- Windows 10/11
- MinGW-w64
- Git

## 编译步骤
```cmd
git clone https://github.com/Pectics/hfutnet.git
cd hfutnet
g++ hfutnet.cpp -o hfutnet.exe -lwininet
```

# 使用技巧

待编辑...