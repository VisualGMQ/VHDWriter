# FixedVHD Writer工具

## 介绍
这个工具可以向固定VHD文件中写入信息。这个工具是完全跨平台的。

## 编译
### 命令行版本
命令行版本采用纯C++11编写，你只需：

```bash
make
```

### GUI版本
GUI版本使用wxWidgets和C++11编写。所以如果你没有wxWidgets你需要安装一个。如果你是在Windows下你可能要修改Makefile来让程序找到你的wxWidgets。

```bash
make FixedVHDWriterGUI
```

编译之后的文件在build/bin/下

其实Windows上有很多好用的VHD写入工具，所以我就没怎么在Windows上下心思了（可能编译会比较麻烦）。