我有个主意☝️🤓

用C语言不依赖任何头文件开发一个新语言

能不能实现还是个问题



### 目前进度🥴

- [x] 词法分析
- [ ] 语法分析
- [ ] 语义分析
- [ ] 代码生成

暂时完成**词法分析**，实现`lexer`函数

`source code` ---`lexer`---> `tokens`



### 编译

进入`src`目录

```shell
$ cd src
$ make
```

这时会在`language/bin`下生成可执行文件`main`

在`language/bin`下新建`main.c`文件并输入

```c
int add(int a, int b)
{
    return a + b;
}

int main()
{
    int a = 1;
    int b = 2;
    int c = add(a, b);
}

```

然后执行命令

```shell
$ ../bin/main ../bin/main.c
```

就可以看到终端输出`tokens`