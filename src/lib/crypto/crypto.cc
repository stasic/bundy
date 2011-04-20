// Copyright (C) 2011  Internet Systems Consortium, Inc. ("ISC")
//
// Permission to use, copy, modify, and/or distribute this software for any
// purpose with or without fee is hereby granted, provided that the above
// copyright notice and this permission notice appear in all copies.
//
// THE SOFTWARE IS PROVIDED "AS IS" AND ISC DISCLAIMS ALL WARRANTIES WITH
// REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY
// AND FITNESS.  IN NO EVENT SHALL ISC BE LIABLE FOR ANY SPECIAL, DIRECT,
// INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM
// LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE
// OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
// PERFORMANCE OF THIS SOFTWARE.

#include "crypto.h"

#include <botan/botan.h>
#include <botan/hmac.h>
#include <botan/hash.h>
#include <botan/types.h>

#include <dns/buffer.h>
#include <dns/name.h>
#include <dns/util/base64.h>

#include <string>

#include <boost/scoped_ptr.hpp>

#include <iostream>

using namespace std;
using namespace isc::dns;


namespace isc {
namespace crypto {

// For Botan, we use the Crypto class object in RAII style
class CryptoImpl {
private:
    Botan::LibraryInitializer _botan_init;
};

Crypto::~Crypto() {
    delete impl_;
}

Crypto&
Crypto::getCrypto() {
    Crypto &c = getCryptoInternal();
    if (!c.impl_) {
        c.initialize();
    }
    return c;
}

Crypto&
Crypto::getCryptoInternal() {
    static Crypto instance;
    return (instance);
}

void
Crypto::initialize() {
    Crypto& c = getCryptoInternal();
    try {
        c.impl_ = new CryptoImpl();
    } catch (const Botan::Exception& ex) {
        isc_throw(InitializationError, ex.what());
    }
}

HMAC*
Crypto::createHMAC(const void* secret, size_t secret_len,
                   const HMAC::HashAlgorithm hash_algorithm) {
    return new HMAC(secret, secret_len, hash_algorithm);
}

void
signHMAC(const void* data, size_t data_len, const void* secret,
         size_t secret_len, const HMAC::HashAlgorithm hash_algorithm,
         isc::dns::OutputBuffer& result, size_t len)
{
    boost::scoped_ptr<HMAC> hmac(Crypto::getCrypto().createHMAC(secret, secret_len, hash_algorithm));
    hmac->update(data, data_len);
    hmac->sign(result, len);
}


bool
verifyHMAC(const void* data, const size_t data_len, const void* secret,
           size_t secret_len, const HMAC::HashAlgorithm hash_algorithm,
           const void* sig, const size_t sig_len)
{
    boost::scoped_ptr<HMAC> hmac(Crypto::getCrypto().createHMAC(secret, secret_len, hash_algorithm));
    hmac->update(data, data_len);
    return (hmac->verify(sig, sig_len));
}

} // namespace crypto
} // namespace isc

