#include "open_ecc.h"

typedef OpenEcc::EccErrorType  ErrorType_t;

OpenEcc::~OpenEcc()
{

}

ErrorType_t OpenEcc::initEccCurves(bool create_priv_key /*= false*/)
{ 
    int crv_len = EC_get_builtin_curves(NULL, 0);
    curves_ = (EC_builtin_curve *)malloc(sizeof(EC_builtin_curve)* crv_len);
    EC_get_builtin_curves(curves_, crv_len);
    priv_key_ = EC_KEY_new(); 
    unsigned int nid;
    nid = curves_[25].nid;
    group_ = EC_GROUP_new_by_curve_name(nid);
    EC_KEY_set_group(priv_key_, group_);

    EC_KEY_generate_key(priv_key_);

    return NOERROR;
}

ErrorType_t OpenEcc::strToPrivKey(const char* data)
{
    BIGNUM* bn = BN_new();
    int ret = BN_hex2bn(&bn, data);
    EC_KEY_free(priv_key_);
    priv_key_ = NULL;
    priv_key_ = EC_KEY_new();
    unsigned int nid;
    nid = curves_[25].nid;
    group_ = EC_GROUP_new_by_curve_name(nid);
    ret = EC_KEY_set_group(priv_key_, group_);

    EC_KEY_generate_key(priv_key_);

    EC_KEY_set_private_key(priv_key_,bn); 
    return NOERROR;
}

ErrorType_t OpenEcc::importPrivKey(const std::string& str_priv_key)
{
    strToPrivKey(str_priv_key.c_str()); 
    return NOERROR;
}

ErrorType_t OpenEcc::privKeyToStr(char** data)
{
    const BIGNUM* key_bn = EC_KEY_get0_private_key(priv_key_);
    *data = BN_bn2hex(key_bn);
    return NOERROR;
}

ErrorType_t OpenEcc::dumpPrivKey(std::string& str_priv_key)
{
    char* data;
    privKeyToStr(&data);
    str_priv_key = data;
    return NOERROR;
}

ErrorType_t OpenEcc::signMessage(const std::string& message, std::string& signature)
{
    BIO *berr;
    int size = ECDSA_size(priv_key_);
    unsigned char* signature_byte = (unsigned char*)malloc(size);
    unsigned char* message_byte = (unsigned char*)message.c_str();

    ERR_load_crypto_strings();
    berr = BIO_new(BIO_s_file());
    unsigned int sig_len;

    ECDSA_sign(0, message_byte, message.size(), signature_byte, &sig_len, priv_key_);

    signature = std::string((char*)signature_byte, sig_len);
    return NOERROR;
}

bool OpenEcc::verifyMessage(const std::string& message, const std::string& signature, const std::string& pub_key)
{
    unsigned int sig_len = signature.size();
    unsigned char* message_byte = (unsigned char*)message.c_str();
    unsigned char* signature_byte = (unsigned char*)signature.c_str(); 
    int ret = ECDSA_verify(0, message_byte, message.size(), signature_byte, sig_len,priv_key_);
    if (ret != 1)
    {
       // ERR_print_errors(berr);
        printf("ECDSA_verify error \n");
        return false;
    }


    return true;
}

ErrorType_t OpenEcc::getPubKey(std::string& pub_key)
{
   const EC_POINT* ecc_pub_key;
   ecc_pub_key = EC_KEY_get0_public_key(priv_key_);
   BN_CTX* bn_ctx = NULL;

   char* data;
   data = EC_POINT_point2hex(group_, ecc_pub_key, POINT_CONVERSION_COMPRESSED, bn_ctx);
   pub_key = data;
   return NOERROR;
}

ErrorType_t OpenEcc::strToPubKey(const char* data, EC_POINT** pub_key)
{
    BN_CTX*  bn_ctx = NULL;
    *pub_key = EC_POINT_new(group_);
    EC_POINT_hex2point(group_, data, *pub_key, bn_ctx);
    return NOERROR;
}

ErrorType_t OpenEcc::calcECDHKey(const std::string& pub_key, std::string& ecdh_key)
{
    unsigned char share_key[32];
    EC_POINT* ecc_pub_key;
    
    strToPubKey(pub_key.c_str(), &ecc_pub_key);
    ECDH_compute_key(share_key, 32, ecc_pub_key, priv_key_, NULL);

    ecdh_key = std::string((char*)share_key,32);
    return NOERROR;
}

