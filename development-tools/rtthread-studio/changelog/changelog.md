# 更新日志

##### [V2.2.4更新发布](https://club.rt-thread.org/ask/article/5109bb2e156ed0f7.html)

- 新增调试模式透视图工具栏调试配置按钮
- 新增帮助菜单下查看精彩文章、精彩项目、常见问题，软件更新日志的菜单功能
- 新增支持以OpenOCD作为后端的任意调试器
- 修复黑色模式下进入RT-Thread Settings的短时间白屏问题
- 修复软件包中心搜索数量与结果不匹配问题
- 修复SDK Manager 离线导入后显示不正常的问题
- 修复命令行过长create proecess异常问题
- 修复开发板支持包制作工具打开时空白的问题
- 修复部分宏以数字结尾不显示标题如U8G2_USING_8080问题
- 修复部分电脑每日提示界面乱码问题
- 修复MD文件打不开的问题
- 修复打开软件包文档时跳转404页面的问题
- 修复基于芯片创建RT-Thread工程提示依赖未安装的问题
- 修复使用4.1.0版本rt-thread时搜索fal软件包无法添加的问题
- 修复离线导入的BSP创建的工程后无法查看BSP信息的问题
- 修复markdwon编辑器不能显示相对路径图片的问题
- 修复QEMU调试配置中配置了SD card 命令参数中没有同步添加的问题
- 优化切换语言菜单功能
- 优化Studio创建工程时默认选择最新稳定版本
- 优化Studio站点升级时不再显示eclipse插件升级
- 优化调试配置中svd文件选择时增加svd文件过滤
- 优化窗口菜单名称
- 优化开发板信息采用WEB风格界面
- 优化欢迎页面展示采用WEB风格并适配黑色模式
- 优化RT-Thread Settings打开速度及稳定性及减少资源占用
- 优化每日提示界面采用WEB风格界面
- 优化下载过程中禁止点击调试按钮功能，防止因下载未完成启动调试出现问题提示

 **芯片支持包** 

- ArteryTek/AT32F4 从0.0.7 升级到0.0.8版本
- STMicroelectronics/STM32L4 从0.2.2升级到0.2.3版本

 **开发板支持包** 

- Allwinner/ARM9-ALLWINNER-TINA 从1.0.0升级到1.0.1版本
- ArteryTek/AT32F403A-AT-START  从1.0.0升级到1.0.1版本
- GigaDevice/GD32F103C-GD-EVAL  从1.0.1升级到1.0.2版本
- GigaDevice/GD32F303Z-GD-EVAL  从1.0.0升级到1.0.1版本
- HPMicro/HPM6750-HPMicro-EVKMINI 从0.4.0 升级到0.5.0版本
- WCH/sdk-bsp-ch32v103r-evt 从1.0.0升级到1.0.1版本
- Nuvoton/NK-N9H30 新增1.0.0版本
- GigaDevice/GD32F450Z-GD-EVAL 从1.0.0升级到1.0.1版本
- NXP/IMXRT1052-NXP-EVK 从0.1.1升级到 0.1.4版本
- NXP/IMXRT1064-NXP-EVK 从1.0.1 升级到1.0.3版本
- GigaDevice/GD32VF103V-GD-EVAL 新增1.0.0版本
- WCH/sdk-bsp-ch32v307v-r1  从1.0.5升级到1.0.8版本
- Nuvoton/NK-980IOT 从1.0.1升级到1.0.2版本
- Kendrtye/K210-RT-DRACO 从1.0.3升级到1.0.4版本
- HPMicro/HPM6750-HPMicro-EVK 新增0.5.0版本
- STMicroelectronics/STM32F103-100ASK-PRO 新增1.0.0版本
- Renesas/sdk-bsp-cpk-ra6m4 从1.2.1升级到1.2.2
- HPMicro/HPM6300-HPMicro-EVK 新增0.6.0版本
- STMicroelectronics/STM32H750-RT-ART-Pi 从1.2.1升级到1.3.0


 **工具链支持包** 

- RISC-V-GCC 从10.1.0 升级到 11.1.0
- RISC-V-GCC-NUCLEI 新增 2022.05版本

 **调试器支持包** 

- OpenOCD-Nuclei 新增2022.05版本
- PyOCD 从0.1.3 升级到 0.1.4
- WCH-LINK_Debugger 从1.0.3升级到1.0.4



##### [V2.2.3更新发布](https://club.rt-thread.org/ask/article/3725.html)
- 新增 wch-link 的调试功能支持
- 新增 SDK Manager 过滤功能
- 新增堆使用状态和手动内存回收
- 新增代码标签右键菜单复制文件全路径和打开文件所在目录功能
- 新增搜索资源框支持全局匹配
- 新增通过帮助菜单打开 Studio 帮助文档和视频及设计参考的功能
- 修复 pyocd other options 配置后丢失的问题
- 修复站点升级部分 rt-thread settings 打不开的问题
- 修复更新 kcs 不填 option 报错的问题
- 修复部分使用 RT-Thread settings 保存异常导致工程信息丢失的问题
- 修复 rebuild 时模态框挡住界面的问题
- 修复 RT-Thread Settings 宏以数字结尾区分时，出现重复的中文翻译项
- 修复 RT-Thread Settings 变更文本框内容并且不移动鼠标直接快捷键保存，修改项不生效的问题
- 优化 RT-Thread Settings 数据微调按钮
- 优化工作空间占用时提示信息
- 优化 sdk 链接和 update 按钮初始化慢的问题
- 优化搜索视图初始化位置在项目资源管理器旁边
- 优化 RT-Thread Settings, 对正在编辑的文本框增加高亮效果
- 优化工程右键菜单，去掉一些不常用的菜单

**V2.2.3 新增开发板支持包**

- WCH/CH32V103R-EVT 版本1.0.0
- Geehy/APM32F1 版本1.0.0

**V2.2.3 更新开发板支持包**

- ARM/QEMU-VEXPRESS-A9 更新到 1.0.6 版本
- HPMicro/HPM6750-HPMicro-EVKMINI 更新到 0.4.0 版本
- Nuvoton/NK-980IOT 更新到 1.0.1 版本
- Nuvoton/NK-RTU980 更新到 1.0.1 版本
- WCH/CH32V307V-R1 更新到 1.0.6 版本
- STMicroelectronics/STM32F401-ST-NUCLEO 更新到0.2.1 版本
- STMicroelectronics/STM32F405-SMDZ-BREADFRUIT 更新到0.1.1 版本
- STMicroelectronics/STM32F411-ST-NUCLEO 更新到0.1.2 版本
- STMicroelectronics/STM32F411-WEACT-MINIF4 更新到 0.1.2 版本
- STMicroelectronics/STM32F412-ST-NUCLEO 更新到 1.0.1 版本
- STMicroelectronics/STM32F427-DJI-ROBOMASTER 更新到 1.0.1 版本
- STMicroelectronics/STM32F103-GIZWITS-GOKITV21 更新到 1.0.1 版本
- STMicroelectronics/STM32F107-UC-EVAL 更新到 1.0.1 版本
- STMicroelectronics/STM32F103-ONENET-NBIOT 更新到 1.0.1 版本

##### [V2.2.1更新发布](https://club.rt-thread.org/ask/article/3466.html)

- 新增 RTTSettings 汉化文件在线更新功能
- 优化 RTTSettings 点击搜索结果跳转不再展开多余节点
- 解决 RTTSettings 概率性无法填充界面问题
- 解决 RTTSettings 16进制显示问题

##### [V2.2.0更新发布](https://club.rt-thread.org/ask/article/3427.html)

-  RT-Thread Settings 全新升级
-  工具栏按钮图标 全新升级
-  新增针对工程的软件包更新开关配置
-  新增导航器窗口工程右键更新软件包等菜单
-  新增通过 RT-Thread bsp 创建工程
-  新增支持 RT-Thread Studio 安装包覆盖安装功能
-  新增支持 Env下的 packages 目录强制覆盖更新功能
-  新增显示 ST-LINK 显示调试器后端日志
-  解决 J-LINK 停止调试的时出现 data abort 异常
-  解决工程重命名后消失需刷新才出现的问题
-  解决不显示示例工程的问题
-  解决调试配置中的 config_* 变量无法识别的问题
-  解决 HPmicro 调试配置中的路径问题  

##### [V2.1.5更新发布](https://club.rt-thread.org/ask/article/3338.html)

-  新增支持新唐开发板(M2354/IOT-M487/PFM-M487/M032KI)的工程创建，下载，调试功能
-  新增支持新辑半导体开发板（HPM6750-HPMicro-EVKMINI）的工程创建，FT2232下载和调试功能
-  新增一键菜单切换工具链及其版本的功能
-  新增JLink下载配置界面功能
-  解决 jlink 下载时没有读取到 USB 的配置的问题
-  优化sdk下载latest源码包时下载速度提升50%

##### [V2.1.4更新发布](https://club.rt-thread.org/ask/article/3223.html)

-  新增更新Sconscripts 工程右键菜单    
-  新增支持沁恒的BSP创建的工程使用WCH-LINK下载  
-  新增Studio创建的瑞萨工程中添加配置功能入口  
-  新增国民技术半导体资源包上线支持  
-  新增资源下载源切换功能 
-  完善DAP-LINK下载配置    
-  解决SDK Manager高分辨率下资源包描述展示问题
-  解决构建过程中偶尔出现sh报错gcc not found
-  解决软件包中心添加外设例程失效的问题
-  解决点击添加软件无效的问题
-  解决DAP调试配置修改device name失效的问题
-  解决Program make.exe not found in PATH问题  

##### [V2.1.3更新发布](https://club.rt-thread.org/ask/article/3149.html)

-  新增上线 RT-Thread 4.0.4 源码包
-  新增上线东软载波 ES32F0、ES32F1 芯片支持包
-  新增上线5个开发板支持包
-  新增上线沁恒 RISC-V-GCC-WCH 工具链
-  新增上线嘉楠 RISC-V-GCC-KENDRYTE 工具链
-  新增上线嘉楠 OpenOCD-KENDRYTE 调试工具包
-  新增SDK Manager 依赖自动下载功能
-  新增SDK Manager 允许所有资源包删除功能
- 新增SDK Manager 离线导入已存在包功能
- 新增SDK Manager 切换下载源功能
- 完善排除构建功能
- 解决修改构建配置参数后点击编译不会重新编译的问题
- 解决中文翻译不彻底的问题
- 解决 BSP 工程切换系统版本失败的问题
- 解决新建的配置模板不生效的问题
- 解决 QEMU 工程创建失败的问题
- 解决 QEMU 找不到适配模拟器的问题
- 优化 QEMU 结束调试速度
- 优化搜索功能
- 优化精简功能菜单

##### [V2.1.2 更新发布](https://club.rt-thread.org/ask/article/3017.html)

- 新增软件异常信息库，辅助异常识别并提示的功能
- 新增状态栏显示文件编码的功能
- 新增开发板支持包能够基于模板制作的功能
- 新增添加源码文件是自动识别文件编码避免乱码的功能
- 完善 Studio 无缝支持 cubemx 后续高版本

##### [V2.1.1 更新发布](https://club.rt-thread.org/ask/article/2887.html)

- 新增上线 RT-Thread nano-3.1.5 源码包
- 新增上线 pyocd 0.1.2 版本添加东软支持
- 新增上线 jlink 6.80d2 版本支持华芯微特 SWM320
- 新增常见软件异常的日志分析和智能友好信息提示引导解决的功能
- 新增 Tip Of The Day 功能
- 新增支持串口终端历史命令
- 新增创建工程磁盘空间不足情况下友好提示
- 完善默认使用 use in line search
- 完善终端窗口在调试时显示
- 解决 ST-Link 调试配置 reset mode 失效问题
- 解决导入 MDK 工程使用 Dap-Link 调试异常问题
- 解决导入一个工程时重复选择两次芯片的问题
- 解决 QEMU-A9 Smart 工程编译不过的问题
- 解决工具链下载后编译时仍找不到工具链的问题
- 解决升级后默认快捷键设置丢失的问题
- 优化 SDK Manager 提示信息
- 优化组件依赖提示信息
- 优化 RT-Thread 工程的重命名功能
- 优化 STM32CubeMX 的已知问题
- 优化配置保存时对话框关闭时机调整
- 优化 QEMU 配置界面 Extra Commands 的显示
- 优化被排除资源的过滤功能

##### [V2.1.0 更新发布](https://club.rt-thread.org/ask/article/2647.html)

- 新增开发板支持包制作工具功能
- 新增 Studio 支持 MDK5 工程协同开发，同时具备双向同步功能
- 新增 Studio 调用并与 STM32CubeMX 协同开发，生成代码至 Studio 工程的功能
- 新增上线 8 个开发板厂商，40 多个新开发板支持包
- 新增代码辅助提示触发条件可配置的首选项功能
- 新增 QEMU 模拟器(STM32F401/STM32F410)
- 新增开发板制作教程文档上线至文档中心
- 新增 RT-Thread V4.0.3 源码包上线至 SDK Manager
- 新增 ST-LINK Debugger V1.6.0 软件包上线至 SDK Manager
- 新增 Studio 支持 MDK 协同开发教程文档上线至文档中心
- 新增 Studio 支持 CubeMX 协同开发教程文档上线至文档中心
- 完善 RT-Thread 源码包显示日期和最后更新日期保持一致
- 完善 Studio 重新构建时自动保存文件
- 完善 QEMU 调试配置参数并优化 QEMU 交互操作体验
- 解决工程构建命令行过长超出 windows 限制的问题
- 解决 Studio 无法切换 workspace 的问题
- 解决 SDK Manager 在部分电脑上导致 Studio 软件闪退的问题
- 解决在某些分辨率下工程创建向导的工程创建模式选项出不来的问题

##### [V2.0.0 更新发布](https://club.rt-thread.org/ask/article/2393.html)

- 新增集成 platformio 裸机框架功能
- 新增 Rebuild 功能(自动先清理工程再重新构建工程)
- 新增开发板详细信息查看入口
- 新增 Studio 工程模板支持包方便制作开发板支持包的模板工程
- 新增支持 jlink 通过 tcp/ip 连接下载和调试
- 新增 QEMU 支持网络功能
- 新增支持 arm-linux-musleabi 编译 RT-Thread Smart 内核
- 新增 FAQ-添加软件包后在 packages 目录找不到的解决方法
- 新增 FAQ-如何解决清理工程时命令行超出 windows 长度限制
- 完善链接脚本编辑器兼容更多格式链接脚本
- 解决 QEMU 网络配置没有正常生效的问题
- 解决 RT-Thread 配置项出现重复配置节点问题
- 解决 RT-Thread 配置节点展开时不停重复创建配置项节点问题

##### [V1.1.5 更新发布](https://club.rt-thread.org/ask/article/2290.html)

- 新增 ART-PI 开发板支持（请先升级至 V1.1.5，再去 sdk 下载 ART-PI 开发板资源包 V1.0.0 版本)
- 新增 STLINK Debugger 调试软件 v1.4.0 版本资源包
- 新增 STLINK 调试软件的版本切换
- 新增 SDK 资源包显示更新时间
- 新增 GD32VF103-NUCLEI-RVSTAR 开发板 v0.1.1 版本资源包
- 完善 RISCV 工程构建信息展示和调试外设寄存器显示问题
- 完善开发板大图查看
- 解决开发板信息展示页面购买链接不显示的问题
- 解决工程构建时链接命令长度超出 windows 长度限制问题(见[FAQ](https://www.rt-thread.org/document/site/#/development-tools/rtthread-studio/faq/studio-faq/#studio-windows))
- 解决只修改了链接脚本构建无效的问题
- 解决完整安装包体积过大问题
- 解决 pyocd 烧写问题(切换成烧写 bin 文件)

##### [V1.1.4 更新发布](https://club.rt-thread.org/ask/article/306.html)

- 工程向导开发和完善 

  新增基于开发板创建工程模式替代老的基于 bsp 模式

- 工具链支持 

  新增 RISC-V 工具链支持

- 构建功能完善 

  1.解决停止并启动调试没自动构建的问题 

  2.解决只修改了链接脚本后构建无效的问题

- SDK Manager 完善 

  1.新增开发板类型资源包 

  2.完善美化 SDK Manger 图标 

  3.解决 SDK 资源包安装失败的问题

- 新开发板支持 

  新增 IMXRT1064/1052 开发板的支持

- RT-Thread 配置完善 

  1.解决 LWIP 配置选项 ICMP 名称问题 

  2.解决 RT-Thread 配置页面在构建时没有自动保存的问题

- QEMU 功能完善 

  1.新增芯来 rvstar 开发板 qemu 仿真支持 

  2.新增 vexpress-a9 的 qemu 仿真支持

  3.新增 qemu 点击下载直接启动运行模式 

  4.新增 STM32 sdio 模拟支持

- 调试器功能完善 

  1.新增 ST-LINK 调试支持复位

  2.新增 ST-LINK 调试支持查看外设寄存器 

  3.新增 ST-LINK 外部 FLASH 下载算法支持 

  4.解决 ST-LINK 下载 Verify 时日志出现乱码的问题

  5.新增 DAP-LINK 对多下载算法的支持 

  6.新增 DAP-LINK 调试器对雅特力芯片下载和调试的支持 

  7.新增 DAP-LINK 下载调试参数配置便于加快下载速度 

  8.解决 J-Link 下载时工程路径有空格弹框需要手动输入起始地址的问题

- 语言切换完善 

  解决切换到英文后部分窗口仍有中文的问题

##### [V1.1.3 更新发布](https://club.rt-thread.org/ask/article/45.html)

- 新增支持新建工程时选择 DAP-Link 调试器功能(暂时不支持 stm32H7 系列)
- 新增支持新建工程时选择 QEMU 模拟处理器功能
- 新增新建 RT-Thread Nano 项目功能
- 新增在 “关于 RT-Thread Studio” 菜单查看更新日志功能
- 完善切换语言的功能，切换后自动重启 studio
- 解决导入工程未识别到芯片时无法退出的问题

##### [V1.1.2 更新发布](https://club.rt-thread.org/ask/article/17.html)

- 新增新建工程向导"添加更多"芯片系列选项
- 新增启动时自动检测并安装必需的内置资源包
- 新增 J-Link 软件支持包 6.80d 版本（支持雅特力芯片）
- 完善切换芯片支持包版本功能
- 完善 J-Link 调试配置提示信息
- 完善 SDK Manager 离线资源包导入功能
- 解决生成工程时 board.h 少了大括号的问题

##### [V1.1.1 更新发布](https://www.rt-thread.org/qa/thread-424893-1-1.html)

- 新增切换工程芯片支持包功能
- 新增切换工程 RT-Thread 版本功能
- 新增启动调试时先自动构建功能
- 新增创建工程默认设置为 UTF8 编码
- 完善切换工程芯片功能
- 完善下载和调试快捷键
- 完善新建工程向导
- 完善新建工程失败信息提示框
- 完善调试配置内路径保存为相对路径
- 完善 SDK Manager 离线资源包导入功能
- 解决树形配置列宽拖动会抖动问题
- 解决配置树点击过快弹框问题
- 解决部分软件包添加失效问题
- 解决 SDK Manager 安装状态图标显示异常问题

##### [V1.1.0 更新发布](https://www.rt-thread.org/qa/forum.php?mod=viewthread&tid=424576&page=1#pid478987)

- 新增 ST-Link 指定扇区擦除功能
- 新增 Code Analysis 功能
- SDK Manager 全新升级
- 新建工程向导全新升级

##### [V1.0.6 更新发布](https://www.rt-thread.org/qa/thread-424221-1-1.html)

- 新增导入 MDK/IAR 工程入口和功能
- 新增编译结果显示 ROM/RAM 使用情况
- 完善基于芯片的工程创建向导
- 完善搜索和替换功能
- 优化配置器打开速度

##### [V1.0.5 更新发布](https://www.rt-thread.org/qa/thread-424084-1-1.html)

- 新增 STM32L1 系列芯片支持(仅 latest 版本支持)
- 完善工程生成添加更多可用外设驱动
- 完善链接脚本编辑器
- 完善软件包下载日志
- 解决导入带空格路径下工程下载失败问题
- 解决导入工程后下载提示无调试配置的问题
- 优化配置器打开速度 修复部分显示问题

##### [V1.0.4 更新发布](https://www.rt-thread.org/qa/thread-423911-1-1.html)

- 新增编译日志简化输出和详细输出功能
- 新增显示当前工程 RT-Thread 版本功能
- 新增 link.lds 文件图形编辑器功能
- 完善导入工程时中文路径检查
- 优化调整 RT-Thread 配置界面
- 解决软件包版本显示不一致问题
- 解决导入工程后下载提示无调试配置的问题
- 解决 5.0 以下版本 JLink 下载结束后进程驻留问题

##### [V1.0.3 更新发布](https://www.rt-thread.org/qa/thread-423751-1-1.html)

- 新建工程向导芯片列表更新
- 调整工程向导基于芯片创建优先
- 调整 F1 系列芯片默认选择 103VE
- 完善树形配置界面提示框中信息展示
- 解决软件包版本显示不正确的问题
- 解决工程重命名后芯片切换和调试配置打不开问题
- 解决删除工程时占用异常的问题
- 升级工程生成器

##### [V1.0.2 更新发布](https://www.rt-thread.org/qa/thread-423613-1-1.html)

- 新增美观完善的 DevStyle 黑色主题
- 新增 ST-Link 程序下载后自动运行和配置功能
- 新增问题反馈菜单导向 Studio 论坛
- 新增配置项依赖信息查看
- 优化完善串口终端功能
- 解决 C++支持问题

##### V1.0.1 更新发布

- 新增项目创建向导控制台串口 LPUART1 选项
- 新增树形配置搜索功能右键菜单（Ctrl+F），便于查找配置项
- 新增树形配置界面一键最大化功能按钮，便于树形配置操作
- 新增 Drivers 组件开发文档右键功能菜单，便于查看开发文档
- 新增 moonrise 主题功能插件
- 新增 RT-Thread 类别的工程导入入口
- 新增启用 C++组件时工程自动添加 C++支持
- 解决树形配置出现重复节点的问题
- 解决欢迎页在部分电脑不显示的问题
- 解决软件包版本无法修改的问题
- 简化 JLink 配置,去掉 JLink 下载配置页
- 采用新的终端软件 putty
- 采用带封装命名的芯片名形式
- 优化完善工程生成器
