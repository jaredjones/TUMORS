//
//  OpenSSLCrypto.cpp
//  TUMORS
//
//  Created by Jared Jones on 12/26/13.
//  Copyright (c) 2013 Uvora. All rights reserved.
//

#include "OpenSSLCrypto.h"
#include <openssl/crypto.h>
#include <openssl/crypto.h>
#include <mutex>
#include <thread>
#include <vector>
std::vector<std::mutex*> cryptoLocks;

static void lockingCallback(int mode, int type, const char * /*file*/, int /*line*/)
{
    if (mode & CRYPTO_LOCK)
        cryptoLocks[type]->lock();
    else
        cryptoLocks[type]->unlock();
}

static void threadIdCallback(CRYPTO_THREADID *id)
{
    CRYPTO_THREADID_set_numeric(id, std::hash<std::thread::id>()(std::this_thread::get_id()));
}

void OpenSSLCrypto::threadsSetup()
{
    cryptoLocks.resize(CRYPTO_num_locks());
    
    for (int i = 0; i < CRYPTO_num_locks(); i++)
        cryptoLocks[i] = new std::mutex();
    
    CRYPTO_THREADID_set_callback(threadIdCallback);
    CRYPTO_set_locking_callback(lockingCallback);
}

void OpenSSLCrypto::threadsCleanup()
{
    CRYPTO_set_locking_callback(NULL);
    CRYPTO_THREADID_set_callback(NULL);
    for (int i = 0; i < CRYPTO_num_locks(); i++)
        delete cryptoLocks[i];
    
    cryptoLocks.resize(0);
}