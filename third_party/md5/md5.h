#ifndef MD5_H
#define MD5_H

#include <cinttypes>
#include <fstream>
#include <stdint.h>
#include <string>
#include <string_view>


namespace md5 {

/* Type define */
typedef uint8_t ByteT;
typedef uint32_t U32T;

using std::ifstream;
using std::string;

/* MD5 declaration. */
class MD5 {
public:
    MD5();
    MD5(const void* input, size_t length);
    MD5(std::string_view str);
    MD5(ifstream& in);
    void update(const void* input, size_t length);
    void update(std::string_view str);
    void update(ifstream& in);
    const ByteT* digest();
    string toString();
    void reset();

private:
    void update(const ByteT* input, size_t length);
    void final();
    void transform(const ByteT block[64]);
    void encode(const U32T* input, ByteT* output, size_t length);
    void decode(const ByteT* input, U32T* output, size_t length);
    string bytesToHexString(const ByteT* input, size_t length);

    /* class uncopyable */
    MD5(const MD5&);
    MD5& operator=(const MD5&);

private:
    U32T _state[4]; /* state (ABCD) */
    U32T _count[2]; /* number of bits, modulo 2^64 (low-order word first) */
    ByteT _buffer[64]; /* input buffer */
    ByteT _digest[16]; /* message digest */
    bool _finished; /* calculate finished ? */

    static const ByteT PADDING[64]; /* padding for calculate */
    static const char HEX[16];
    static const size_t BUFFER_SIZE = 1024;
};

}

#endif /*MD5_H*/
