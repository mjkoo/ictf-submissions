#define _GNU_SOURCE

#include <fcntl.h>
#include <libgen.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>

#include "lodepng.h"
#include <openssl/evp.h>

#define ELF_FOLDER "\177ELF"
#define IN_FILENAME "secrets.png"
#define OUT_FILENAME "d00rz_0f_dur1n.pdf"
#define KEY "g@nd@1f"
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define ITER 10000

static char *prefix = NULL;
static char *infile = NULL;
static char *outfile = NULL;

enum errors {
    FILE_NOT_FOUND,
    CANT_DECODE_PNG,
    WRONG_PASSWORD,
    BAD_CIPHER,
};

static void die(enum errors error)
{
    switch (error) {
    case FILE_NOT_FOUND:
        printf("'Well, here we are and all ready,' said Merry; 'but where are the Doors? I can't see any sign of "
               "them.'\n");
        break;
    case CANT_DECODE_PNG:
        printf("'What does the writing say?' asked Frodo, who was trying to decipher the inscription on the arch.\n");
        break;
    case WRONG_PASSWORD:
        printf("'Dwarf-doors are not made to be seen when shut,' said Gimli. 'They are invisible, and their own makers "
               "cannot find them or open them, if their secret is forgotten.'\n");
        break;
    case BAD_CIPHER:
        printf("'I thought I knew the elf-letters, but I cannot read these.'\n");
        break;
    }

    exit(error);
}

static void check_password(const char* password)
{
    int i;
    size_t password_len;

    uint8_t expected[] = { 0x2e, 0x6e, 0xe0, 0x6d, 0x61, 0xf1, 0x3b, 0x6e, 0x21 };
    uint8_t indices[] = { 8, 0, 5, 6, 2, 7, 4, 3, 1 };

    password_len = strlen(password);
    if (password_len != sizeof(expected))
        die(WRONG_PASSWORD);

    for (i = 0; i < password_len; i++) {
        int j = indices[i];
        uint8_t c = password[j];
        c ^= j;

        c -= 7;
        c += 66;
        c -= 14;
        c = (c >> 7) | (c << (1));
        c -= 149;
        c ^= 167;
        c ^= 168;
        c = ~c;
        c -= 177;
        c = (c << 4) | (c >> (4));
        c -= 97;
        c += 132;
        c ^= 2;
        c += 119;
        c = ~c;
        c ^= 76;
        c += 202;
        c = ~c;
        c = (c >> 7) | (c << (1));
        c += 80;

        if (c != expected[i])
            die(WRONG_PASSWORD);
    }
}

static void decrypt_stage2(uint8_t* ct, size_t ct_len)
{
    int i;
    char password[256] = {};
    size_t xor_len, key_len, iv_len;
    char salt[8];
    char key[EVP_MAX_KEY_LENGTH];
    char iv[EVP_MAX_IV_LENGTH];
    char key_iv_pair[EVP_MAX_KEY_LENGTH + EVP_MAX_IV_LENGTH];
    char* pt = NULL;
    int pt_len, pad_len;
    const EVP_CIPHER* cipher;
    EVP_CIPHER_CTX* ctx = NULL;
    FILE* fp;

    printf("'But this Door was not made to be a secret known only to Dwarves,' said Gandalf, coming suddenly to life "
           "and turning round.\n");
    printf("'Unless things are altogether changed, eyes that know what to look for may discover the signs.'\n");
    printf("\n> ");
    fflush(stdout);
    fgets(password, sizeof(password), stdin);
    password[strcspn(password, "\n")] = '\x00';
    check_password(password);

    // Here we know our password is right, lets start decrypting
    xor_len = strlen(KEY);
    for (i = 0; i < ct_len; i++) {
        ct[i] ^= KEY[i % xor_len];
    }

    if (ct_len >= 16 && strncmp(ct, "Salted__", 8) == 0) {
        memcpy(salt, &ct[8], 8);
        ct += 16;
        ct_len -= 16;
    }

    cipher = EVP_aes_256_cbc();
    key_len = EVP_CIPHER_key_length(cipher);
    iv_len = EVP_CIPHER_iv_length(cipher);

    PKCS5_PBKDF2_HMAC(password, -1, salt, 8, ITER, EVP_sha256(), key_len + iv_len, key_iv_pair);
    memcpy(key, key_iv_pair, key_len);
    memcpy(iv, key_iv_pair + key_len, iv_len);

    ctx = EVP_CIPHER_CTX_new();
    if (!ctx)
        goto bad_cipher;

    if (EVP_DecryptInit_ex(ctx, cipher, NULL, key, iv) != 1)
        goto bad_cipher;

    pt = calloc(ct_len, 1);
    if (!pt)
        goto bad_cipher;

    if (EVP_DecryptUpdate(ctx, pt, &pt_len, ct, ct_len) != 1)
        goto bad_cipher;

    if (EVP_DecryptFinal(ctx, pt + pt_len, &pad_len) != 1)
        goto bad_cipher;

    pt_len += pad_len;

    fp = fopen(outfile, "wb");
    if (!fp)
        die(FILE_NOT_FOUND);

    fwrite(pt, pt_len, 1, fp);
    fclose(fp);

    free(pt);
    EVP_CIPHER_CTX_free(ctx);

    return;

bad_cipher:
    free(pt);
    EVP_CIPHER_CTX_free(ctx);
    die(BAD_CIPHER);
}

int main(int argc, char** argv)
{
    int i;
    unsigned error;
    unsigned width, height;
    uint8_t* png = NULL;
    uint8_t* image = NULL;
    size_t pngsize;

    LodePNGState state;

    // argv[0] from loader should point to tar file, see if ELF folder exists
    asprintf(&prefix, "%s/%s", dirname(argv[0]), ELF_FOLDER);
    asprintf(&infile, "%s/%s", prefix, IN_FILENAME);
    asprintf(&outfile, "%s/%s", prefix, OUT_FILENAME);

    // Tar file has borked permissions for this dir, if it exists but hasn't
    // been fixed lets fix it
    chmod(prefix, 0755);
    if (access(infile, R_OK) != 0)
        die(FILE_NOT_FOUND);

    lodepng_state_init(&state);
    error = lodepng_load_file(&png, &pngsize, infile);
    if (!error)
        error = lodepng_decode(&image, &width, &height, &state, png, pngsize);
    if (error)
        die(CANT_DECODE_PNG);

    free(png);

    for (i = 0; i < state.info_png.text_num; i++) {
        if (strncmp(state.info_png.text_keys[i], OUT_FILENAME, strlen(OUT_FILENAME) - 4) == 0) {
            decrypt_stage2(state.info_png.text_strings[i], state.info_png.text_sizes[i]);
        }
    }

    lodepng_state_cleanup(&state);

    free(image);

    free(prefix);
    free(infile);
    free(outfile);

    printf("'Look!' he said. 'Can you see anything now?'\n");
    return 0;
}
