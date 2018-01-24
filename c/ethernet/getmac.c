/**
 * FileName:   getmac.c
 * Author:     Chen Yanfei
 * @contact:   fasionchan@gmail.com
 * @version:   $Id$
 *
 * Description:
 *
 * Changelog:
 *
 **/

#include <net/if.h>
#include <stdio.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/socket.h>


/**
 * 将二进制MAC地址转成成可读形式
 *
 * @n - 二进制形式，长度必须为6个字节
 * @a - 可读形式存储缓冲器，长度必须至少为18个字节(包含末尾'\0')
 **/
void mac_ntoa(unsigned char *n, char *a) {
    // 遍历二进制形式6个字节
    for (int i=0; i<6; i++) {
        // 格式化串
        char *format = ":%02x";
        // 首个字节不需要':'开头
        if(0 == i) {
            format = "%02x";
        }

        // 输出当前字节
        a += sprintf(a, format, n[i]);
    }
}


int main(int argc, char *argv[]) {
    // 创建套接字
    int s = socket(AF_INET, SOCK_STREAM, 0);
    if (-1 == s) {
        perror("Fail to create socket");
        return 1;
    }

    // 结构体填充网卡名
    struct ifreq ifr;
    strncpy(ifr.ifr_name, argv[1], 15);

    // ioctl系统调用获取硬件地址
    int ret = ioctl(s, SIOCGIFHWADDR, &ifr);
    if (-1 == ret) {
        perror("Fail to get mac address");
        return 2;
    }

    // 转换成二进制形式
    char mac[18];
    mac_ntoa((unsigned char *)ifr.ifr_hwaddr.sa_data, mac);

    // 打印结果
    printf("IFace: %s\n", ifr.ifr_name);
    printf("MAC: %s\n", mac);

    return 0;
}
