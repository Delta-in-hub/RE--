# Regex based on Dfa
> https://swtch.com/~rsc/regexp/regexp1.html

功能与[Nfa版本](../nfaRE--/Readme.md)相同

[dfaRE-- 代码实现](./dfaRE--.hpp)

## 过程

nfa是给定目标字符串,一边遍历一边更新当前状态.而dfa是给定regexp就立即搜索,把所有状态建立出来,然后匹配就是在建完的图上搜索.

### 思路

我们在nfa的基础上构建dfa,在构造函数中构建dfa状态,匹配过程只需要在这个状态图中搜索即可.
```c++
class dfaRE : protected RE::nfaRE
{
    //...
};
```

在nfa中,我们使用两个`std::unordered_set<>`去保存当前状态集合与下一个状态集合,每读入一个字符便遍历当前状态集合,找出下一个状态集合.
在dfa中,我们将每一个可能的状态集合缓存起来,对于读入的一个字符无需遍历直接找到下一个状态.换言之,我们就是将nfa中`std::unordered_set<>`的每一种情况打包成一个新的状态.


引入结构体`DState`表示dfa中的状态,其为nfa中某几个状态的集合.
`std::vector<State*>`是该dfa状态包含哪些nfa状态
`std::unordered_map<char, DState*>`是到下一个dfa状态的索引.
```c++
struct DState
{
    std::unordered_map<char, DState*> m;
    std::vector<State*> n;
    bool searched;
};
```

此外,`std::map<std::vector<State*>, DState*> allDState`保存所有的dfa状态
因为使用`std::vector<State*>`作为key,因此每创建一个`vector`后都要`sort`一下,才能正确判重.


### 建立dfa
首先从nfa的`Start`状态建立dfa的开始状态,然后从这个状态开始递归的去建立所有dfa状态.
这个过程和[Nfa](../nfaRE--/nfaRE--.hpp)中的搜索过程很相似.不过是,从当前状态能接受的字符寻找下一个状态集合,然后把新的状态集合打包构成一个新的dfa状态.
可能一个dfa状态中有两个nfa状态能接受相同字符,只需要处理一次即可.


注意处理**重复dfa状态**与**环**.
一个dfa状态被创建后就加入到`allDState`中,当你在创建一个dfa状态前要先到`allDState`中查它是否已经被创建过.

对于环,因为整个状态中可能会出现环路(不仅仅是自环),我们深搜过去就会死循环,因此`DState`中的`searched`就排上用场.



### 内存限制
当dfa状态过多时,内存占用可能会很大,此时释放掉所有空间,然后使用`nfaRE::match()`去匹配.

与原文一致,将状态数限制为32
`constexpr size_t MAXDSTATELIMIT = 32`

## 与原文实现比较
原文对于每个状态中,使用`DState* next[256]`去散列,而我使用`std::unordered_map<>`,不考虑内存对齐等等,两者大概在字符数为58时,占用内存相当.[测试见此](https://gist.github.com/Delta-in-hub/b4fb3e92c1b4901622c88fb41ec9ca30).

~~原文直接在`DState`结构体上构建了一颗BST,而我对于每个`std::vector<State*>`在结构体中存了一遍,在`allDState`中又存了一遍.这个可以通过使用`std::vector<State*> *`做key,然后重载他的比较类解决.~~  那么现在只有容器带来的额外内存占用,这其实是完全可以忽略的,还避免了BST退化成链表的问题.


原文在第一遍搜索的时候构建状态,处理的非常巧妙!但我也确实是没怎么理解清楚.


## 关于dfa最小化
因为在构建dfa时,跳过了所有`Split|Merge`也就是空转移节点,所以dfa状态不会很多.形如`a(b|c)*`的正则式,仅会生成两个dfa状态,与书上最小化的dfa相同.
对于`(123?){2,5}|(abc*){4,6}`构建出的dfa状态数为28,也与最小化之后的状态数相同.
dfa最小化不会对这种隔构建方式不会有太大影响.

当然并不完备,对于`(ab)*(a*|b*)(ba)*`构建dfa状态数为7,但最小化之后应该为6.

从测试中我们也可以看出,RE--主要问题在于功能有限,以及构建正则速度慢于`std::regex`,我们解析正则的方式并不十分高效,拓展性也较差.

但还是实现了,虽然没有太多必要,但既然学了,最小化也是比较重要的一部分,就做吧,权当练手.
抱怨一下,这两天因为最小化牵扯了太多精力,LR文法学习进度被耽搁了.
昨天vscode出了bug,没法debug,心态爆炸.
今天,实现的思路错了.写完,debug完,跑测试样例,大部分都正确,少数错误.debug来debug去,发现是思路错误,推倒重来.
正确的思路很重要,敲代码可能只需要几十分钟.
一个完整的测试集也很重要,自己想的例子总是不够全面,让你误以为写对了.