rm gmsv_socks_linux.dll
g++ -m32 -fPIC -g -c -shared -std=c++0x -I./ -I./includes *.cpp
g++ -m32 -g -shared -o gmsv_socks_linux.dll *.o
chmod -R 777 .

