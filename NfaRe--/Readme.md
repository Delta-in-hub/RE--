# Regex based on Nfa
> https://swtch.com/~rsc/regexp/regexp1.html

仅支持 `( | ) * + ?`
不支持元字符以及转义,如`.` `\d` `\(`等等...
例如`(ab)`将匹配`ab`,`\(ab\)`将匹配`\ab\`

## 过程
### 1.将正则式转化为后缀表达式
例如`a(bb)+a` -> `abb.+.a.` 这里我们假定`.`表示连接运算.

- [ ] 处理空括号
- [ ] 处理括号嵌套
- [ ] `a?*`处理
#### 样例
```
asabasd
a?b+c*d?
(a)(b)(c)(d)
(a?b*|sdc+df?)asdf
(a*b?|bcd)asdf
(a*b?|bcd)?asdf
asd(as?d|f*)
(a|b|c|d)
dsaf(a*d|b?d|c+d)?*
(((a)))(d)(b)
(a|(b*h+d?|h?d?d+d*)+)asdf
```
