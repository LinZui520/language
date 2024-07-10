
typedef unsigned long int size_t;
typedef signed long int ssize_t;

#define STDIN 0
#define STDOUT 1

ssize_t read(ssize_t fd, char *buf, size_t count);

ssize_t write(ssize_t fd, const char *buf, size_t count);

ssize_t open(const char *pathname, int flags, int mode);

ssize_t close(ssize_t fd);


#define SEEK_SET 0
#define SEEK_CUR 1
#define SEEK_END 2

ssize_t lseek(ssize_t fd, ssize_t offset, ssize_t whence);
