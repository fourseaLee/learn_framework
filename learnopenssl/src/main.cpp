#include <string.h>
#include <stdio.h>
#include <openssl/ec.h>
#include <openssl/ecdh.h>
#include <openssl/ecdsa.h>
#include <openssl/objects.h>
#include <openssl/err.h>
#include <openssl/aes.h>
#include <openssl/pem.h>
#include <openssl/bio.h>
#include "open_ecc.h"
#include <iostream>
static EC_KEY* send_key;
static EC_KEY* recieve_key;
void send(int step)
{
    if(step == 0)
    {
        
            
    }
    else if(step == 1)
    {

    }
 
}

void transfer(int step, const char* data, size_t len)
{
    EC_KEY* transfer_key ;
    //int ret = EC_KEY_otc2priv(key, data, len);
    

}
const signed char p_util_hexdigit[256] = {
    -1, -1,  -1,  -1,  -1,  -1,  -1,  -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1,  -1,  -1,  -1,  -1,  -1,  -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1,  -1,  -1,  -1,  -1,  -1,  -1, -1, -1, -1, -1, -1, -1, -1, -1,
    0,  1,   2,   3,   4,   5,   6,   7,  8,  9,  -1, -1, -1, -1, -1, -1,
    -1, 0xa, 0xb, 0xc, 0xd, 0xe, 0xf, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1,  -1,  -1,  -1,  -1,  -1,  -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, 0xa, 0xb, 0xc, 0xd, 0xe, 0xf, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1,  -1,  -1,  -1,  -1,  -1,  -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1,  -1,  -1,  -1,  -1,  -1,  -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1,  -1,  -1,  -1,  -1,  -1,  -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1,  -1,  -1,  -1,  -1,  -1,  -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1,  -1,  -1,  -1,  -1,  -1,  -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1,  -1,  -1,  -1,  -1,  -1,  -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1,  -1,  -1,  -1,  -1,  -1,  -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1,  -1,  -1,  -1,  -1,  -1,  -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1,  -1,  -1,  -1,  -1,  -1,  -1, -1, -1, -1, -1, -1, -1, -1, -1,
};



signed char HexDigit(char c) 
{
    return p_util_hexdigit[(unsigned char)c];
}


void HexToBin(const char* hex, unsigned char* bin)
{ 
    
    int counter = 0;
    for (int i = 0 ; i < 32; i++)
    {
        printf("%x ", hex[i]);
        unsigned char v =(unsigned char)hex[i];

        bin[counter] = v;
        counter ++;
    }

    printf("\n");

    
}


void
hex_to_bin(const char *hex, const size_t hex_len,
        unsigned char *bin, const size_t bin_size)
{
    //assert(hex_len % 2 == 0);
    //assert(bin_size >= hex_len >> 1);
    const char *ph = hex;
    unsigned char *end = bin + bin_size;
    while (*ph && bin < end)
    {
        sscanf(ph, "%2hhx", bin++);
        ph += 2;
    }
}


static bool hex_convert_memory(const unsigned char *m ,size_t n, char *b, size_t width)
{
    size_t i;

    for (i = 0; i < n; i++) 
    {
        const unsigned char c = *m++;

        *b++ = "0123456789abcdef"[c >> 4];
        *b++ = "0123456789abcdef"[c & 15];
        if (i % width == width - 1 && i != n - 1)
            *b++ = ' ';
    }
    *b = '\0';

}

void PrivKeyToStr(EC_KEY* priv_key, char** data)
{
    const BIGNUM* key_bn = EC_KEY_get0_private_key(priv_key);
    *data = BN_bn2hex(key_bn);
    printf("%s \n", *data);
}

void StrToPrivKey(const char* data, EC_KEY* priv_key)
{
   BIGNUM* bn = BN_new();
   int ret = BN_hex2bn(&bn, data);
   EC_KEY_set_private_key(priv_key,bn); 
}

void PubKeyToStr(const EC_POINT* pubkey, char** data)
{
    EC_GROUP* group; 
    int nid = 722;
    group = EC_GROUP_new_by_curve_name(nid);
    BN_CTX* bn_ctx = NULL;
    *data = EC_POINT_point2hex(group, pubkey, POINT_CONVERSION_COMPRESSED, bn_ctx);
    printf("%s \n", *data);

}

void PubKeyToStr1(const EC_POINT* pubkey, const EC_GROUP* group, char** data)
{

    BN_CTX* bn_ctx = NULL;
    *data = EC_POINT_point2hex(group, pubkey, POINT_CONVERSION_COMPRESSED, bn_ctx);
    printf("%s \n", *data);

}
void StrToPubKey(const char* data, EC_POINT* pubkey)
{
    printf(" strtopubkey %s \n", data); 
    EC_GROUP* group;
    int nid = 722;
    group = EC_GROUP_new_by_curve_name(nid);
    BN_CTX* bn_ctx = NULL;
    pubkey = EC_POINT_new(group); 
    EC_POINT_hex2point(group, data, pubkey, bn_ctx);

    char* buf;
    PubKeyToStr(pubkey, &buf);
}

void PrintStrHex(const std::string& data)
{
    int size = data.size();
    unsigned char* data_byte = (unsigned char*)data.c_str();
    for (int i = 0; i < 32; i++)
    {
        printf("%x", data_byte[i]);
    }
    printf("\n");
}

void TestOpenEcc()
{
    OpenEcc open_ecc;
    open_ecc.initEccCurves(true);

    std::cout << "priv key: " << std::endl;
    std::string priv_key ;
    open_ecc.dumpPrivKey(priv_key);
    std::cout << priv_key << std::endl;

    std::string message = "asdaescaeesafawwqgavcq2123bwjuert";
    std::string signature;
    open_ecc.signMessage(message, signature);
    std::cout << "signature: \n" ;
    PrintStrHex(signature);
    open_ecc.signMessage(message, signature);
    std::cout << "signature: \n" ;
    PrintStrHex(signature);

    std::string pub_key;
    open_ecc.getPubKey(pub_key);
    std::cout << "pub key: " << std::endl;
    std::cout << pub_key << std::endl;

    open_ecc.verifyMessage(message, signature, pub_key);
    std::string message_error = "adfafafwedaceq121322134321";
    open_ecc.verifyMessage(message_error, signature, pub_key);
    std::cout << "verify " << std::endl;

    std::string from_key = "0207408978889F432ECFB0409827FBA813128ED73B";
    std::cout << "from key" << std::endl;
    std::cout << from_key << std::endl;
    open_ecc.importPrivKey(from_key);
    std::string signature1;
    open_ecc.signMessage(message, signature1);
    std::cout << "signature: \n" ;
    PrintStrHex(signature1);
    open_ecc.verifyMessage(message, signature1, pub_key);
  
    open_ecc.signMessage(message, signature);
    std::cout << "signature: \n" ;
    PrintStrHex(signature);
    std::cout << "diff verify" << std::endl;
    open_ecc.verifyMessage(message, signature, pub_key);

    std::string pub_key1;
    open_ecc.getPubKey(pub_key1);
    std::cout << "pub key: " << std::endl;
    std::cout << pub_key1 << std::endl;

    std::string from_priv_key;
    open_ecc.dumpPrivKey(from_priv_key);
    std::cout << "from priv key" << std::endl;
    std::cout << from_priv_key << std::endl;

    std::string ecdh_key;

    pub_key = "030583A682FDCC91086F3BB90004EF8E864A2EEA1245";
    open_ecc.calcECDHKey(pub_key, ecdh_key);
    std::cout << "ecdh key size: " << ecdh_key.length() <<std::endl;
//    std::cout << ecdh_key << std::endl;
    
    unsigned char* ecdh_data = (unsigned char*)ecdh_key.c_str();
    for (int i = 0; i < 32; i++)
    {
        printf("%x", ecdh_data[i]);
    }
    std::cout << std::endl;

    
}


int main()
{
    TestOpenEcc();
    return 0;
    EC_KEY*key1, *key2;
    const EC_POINT *pubkey1, *pubkey2;
    EC_GROUP *group1, *group2;

    unsigned int ret,nid,size,i,sig_len;
    unsigned char *signature,digest[20];

    BIO *berr;
    EC_builtin_curve *curves;
    int crv_len;
    unsigned char shareKey1[32] = {0};
    unsigned char shareKey2[32] = {0};

    int len1,len2;

    key1 = EC_KEY_new();

    if(key1 == NULL)
    {
        printf("EC_KEY_new error! \n");
        return -1;
    }

    key2 = EC_KEY_new();

    if(key2 == NULL)
    {
        printf("EC_KEY_new error! \n");
        return -1;
    }

    crv_len = EC_get_builtin_curves(NULL, 0);

    curves = (EC_builtin_curve *)malloc(sizeof(EC_builtin_curve)* crv_len);

    EC_get_builtin_curves(curves, crv_len);

    nid = curves[25].nid;

    printf("nid: %d \n", nid);
    group1 = EC_GROUP_new_by_curve_name(nid);

    if(group1 == NULL)
    {
        printf("EC_GROUP_new_by_curve_name error! \n");
        return -1;
    }
    
    group2 = EC_GROUP_new_by_curve_name(nid);
    if(group2 == NULL)
    {
        printf("EC_GROUP_new_by_curve_name error! \n");
        return -1;
    }

    ret = EC_KEY_set_group(key1, group1);

    if (ret != 1)
    {
        printf("EC_KEY_set_group error,. \n");
        return -1;
    }

    ret = EC_KEY_set_group(key2, group2);

    if (ret != 1)
    {
        printf("EC_KEY_set_group error,. \n");
        return -1;
    }

    ret = EC_KEY_generate_key(key1);
    if(ret != 1)
    {
        printf("EC_KEY_generate_key error. \n");
        return -1;
    }
    
    char* data1;
    PrivKeyToStr(key1, &data1);

    ret = EC_KEY_generate_key(key2);
    if(ret != 1)
    {
        printf("EC_KEY_generate_key error. \n");
        return -1;
    }

    ret = EC_KEY_check_key(key1);

    if(ret != 1)
    {
        printf("check key error.\n");
        return -1;
    }
    size = ECDSA_size(key1);
    printf("size %d \n", size);

    for(i = 0; i < 20; i++)
        memset(&digest[i], i+1, 1);
    signature = (unsigned char*)malloc(size);

    ERR_load_crypto_strings();

    berr = BIO_new(BIO_s_file());

    ret = ECDSA_sign(0, digest, 20, signature, &sig_len, key1);

    if(ret != 1)
    {
        ERR_print_errors(berr);
        printf("sign error! \n");
        return -1;
    }

    ret = ECDSA_verify(0, digest, 20, signature, sig_len, key1);
    if (ret != 1)
    {
        ERR_print_errors(berr);
        printf("ECDSA_verify error \n");
        return -1;
    }

    pubkey2 = EC_KEY_get0_public_key(key2);

    len1 = ECDH_compute_key(shareKey1, 32, pubkey2, key1, NULL);
    pubkey1 = EC_KEY_get0_public_key(key1);

    char* pub_data;
    EC_POINT *pubkey11; 
    PubKeyToStr(pubkey1, &pub_data);

    StrToPubKey(pub_data,pubkey11);
    len2 = ECDH_compute_key(shareKey2, 32, pubkey1, key2, NULL);

    if (len1 != len2)
    {
        printf("error \n");
    }
    else
    {
        printf("share key1 len: %d \n", sizeof(shareKey1));
        printf("share key2 len: %d \n", sizeof(shareKey2));
        for(int i = 0; i < 32; i++)
        {
            printf("%02x ", shareKey1[i]);
        }

        printf("\n");
        for(int i = 0; i < 32; i++)
        {
            printf("%02x ", shareKey2[i]);
        }

        printf("\n");
        ret = memcmp(shareKey1, shareKey2, len1);

        if (ret == 0)
            printf("create success! \n");
        else
            printf("create failed! \n");
    }

    printf("test ok! \n");
    
    unsigned char shareKey[64];
    
    //HexToBin(shareKey1,shareKey);

    //unsigned char shortKey[32];
/*
    for (int i =0; i < 32; i++)
    {
        shortKey[i] = shareKey[i];
        printf("%x", shareKey[i]);
    }
    printf("\n");
*/
    AES_KEY enc_key;
    const int key_bits = sizeof(shareKey1) / sizeof(shareKey1[0]) * 8;
    ret = AES_set_encrypt_key(shareKey1, key_bits, &enc_key);

    if(ret != 0)
    {
        printf("init aes key failed! key size: %d\n", key_bits);
    }


    BIO_free(berr);
    EC_KEY_free(key1);
    EC_KEY_free(key2);
    free(signature);
    free(curves);

  //  int ok = AES_set_encrypt();

    return 0;

}



