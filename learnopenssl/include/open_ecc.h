#ifndef OPEN_ECC_H
#define OPEN_ECC_H
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
#include <string>

class OpenEcc
{
public:
   enum EccErrorType
   {
        NOERROR = 0 
   };
public:
    OpenEcc()
    {}
    ~OpenEcc();

    EccErrorType initEccCurves(bool create_priv_key = false);
    
    EccErrorType importPrivKey(const std::string& str_priv_key);

    EccErrorType dumpPrivKey(std::string& str_priv_key);

    EccErrorType signMessage(const std::string& message, std::string& signature);

    bool verifyMessage(const std::string& message, const std::string& signature, const std::string& pub_key);

    EccErrorType calcECDHKey(const std::string& pub_key, std::string& ecdh_key);

    EccErrorType getPubKey(std::string& pub_key);

protected:

    EccErrorType strToPrivKey(const char* data);

    EccErrorType privKeyToStr(char** data);

    EccErrorType strToPubKey(const char* data, EC_POINT** pub_key);

protected:
    EC_KEY* priv_key_ ;
    EC_GROUP* group_ ;
    BIO* berr_;
    EC_builtin_curve *curves_;
};

#endif
