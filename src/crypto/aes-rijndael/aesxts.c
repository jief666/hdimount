/*
 * Copyright (c) 2010 Apple Inc. All Rights Reserved.
 * 
 * @APPLE_LICENSE_HEADER_START@
 * 
 * This file contains Original Code and/or Modifications of Original Code
 * as defined in and that are subject to the Apple Public Source License
 * Version 2.0 (the 'License'). You may not use this file except in
 * compliance with the License. Please obtain a copy of the License at
 * http://www.opensource.apple.com/apsl/ and read it before using this
 * file.
 * 
 * The Original Code and all software distributed under the License are
 * distributed on an 'AS IS' basis, WITHOUT WARRANTY OF ANY KIND, EITHER
 * EXPRESS OR IMPLIED, AND APPLE HEREBY DISCLAIMS ALL SUCH WARRANTIES,
 * INCLUDING WITHOUT LIMITATION, ANY WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE, QUIET ENJOYMENT OR NON-INFRINGEMENT.
 * Please see the License for the specific language governing rights and
 * limitations under the License.
 * 
 * @APPLE_LICENSE_HEADER_END@
 */

#include "../aes-rijndael/aesxts.h"

#include <stdint.h>
#include <string.h>
#include <stdlib.h>


int 
aes_encrypt_key(const uint8_t *key, int key_len, aesedp_encrypt_ctx cx[1]);

int 
aes_decrypt_key(const uint8_t *key, int key_len, aesedp_decrypt_ctx cx[1]);

int
aes_encrypt(const uint8_t *Plaintext, uint8_t *Ciphertext, aesedp_encrypt_ctx *ctx);

int
aes_decrypt(const uint8_t *Ciphertext, uint8_t *Plaintext, aesedp_decrypt_ctx *ctx);


/* error codes [will be expanded in future releases] */
enum {
   CRYPT_OK=0,             /* Result OK */
   CRYPT_ERROR=1,            /* Generic Error */
   CRYPT_INVALID_KEYSIZE=3,  /* Invalid key size given */
   CRYPT_INVALID_ARG=16,      /* Generic invalid argument */
};

static int 
aesedp_keysize(int *keysize)
{
	switch (*keysize) {
		case 16:
		case 24:
		case 32:
			return CRYPT_OK;
		default:
			return CRYPT_INVALID_KEYSIZE;
	}
}

static int 
aesedp_setup(const uint8_t *key, int keylen, int num_rounds, aesedp_ctx *skey)
{
	aesedp_ctx *ctx = (aesedp_ctx *) skey;
	int retval;

	if((retval = aesedp_keysize(&keylen)) != CRYPT_OK) return retval;
	if((retval = aes_encrypt_key(key, keylen, &ctx->encrypt)) != CRYPT_OK) return CRYPT_ERROR;
	if((retval = aes_decrypt_key(key, keylen, &ctx->decrypt)) != CRYPT_OK) return CRYPT_ERROR;
	return CRYPT_OK;
}

static void 
aesedp_done(aesedp_ctx *skey)
{
}

/** Start XTS mode
   @param cipher      The index of the cipher to use
   @param key1        The encrypt key
   @param key2        The tweak encrypt key
   @param keylen      The length of the keys (each) in octets
   @param num_rounds  The number of rounds for the cipher (0 == default)
   @param xts         [out] XTS structure
   Returns CRYPT_OK upon success.
*/

uint32_t
xts_start(uint32_t cipher, // ignored - we're doing this for xts-aes only
						const uint8_t *IV, // ignored
						const uint8_t *key1, int keylen,
						const uint8_t *key2, int tweaklen, // both keys are the same size for xts
						uint32_t num_rounds, // ignored
						uint32_t options,    // ignored
						symmetric_xts *xts)
{
   uint32_t err;

   /* check inputs */
   if((key1 == 0)|| (key2 == 0) || (xts == 0)) return CRYPT_INVALID_ARG;

   /* schedule the two ciphers */
   if ((err = aesedp_setup(key1, keylen, num_rounds, &xts->key1)) != 0) {
      return err;
   }
   if ((err = aesedp_setup(key2, keylen, num_rounds, &xts->key2)) != 0) {
      return err;
   }
   xts->cipher = cipher;

   return err;
}




/** multiply by x 
  @param I      The value to multiply by x (LFSR shift)
*/
static void xts_mult_x(uint8_t *I)
{
  uint32_t x;
  uint8_t t, tt;

  for (x = t = 0; x < 16; x++) {
     tt   = I[x] >> 7;
     I[x] = ((I[x] << 1) | t) & 0xFF;
     t    = tt;
  }
  if (tt) {
     I[0] ^= 0x87;
  } 
}

static int tweak_crypt(const uint8_t *P, uint8_t *C, uint8_t *T, aesedp_encrypt_ctx *ctx)
{
   uint32_t x;
   uint32_t err;

   /* tweak encrypt block i */
   for (x = 0; x < 16; x += sizeof(uint64_t)) {
      *((uint64_t*)&C[x]) = *((uint64_t*)&P[x]) ^ *((uint64_t*)&T[x]);
   }
     
   if ((err = aes_encrypt(C, C, ctx)) != CRYPT_OK) {
      return CRYPT_INVALID_KEYSIZE;
   }

   for (x = 0; x < 16; x += sizeof(uint64_t)) {
      *((uint64_t*)&C[x]) ^= *((uint64_t*)&T[x]);
   }

   /* LFSR the tweak */
   xts_mult_x(T);

   return CRYPT_OK;
}   

/** XTS Encryption
  @param pt     [in]  Plaintext
  @param ptlen  Length of plaintext (and ciphertext)
  @param ct     [out] Ciphertext
  @param tweak  [in] The 128--bit encryption tweak (e.g. sector number)
  @param xts    The XTS structure
  Returns CRYPT_OK upon success
*/
int xts_encrypt(
   const uint8_t *pt, unsigned long ptlen,
         uint8_t *ct,
   const uint8_t *tweak,
         symmetric_xts *xts)
{
   aesedp_encrypt_ctx *encrypt_ctx = &xts->key1.encrypt;
	uint8_t PP[16]	= {0};
   uint8_t CC[16]	= {0};
   uint8_t T[16]	= {0};
   uint32_t i, m, mo, lim;
   uint32_t err;

   /* check inputs */
   if((pt == 0) || (ct == 0)|| (tweak == 0) || (xts == 0)) return 1;

   /* get number of blocks */
   m  = ptlen >> 4;
   mo = ptlen & 15;

	/* must have at least one full block */
   if (m == 0) {
      return CRYPT_INVALID_ARG;
   }

   /* encrypt the tweak */
   if ((err = aes_encrypt(tweak, T, &xts->key2.encrypt)) != 0) {
      return CRYPT_INVALID_KEYSIZE;
   }

   /* for i = 0 to m-2 do */
   if (mo == 0) {
      lim = m;
   } else {
      lim = m - 1;
   }

   for (i = 0; i < lim; i++) {
      err = tweak_crypt(pt, ct, T, encrypt_ctx);
      ct += 16;
      pt += 16;
   }
   
   /* if ptlen not divide 16 then */
   if (mo > 0) {
      /* CC = tweak encrypt block m-1 */
      if ((err = tweak_crypt(pt, CC, T, encrypt_ctx)) != 0) {
         return err;
      }

      /* Cm = first ptlen % 16 bytes of CC */
      for (i = 0; i < mo; i++) {
          PP[i] = pt[16+i];
          ct[16+i] = CC[i];
      }

      for (; i < 16; i++) {
          PP[i] = CC[i];
      }

      /* Cm-1 = Tweak encrypt PP */
      if ((err = tweak_crypt(PP, ct, T, encrypt_ctx)) != 0) {
         return err;
      }
   }

   return err;
}

static int tweak_uncrypt(const uint8_t *C, uint8_t *P, uint8_t *T, aesedp_decrypt_ctx *ctx)
{
   uint32_t x;
   uint32_t err;

   /* tweak encrypt block i */
   for (x = 0; x < 16; x += sizeof(uint64_t)) {
      *((uint64_t*)&P[x]) = *((uint64_t*)&C[x]) ^ *((uint64_t*)&T[x]);
   }
     
   err = aes_decrypt(P, P, ctx);  

   for (x = 0; x < 16; x += sizeof(uint64_t)) {
      *((uint64_t*)&P[x]) ^=  *((uint64_t*)&T[x]);
   }

   /* LFSR the tweak */
   xts_mult_x(T);

   return err;
}

/** XTS Decryption
  @param ct     [in] Ciphertext
  @param ptlen  Length of plaintext (and ciphertext)
  @param pt     [out]  Plaintext
  @param tweak  [in] The 128--bit encryption tweak (e.g. sector number)
  @param xts    The XTS structure
  Returns CRYPT_OK upon success
*/

int xts_decrypt(
   const uint8_t *ct, unsigned long ptlen,
         uint8_t *pt,
   const uint8_t *tweak,
         symmetric_xts *xts)
{
   aesedp_decrypt_ctx *decrypt_ctx = &xts->key1.decrypt;
	uint8_t PP[16]	= {0};
   uint8_t CC[16]	= {0};
   uint8_t T[16]	= {0};
   uint32_t i, m, mo, lim;
   uint32_t err;

   /* check inputs */
   if((pt == 0) || (ct == 0)|| (tweak == 0) || (xts == 0)) return 1;

   /* get number of blocks */
   m  = ptlen >> 4;
   mo = ptlen & 15;

   /* must have at least one full block */
   if (m == 0) {
      return CRYPT_INVALID_ARG;
   }

   /* encrypt the tweak , yes - encrypt */
   if ((err = aes_encrypt(tweak, T, &xts->key2.encrypt)) != 0) {
      return CRYPT_INVALID_KEYSIZE;
   }

   /* for i = 0 to m-2 do */
   if (mo == 0) {
      lim = m;
   } else {
      lim = m - 1;
   }

   for (i = 0; i < lim; i++) {
      if ((err = tweak_uncrypt(ct, pt, T, decrypt_ctx)) != CRYPT_OK) {
		  return err;
	  }
      ct += 16;
      pt += 16;
   }
   
   /* if ptlen not divide 16 then */
   if (mo > 0) {
      memcpy(CC, T, 16);
      xts_mult_x(CC);

      /* PP = tweak decrypt block m-1 */
      if ((err = tweak_uncrypt(ct, PP, CC, decrypt_ctx)) != CRYPT_OK) {
        return err;
      }

      /* Pm = first ptlen % 16 bytes of PP */
      for (i = 0; i < mo; i++) {
          CC[i]    = ct[16+i];
          pt[16+i] = PP[i];
      }
      for (; i < 16; i++) {
          CC[i] = PP[i];
      }

      /* Pm-1 = Tweak uncrypt CC */
      if ((err = tweak_uncrypt(CC, pt, T, decrypt_ctx)) != CRYPT_OK) {
        return err;
      }
   }

   return CRYPT_OK;
}



void xts_done(symmetric_xts *xts)
{
   if(xts == 0) return;
   aesedp_done(&xts->key1);
   aesedp_done(&xts->key2);
}

