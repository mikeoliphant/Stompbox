/*
 * Author     :  Paul Kocher
 * E-mail     :  pck@netcom.com
 * Date       :  1997
 * Description:  C implementation of the Blowfish algorithm.

  CBC mode wrappers by Theo Niessink <http://www.taletn.com/>.

  To mimic OpenSSL enc -bf-cbc do something like:

	BLOWFISH_CBC_CTX ctx;
	int n;

	fprintf(stdout, "Salted__");
	fwrite(salt, 1, 8, stdout);

	Blowfish_CBC_Init(&ctx, pass, passLen, salt, 16);
	n = Blowfish_CBC_Encrypt(&ctx, buf, bufLen);

	fwrite(buf, 1, n, stdout);

*/

#ifndef _WDL_BLOWFISH_H_
#define _WDL_BLOWFISH_H_

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
  unsigned int P[16 + 2];
  unsigned int S[4*256];
} BLOWFISH_CTX;

void Blowfish_Init(BLOWFISH_CTX *ctx, const unsigned char *key, int keyLen);
void Blowfish_Encrypt(BLOWFISH_CTX *ctx, unsigned int *xl, unsigned int *xr);
void Blowfish_Decrypt(BLOWFISH_CTX *ctx, unsigned int *xl, unsigned int *xr);

typedef struct {
  BLOWFISH_CTX bf;
  unsigned int iv[2];
} BLOWFISH_CBC_CTX;

// Salt length is always 8 bytes. Key length should be 4..56 bytes.
void Blowfish_CBC_Init(BLOWFISH_CBC_CTX *ctx, const char *pass, int passLen, const unsigned char *salt, int keyLen);

// Both return the new buffer length. The encrypted buffer length will be up
// to 8 bytes larger than the unencrypted buffer length, i.e.:
// encryptedLen = ((unencryptedLen / 8) + 1) * 8;
int Blowfish_CBC_Encrypt(BLOWFISH_CBC_CTX *ctx, void *buf, int bufLen);
int Blowfish_CBC_Decrypt(BLOWFISH_CBC_CTX *ctx, void *buf, int bufLen);

#ifdef __cplusplus
};
#endif

#endif //_WDL_BLOWFISH_H_
