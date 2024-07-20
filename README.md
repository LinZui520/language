我有个主意☝️🤓

用C语言不依赖C库头文件开发一个新语言（或者叫编译器）

能不能实现还是个问题

吐槽：代码写的真烂 什么时候重构一下😅



### 目前进度🥴

- [x] 词法分析
- [x] 语法分析
- [x] 语义分析
- [x] 代码生成

**词法分析**完成，实现`lexer`函数

**语法分析**完成，实现`parser`函数

**语义分析**完成

**汇编代码生成**完成，**有点小bug**🫣

`source code` ---`lexer`---> `tokens`---`parser`--->`AST`

`AST`---`semantic_analysis`--->`symbols`--->`asm`				



### 目标

实现一个编译器，编译一个规则由自己制定的语言

支持`int`类型的**加减乘除**

支持`for`，`while`循环

支持`if`，`else`

支持函数



### 目前语言特性

关键字 ：

1. `func` 定义函数
2. `var` 定义变量 （可以省略）
3. 调用函数，但**不支持**调用函数的参数是调用函数比如`add(a, mul(b, c))`
4. `return` 函数返回值

标点符号：

1. `+`，`-`，`*`，`/` 四则运算
2. `(`，`)` 改变运算优先级
3. `;` 每条语句结束后必须加上这个

> 自然数是如此的美丽，所以请不要在代码中定义`a = -1`或`a = 1.2`之类的数字
>
> 也不要让这些表达式的计算的结果为负数或小数
>
> 绝对不是我懒得实现对负数和小数的判断，只因自然数太美丽了

**最后编译生成的可执行文件会打印`main`函数返回的值**



### 编译

进入`src`目录

```shell
$ cd src
$ make
```

这时会在`language/bin`下生成可执行文件`main`

在`language/bin`下新建`main.txt`文件并输入

正确代码 可以通过**语义分析**

```c
func add(a, b)
{
    return a + b;
}

func sub(a, b)
{
    return a - b;
}

func mul(a, b)
{
    return a * b;
}

func div(a, b)
{
    return a / b;
}

func f(a)
{
    b = a * a;
    return div(b, a);
}

func main()
{
    a = (add(1, 2) + mul(3, 4)) * (sub(6, 1) - div(8, 2));
    b = f(a);
    b = b + 1;
    return b;
}

```

然后执行命令

```shell
$ cd ../bin
$ ./compiler main.txt
```

就可以看到终端输出`tokens`，`AST_expr`，`symbols`

且会在当前目录生成`main.s`的汇编文件和`main`可执行文件

```sh
$ ./main
```

终端会输出`main`函数的返回值

如果发现没有输出 程序报错 请不要惊讶 很正常的嘛 人生哪有一帆风顺🤤
