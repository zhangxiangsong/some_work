# zhangxiangsong

README

  本软件用于同步远程设置文件至本地服务器，可运行与windows 与 linux

COMPILE
  在windows下可直接通过Visual C++ 编译
  在linux 需要首先安装  curl-7.44.0.tar.gz
  然后修改clientsync/Makefile文件 指定上述软件的include 目录和 库目录
  在当前目录下执行sudo make即可编译

  注意必须进入到bin目录下启动 否则找不到服务器程序

  启动服务器  cd bin    sudo ./server_begin
  停止服务器  cd bin    sudo ./server_end

CONFIGFILE
  服务器配置文件可参照本身注释 

NOTICE
  运行该软件必须要管理员权限，或者有足够权限的用户