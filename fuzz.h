#ifndef DROPBEAR_FUZZ_H
#define DROPBEAR_FUZZ_H

#include "config.h"

#if DROPBEAR_FUZZ

#include "includes.h"
#include "buffer.h"
#include "algo.h"
#include "fuzz-wrapfd.h"

// once per process
void fuzz_common_setup(void);
void fuzz_svr_setup(void);

// must be called once per fuzz iteration. 
// returns DROPBEAR_SUCCESS or DROPBEAR_FAILURE
int fuzz_set_input(const uint8_t *Data, size_t Size);

int fuzz_run_preauth(const uint8_t *Data, size_t Size, int skip_kexmaths);
const void* fuzz_get_algo(const algo_type *algos, const char* name);

// fuzzer functions that intrude into general code
void fuzz_kex_fakealgos(void);
int fuzz_checkpubkey_line(buffer* line, int line_num, char* filename,
        const char* algo, unsigned int algolen,
        const unsigned char* keyblob, unsigned int keybloblen);
extern const char * const * fuzz_signkey_names;
void fuzz_seed(void);
void fuzz_get_socket_address(int fd, char **local_host, char **local_port,
                        char **remote_host, char **remote_port, int host_lookup);
void fuzz_fake_send_kexdh_reply(void);

// fake IO wrappers
#ifndef FUZZ_SKIP_WRAP
#define select(nfds, readfds, writefds, exceptfds, timeout) \
        wrapfd_select(nfds, readfds, writefds, exceptfds, timeout)
#define write(fd, buf, count) wrapfd_write(fd, buf, count)
#define read(fd, buf, count) wrapfd_read(fd, buf, count)
#define close(fd) wrapfd_close(fd)
#endif // FUZZ_SKIP_WRAP

struct dropbear_fuzz_options {
    int fuzzing;

    // fuzzing input
    buffer *input;
    struct dropbear_cipher recv_cipher;
    struct dropbear_hash recv_mac;
    int wrapfds;

    // whether to skip slow bignum maths
    int skip_kexmaths;

    // dropbear_exit() jumps back
    int do_jmp;
    sigjmp_buf jmp;

    uid_t pw_uid;
    gid_t pw_gid;
    char* pw_name;
    char* pw_dir;
    char* pw_shell;
    char* pw_passwd;
};

extern struct dropbear_fuzz_options fuzz;

#endif // DROPBEAR_FUZZ

#endif /* DROPBEAR_FUZZ_H */
