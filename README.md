我有个主意☝️🤓

用C语言不依赖C库头文件开发一个新语言（或者叫编译器）

能不能实现还是个问题



### 目前进度🥴

- [x] 词法分析
- [x] 语法分析
- [ ] 语义分析
- [ ] 代码生成

**词法分析**完成，实现`lexer`函数

**语法分析**完成，实现`parser`函数

`source code` ---`lexer`---> `tokens`---`parser`--->`AST`



### 目标

实现一个编译器，编译一个规则由自己制定的语言

支持`int`类型的**加减乘除**

支持函数



### 编译

进入`src`目录

```shell
$ cd src
$ make
```

这时会在`language/bin`下生成可执行文件`main`

在`language/bin`下新建`main.txt`文件并输入

```c
func add(var a, var b)
{
    return a + b;
}

func main()
{
    var a = 1;
    var b = 2;
    var c = 3 + add(a, b);
    var d = c * 4;
    return 0;
}

```

然后执行命令

```shell
$ ../bin/main ../bin/main.txt
```

就可以看到终端输出`tokens`，`AST_expr`