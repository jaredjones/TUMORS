//
//  OpenSSLCrypto.h
//  TUMORS
//
//  Created by Jared Jones on 12/26/13.
//  Copyright (c) 2013 Uvora. All rights reserved.
//

#ifndef OPENSSL_CRYPTO_H
#define OPENSSL_CRYPTO_H

/**
* A group of functions which setup openssl crypto module to work properly in multithreaded enviroment
* If not setup properly - it will crash
*/
namespace OpenSSLCrypto
{
    /// Needs to be called before threads using openssl are spawned
    void threadsSetup();
    /// Needs to be called after threads using openssl are despawned
    void threadsCleanup();
}

#endif