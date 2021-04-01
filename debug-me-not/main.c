#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/mman.h>
#include <sys/ptrace.h>

#define PAGE_SIZE (4096)
#define CRC32_JIT (0x33d682a)

extern uint8_t _binary_decrypt_enc_start[];
extern uint8_t _binary_decrypt_enc_end[];

void (*decrypt_func)(char*);

// From http://home.thep.lu.se/~bjorn/crc/
// (public domain)
uint32_t crc32_for_byte(uint32_t r)
{
    for (int j = 0; j < 8; ++j)
        r = (r & 1 ? 0 : (uint32_t)0xEDB88320L) ^ r >> 1;
    return r ^ (uint32_t)0xFF000000L;
}

uint32_t crc32(const void* data, size_t n_bytes)
{
    uint32_t crc = 0;
    static uint32_t table[0x100];
    if (!*table)
        for (size_t i = 0; i < 0x100; ++i)
            table[i] = crc32_for_byte(i);
    for (size_t i = 0; i < n_bytes; ++i)
        crc = table[(uint8_t)crc ^ ((uint8_t*)data)[i]] ^ crc >> 8;

    return crc;
}

__attribute__((constructor)) static void __init_plt()
{
    uint8_t* pc;

    // Evil anti-debugging
    if (ptrace(PTRACE_TRACEME, 0, 1, 0) == -1)
        return;

    for (pc = _binary_decrypt_enc_start; pc < _binary_decrypt_enc_end; pc++)
        *pc ^= 0x42;
}

__attribute__((always_inline)) static inline void decode_func(char* s, size_t size, uint32_t key)
{
    int i;
    uint32_t* w = (uint32_t*)s;

    for (i = 0; i < size / sizeof(uint32_t); i++)
        w[i] ^= key;
}

int main(int argc, char** argv)
{
    static char flag[25];
    uint8_t* jit;
    uint32_t key;
    size_t jit_size = _binary_decrypt_enc_end - _binary_decrypt_enc_start;

    if (argc != 2) {
        fprintf(stderr, "feed me flagzzz\n");
        return 1;
    }

    if (strlen(argv[1]) < 4) {
        fprintf(stderr, "this flag is too smol\n");
        return 1;
    }

    // Correct key is "ictf" -> 0x69637466
    key = ntohl(*(uint32_t *)(argv[1]));
    if (key != 0x69637466) {
        fprintf(stderr, "ewww... this flag is not T A S T Y\n");
        return 1;
    }

    jit = mmap(NULL, PAGE_SIZE, PROT_READ | PROT_WRITE | PROT_EXEC, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (jit == MAP_FAILED) {
        fprintf(stderr, "no memory :(\n");
        return 1;
    }

    memcpy(jit, _binary_decrypt_enc_start, jit_size);
    decode_func(jit, jit_size, key);

    if (crc32(jit, jit_size) != CRC32_JIT) {
        fprintf(stderr, "uggghhh... i dont feel so good\n");
        return 0;
    }

    decrypt_func = (void (*)(char*))jit;
    decrypt_func(flag);

    if (strncmp(flag, argv[1], sizeof(flag)) == 0) {
        printf("you win!\n");
        return 0;
    }

    fprintf(stderr, "lose :(\n");

    return 1;
}
