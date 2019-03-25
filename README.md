# FLV_Parser

这个Demo主要是将音频码流和视频码流打包到一起后的数据——封装格式数据的处理程序。封装格式数据在视频播放器中的位置如下所示。

![image_text](https://github.com/MTerence/FLV_Parser/blob/master/FLV_Parser/Source/20160118102143440.png)

本Demo中的程序是一个FLV封装格式解析程序。该程序可以从FLV中分析得到它的基本单元Tag，并且可以简单解析Tag首部的字段。通过修改该程序可以实现不同的FLV格式数据处理功能。




# 原理
FLV封装格式是由一个FLV Header文件头和一个一个的Tag组成的。Tag中包含了音频数据以及视频数据。FLV的结构如下图所示。

![image_text](https://github.com/MTerence/FLV_Parser/blob/master/FLV_Parser/Source/20160118103525777.jpeg)


