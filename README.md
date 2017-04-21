# MiniC Compiler

## 运行环境
64位Linux操作系统

## 编译方法：
依赖的软件：

- cmake
- flex
- bison
- 支持c++11的编译器

```
$ cd /path/to/MiniCCompiler
$ cmake .
$ make
```

## 运行方法
上述编译方法会在bin目录下产生parser和sim两个可执行文件，分别是MiniC前端分析器和三地址代码模拟器。

parser从标准输入读入，输出到标准输出。

sim从命令行参数指定的文件读入，输出到标准输出，加-d选项进入调试模式，具体调试方法在[这里](doc/three-address-code-debug.txt)。

## 语法定义
[MiniC语法](doc/syntax.txt)

[三地址码语法](doc/three-address-code.txt)
