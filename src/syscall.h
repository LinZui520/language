
typedef unsigned long int size_t;
typedef signed long int ssize_t;

ssize_t read(ssize_t fd, char *buf, size_t count);

ssize_t write(ssize_t fd, const char *buf, size_t count);

ssize_t open(const char *pathname, int flags, int mode);

ssize_t close(ssize_t fd);
