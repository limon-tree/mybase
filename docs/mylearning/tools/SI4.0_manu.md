# SI总结

## 简介
[使用教程](http://10.10.8.114:8888/mylearning/7a7908b6_8b440c62_0.html)

## 使用技巧总结

### 调用可执行程序
调用SVN的可执行程序，直接比较当前文件和SVN服务器上的文件。前提条件是安装好了SVN，且当前文件是SVN服务器Checkout下来的。

 1. 命令名：svnDiff

    执行命令："C:\Program Files\TortoiseSVN\bin\TortoiseProc.exe" /command:diff /path:%f

 2. 命令名：svnshowlog

    执行命令："C:\Program Files\TortoiseSVN\bin\TortoiseProc.exe" /command:log /path:.

 3. 命令名：svncommit

    执行命令："C:\Program Files\TortoiseSVN\bin\TortoiseProc.exe" /command:commit /path:.
 4. Astyle 格式化代码（SI 3.5可用）

    命令名：Astyle

    执行命令："D:\tools\Insight3.exe"  --style=allman  --indent=spaces=4  --pad-oper --pad-header --unpad-paren  --suffix=none --align-pointer=name --lineend=linux --convert-tabs --verbose %f 
    
    这个rename Astyle.exe to Insight3.exe 为了跳过加密， --style=allman/java 个人爱好可选

 5. 4.0 SI 内置的格式化代码
Tools->Reformat Source Code Options->ANSI/K&S（java风格）   记得编辑EDit preset，去勾选Convert spaces to tabs
 
 6. SI_scan_config
 7. 去掉每次打开项目提示换行符和默认设置不一致,点击`option->preferences->files->Ask to fix inconsistent line ending`
 


### quiker.em的使用（从网络下载）

（1）、首先要把这个文件搞到手，然后将其加载到Base工程中，其次要定义一个快捷键调用quiker.em中的宏AutoExpand。quiker.em中写了那么多宏，为什么只调用AutoExpand宏就可以。因为调用AutoExpand宏后，AutoExpand会根据当前文本内容自动调用其他宏。一般情况下都用快捷键ctrl+enter调用AutoExpand宏。

（2）、输入文本config，然后执行AutoExpand，根据提示完成语言、姓名配置。

（3）、输入文本fu，然后执行AutoExpand，根据提示完成函数的注释。（在函数名的上一行执行）

（4）、输入文本if，然后执行AutoExpand，可以自动完成语法。其他类似。

（5）、输入文本file，然后执行AutoExpand，可以自动生成.c文件描述。

（6）、输入文本hdn，然后执行AutoExpand，根据提示完成.h文件宏定义。

（7）、在.c文件里输入hd，然后执行AutoExpand，可以自动生成.c文件对应的头文件。

（8）、后续补充    单行注释：Ctrl + /  <-->  CommentSelStr_old

```
co      弹窗配置语言和姓名
hi      文件顶部注释中新增modification
pn      弹窗定义bugID
abg     添加开始和结束注释行并附带bugID
dbg     删除开始和结束注释行并附带bugID
mbg     修改开始和结束注释行并附带bugID
{       单作用域补全
wh      补全while(){}
else    补全else{}
#ifd    弹窗定义#ifdef
#ifn    弹窗定义#ifndef
#if     弹窗定义#if
cpp     补全兼容C/C++宏定义
if      补全if(){}
ef      补全else if(){}
ife     补全if(){}else{}
ifs     补全if(){}else if(){}else{}
for     弹窗输循环变量名补全for(){}
sw      弹窗输入case数补全switch-case
case    补全单个case语句
do      补全do{}while();
st      弹窗输入结构体名补全_T_STRU
en      弹窗输入枚举名补全_ENUM
fi      弹窗置顶输入文件备注信息
fu      弹窗补全函数备注和格式
tab     弹窗提示替换空格数和是否全部替换
ap      弹窗提示带bugID和描述的双行注释
hd      一键补全头文件注释和兼容C/C++结构
ab      补全待bugID的add注释begin
ae      补全待bugID的add注释end
db      补全待bugID的delete注释begin
de      补全待bugID的delete注释end
mb      补全待bugID的modify注释begin
me      补全待bugID的modify注释end
 
source insight 4 快捷键设置
Macro: AutoExpand   Alt+Enter
高亮选中符号：F8
替换：Ctrl + h
查找：Ctrl + f
全局查找：Ctrl + /
向上搜索 Ctrl + F3
向下搜索 Ctrl + F4
跳转到行：F5
跳转到定义：Ctrl + 鼠标左键
单行注释：Ctrl + /  <-->  CommentSelStr_old
（1）打开 Symbol Window。
View -> Symbol Window。快捷键：Alt+F8。
（2）打开 Context Window。
View -> Panels -> Context Window
（3）打开 Relation Window。
View -> Panels -> Relation Window
（4）打开 Project Window。
View -> Panels -> Project Window
relation 关系	
Look up symbols 看(查)符号表 F7 
context 上下文 

project window				项目窗口  		Ctrl+O	
symbol window        		符号表窗口		Alt+F8 打开关闭
Context window   			上下文窗口		自定义打开关闭
relation window    			关系表窗口		自定义打开关闭
project symbols window      项目符号表窗口	F7
folders window				文件夹窗口		
```

### 编码格式设置
点击`option->files->Default encodeing` 设置为ANSI以保持和旧代码中中文格式一致，设置完毕后需要重新打开文件进行浏览。