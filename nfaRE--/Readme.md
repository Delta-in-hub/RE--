# Regex based on Nfa
> https://swtch.com/~rsc/regexp/regexp1.html

仅支持 `( | ) * + ?`
不支持元字符以及转义,如`.` `\d` `\(`等等...
例如`(ab)`将匹配`ab`,`\(ab\)`将匹配`\ab\`

[nfaRE-- 代码实现](./nfaRE--.hpp)

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
原文中的`Ptrlist`共用体对我来说实在是有点绕,如果我没理解错的话,作者使用union实现type punning,使得Ptrlist即可指向`State`,也可指向`State*`
如此处理`|`和`?`只需要引入一个节点,而不是两个

![v2-8270a394daf5b58cc218c256c00adee5_r](../assets/v2-8270a394daf5b58cc218c256c00adee5_r.png)

上方的图,箭头类型为`State*`或者`State**`
下方的图,箭头类型为`State*`

我们进行如下处理,多引入一个节点也没关系的,反而在代码思路上更加清晰.
<img src="../assets/scanner_20210403_232010.jpg" alt="scanner_20210403_232010" style="zoom: 40%;" />
只有`Split`的`out1`是有意义的,其余`out1`根本用不到的.
处理时注意,若当前`State`为`Split`时,要合并他的`out1`,看图就理解了(e*那种情况).

此外,`Frag`结构体保存当前正则块的头和尾,`head`指向第一个节点,`tail`指向最后一个节点.
处理到最后,只剩下一个`Frag`,其`head`指向起始状态,`tail`指向终止状态.

### 3.模拟nfa状态转移

维护两个集合,一个是当前状态的集合,一个是当前状态接受一个字符之后的状态集合.
原文中使用两个预先开好的数组,为了避免插入时重复,对每个`State`维护`lastid`代表其最后加入的数组,这样插入时就不用再扫描一遍数组才能判断是否已经存在于数组中了.
每读入一个字符时,就从当前状态找下一个状态集合,然后交换当前集合与下一个状态集合,意思就是状态前进一步.
读入所有字符后,在当前状态集合中查找是否存在终止状态,若有则匹配成功.

但是,我们自然想到使用`std::unordered_set<>`进行维护,但是插入变对数时间,清除变线性时间.


### 其他
析构时,不能直接深搜释放空间,因为状态图是有环的,可能会多次`delete`同一个地址.
可以跑一遍拓扑序,或者用`std::unordered_set<>`把每个状态都存起来然后再`delete`.要先序遍历,然后加入时判断是否已经加入.
这里dubug了好久,记得不要`delete &Accept`.

## 与原文实现比较
原文中处理`?` `|`仅需要引入一个新的节点,而我引入两个.我对原文中`Ptrlist`的处理不是很理解.

原文使用数组存储状态集合,我是用`std::unordered_set<>`,如此不用手动处理重复加入,缺点也很明显: 1.占用更多的内存 2.每次用完要`clear()`一下,数组仅需移动指针即可.
状态数与正则表达式长度成线性,因此状态也不会很多.