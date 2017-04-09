##人脸识别PHP扩展

#### 说明：
1. 当前只有Linux版，不支持windows 
2. 需要的第三方库:gcc(本人测试时的版本是4.8.5)
                opencv3.2.0(请从官网下载源码安装)
3. 本人的php版本是5.5.7，但是可以支持更高版。Opencv的安装路径不会进行限制，开发者安装时可以随意指定
    opencv安装的路径。
4. 本函数并不是100%准确，很难实现100%，但是容错率很高的，当然这要取决于xml文件和图片的质量

#### 安装:
* 请到你的php安装目录的"*/ext"文件夹中创建face_detector文件夹。
* 进入face_detector文件夹调用"*/phpize"命令(可以自行查找phpize的位置)
* 然后把本包中的config.m4,configure文件替换face_detector生成的同名文件
* 输入命令
```
   ./configure --with-dynamic-dir="-L(你的opencv lib路径) -lopencv_shape 
   -lopencv_stitching -lopencv_objdetect -lopencv_superres -lopencv_videostab 
   -lopencv_calib3d -lopencv_features2d -lopencv_highgui -lopencv_videoio 
   -lopencv_imgcodecs -lopencv_video -lopencv_photo -lopencv_ml 
   -lopencv_imgproc -lopencv_flann -lopencv_core"  
   --with-include-dir=(你的opencv include路径)
```
* 然后make & make install即可，也可以使用make test测试是否成功，其它就像安装其它php模块步骤一样.
    如果成功，可以在php加载模块中找到face_detector模块
* 如果运行时失败，可尝试将opencv lib 路径加入到系统的g++运行lib搜索配置中，还不行可留言                

#### 使用:
只有一个函数
````
    $test = face_detector(
            "*/haarcascade_frontalface_alt.xml",
            "*/test.jpg");
    var_dump(json_decode($test))
    
    * 参数1 : 指定检测特征xml文件路径,最好是绝对路径,一般来说opencv源码包安装后都会有好几个xml文件，
              请自行查找位置路径,当然也可以自己写，详细上opencv官网查看
    * 参数2 : 此参数可以是图片数据字符串，可以是图片的路径，但是不支持base64字符串，若base64数据请自
              行解码后再传进去，支持多种图片格式:jpg，jpeg，png，bmp，gif等
    返回:
              设定正确返回的是json字符串，数组类型数据：
              [
                ['x'=>人脸的左上角x坐标,'y'=>人脸的左上角y坐标,'width'=>人脸的宽度,'height'=>人脸的高度]，
                ['x'=>0,'y'=>0,'width'=>0,'height'=>0，
                ['x'=>0,'y'=>0,'width'=>0,'height'=>0]，
                ......
              ]
              
              异常返回码 int型
                 说明             值   
              1. 形参有空         -1    
              2. xml文件路径错误   -2     
              3. xml文件解析失败   -3     (一般来说是xml文件内容格式错误，或不符合opencv的格式要求)
              4. 图片解析失败      -4     (有可能是你的输入的图片路径错误，也可能是你的图片数据格式错误或类型不支持)   
````