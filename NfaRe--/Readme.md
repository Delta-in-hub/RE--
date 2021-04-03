# Regex based on Nfa
> https://swtch.com/~rsc/regexp/regexp1.html

仅支持 `( | ) * + ?`
不支持元字符以及转义,如`.` `\d` `\(`等等...
例如`(ab)`将匹配`ab`,`\(ab\)`将匹配`\ab\`

## 过程
### 1.将正则式转化为后缀表达式
例如`a(bb)+a` -> `abb.+.a.` 这里我们假定`.`表示连接运算.

使用`numAtom`储存当前正则块数量,范围0-2.
在处理字符时,若`numAtom == 2`就要加一个连接符,然后`numAtom--`.
举例如下:输入rex为`abc`
```
postRex|numAtom
a       numAtom = 1
ab      numAtom = 2
之后读入第三个字符时,要加连接符了
ab.     numAtom = 1
此时 ab. 成为了一个正则块
ab.c    numAtom = 2
当前正则块为(ab.)和(c)这两部分
```
使用`numAlt`记录当前`|`的数量

遇到`(`将当前`numAtom`和`numAlt`状态压栈,然后全部赋0,从全新的状态继续处理.
遇到`)`处理当前的状态,然后弹栈取出进入`()`之前的状态.

最后,记得错误处理,以及最后的一些处理.

[代码见此](test\rexToPostRexTest.cc)

[正则样例](test\rexTest.txt)

### 2.从后缀表达式生成nfa状态
原文中的`Ptrlist`共用体对我来说实在是有点绕,貌似是

### 3.模拟nfa状态转移
