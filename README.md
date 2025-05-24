# hfutnet
一个可以快速连接合肥工业大学校园网的工具

# 开发背景
由于工大校园网的连接方式比较原始，无法在插拔网线前后保存连接会话、实现自动登录。为了简化/自动化这一过程，我开发了这个工具。

# 使用方法
```cmd
hfutnet.exe -s/--sid <学号> -p/--password <密码>
```

## 命令示例
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

# 使用示例

## 启动电脑时自动登录
1. 将`hfutnet.exe`放在一个固定的目录下，比如`C:\hfutnet.exe`
2. 按`Win + R`，输入`taskschd.msc`，打开任务计划程序
<img src="/assets/1-1.png" alt="运行" width="500">
<img src="/assets/1-2.png" alt="任务计划程序" width="500">
3. 选择左侧`任务计划程序库`，在右侧点击`创建基本任务`
<img src="/assets/1-3.png" alt="创建基本任务" width="500">
4. 输入任务名称，比如`CampusNetworkAutoLoginOnBoot`，点击`下一页`
5. 选择`当计算机启动时`，点击`下一页`
<img src="/assets/1-4.png" alt="选择触发器" width="500">
6. 选择`启动程序`，点击`下一页`
<img src="/assets/1-5.png" alt="选择操作" width="500">
7. 在`程序/脚本`中输入`C:\hfutnet.exe`，在`添加参数`中输入`-s <学号> -p <密码>`，点击`下一页`
<img src="/assets/1-6.png" alt="配置启动程序" width="500">
8. 点击`完成`
<img src="/assets/1-7.png" alt="完成" width="500">
9. 右键点击刚刚创建的任务，选择`属性`
<img src="/assets/1-8.png" alt="任务属性" width="500">
10. 在`常规`选项卡中，选择`不管是否登录都要运行`，并勾选`使用最高权限运行`
<img src="/assets/1-9.png" alt="常规" width="500">
11. 点击`确定`，输入管理员密码确认即可