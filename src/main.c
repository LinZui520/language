#include "syscall.h"
#include "utils.h"

int main(int argc, char *argv[])
{
	// 判断输入参数是否正确
	if (argc != 2) {
		log("参数错误\n");
		return 1;
	}

	// 打开文件 获取文件描述符
	ssize_t fd = open(argv[1], 0, 0);
	if (fd < 0) {
		log("打开文件失败");
		return 1;
	}
	log("打开文件成功\n");

	// 获取文件长度
	ssize_t length = lseek(fd, 0, SEEK_END);

	// 重置文件指针
	lseek(fd, 0, SEEK_SET);

	// 读取文件内容
	char buf[length + 1];
	ssize_t count = read(fd, buf, length);
	if (count < 0) {
		log("读取文件失败");
		return 1;
	}
	buf[length] = '\0';
	log("读取文件成功\n");

	write(STDOUT, buf, length);

	return 0;
}
