我有个主意☝️🤓

用C语言不依赖C库头文件开发一个新语言（或者叫编译器）

能不能实现还是个问题

吐槽：代码写的真烂 什么时候重构一下😅



### 目前进度🥴

- [x] 词法分析
- [x] 语法分析
- [x] 语义分析
- [ ] 代码生成

**词法分析**完成，实现`lexer`函数

**语法分析**完成，实现`parser`函数

**语义分析**完成

`source code` ---`lexer`---> `tokens`---`parser`--->`AST`

`AST`---`semantic_analysis`--->`symbols`					



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
3. `return` 函数返回值

标点符号：

1. `+`，`-`，`*`，`/` 四则运算
2. `(`，`)` 改变运算优先级
3. `;` 每条语句结束后必须加上这个

> 自然数是如此的美丽，所以请不要在代码中定义`a = -1`或`a = 1.2`之类的数字
>
> 也不要让这些表达式的计算的结果为负数或小数
>
> 绝对不是我懒得实现对负数和小数的判断，只因自然数太美丽了



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

func main()
{
    var a = 1;
    b = 2;
    var c = 3 + add(a, b);
    d = c * 4;
    e = ((1 + 2) * (1 + 2)) * (1 + 2);
    return 0;
}

```

或者输入错误代码 不能通过**语义分析**

```c
func mul(a, b)
{
    c = a + b;
    return a * b * c;
}

func add(a, b)
{
    return a * (mul(a, b) + b);
}

func main()
{
    var a = 1;
    b = 2;
    c = 3 + mul(a, b);
    d = ((a + b) * (c + 4)) - (b / 2);
    e = (add(a, b) + e) * mul(c, d);
    return 0;
}

```

然后执行命令

```shell
$ ../bin/main ../bin/main.txt
```

就可以看到终端输出`tokens`，`AST_expr`，`symbols`

如果发现没有输出，程序报错 请不要惊讶 很正常的嘛 人生哪有一帆风顺🤤
