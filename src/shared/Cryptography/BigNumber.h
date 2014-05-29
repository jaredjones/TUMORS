//
//  BigNumber.h
//  TUMORS
//
//  Created by Jared Jones on 12/26/13.
//  Copyright (c) 2013 Uvora. All rights reserved.
//

#ifndef __TUMORS__BigNumber__
#define __TUMORS__BigNumber__

#include "Common.h"
#include <memory>

struct bignum_st;

class BigNumber
{
public:
    BigNumber();
    BigNumber(BigNumber const& bn);
    BigNumber(uint32);
    ~BigNumber();
    
    void SetDword(uint32);
    void SetQword(uint64);
    void SetBinary(uint8 const* bytes, int32 len);
    void SetHexStr(char const* str);
    
    void SetRand(int32 numbits);
    
    BigNumber& operator=(BigNumber const& bn);
    
    BigNumber operator+=(BigNumber const& bn);
    BigNumber operator+(BigNumber const& bn)
    {
        BigNumber t(*this);
        return t += bn;
    }
    
    BigNumber operator-=(BigNumber const& bn);
    BigNumber operator-(BigNumber const& bn)
    {
        BigNumber t(*this);
        return t -= bn;
    }
    
    BigNumber operator*=(BigNumber const& bn);
    BigNumber operator*(BigNumber const& bn)
    {
        BigNumber t(*this);
        return t *= bn;
    }
    
    BigNumber operator/=(BigNumber const& bn);
    BigNumber operator/(BigNumber const& bn)
    {
        BigNumber t(*this);
        return t /= bn;
    }
    
    BigNumber operator%=(BigNumber const& bn);
    BigNumber operator%(BigNumber const& bn)
    {
        BigNumber t(*this);
        return t %= bn;
    }
    
    bool isZero() const;
    
    BigNumber ModExp(BigNumber const& bn1, BigNumber const& bn2);
    BigNumber Exp(BigNumber const&);
    
    int32 GetNumBytes(void);
    
    struct bignum_st *BN() { return _bn; }
    
    uint32 AsDword();
    
    std::unique_ptr<uint8> AsByteArray(int32 minSize = 0, bool littleEndian = true);
    
    char * AsHexStr() const;
    char * AsDecStr() const;
    
private:
    struct bignum_st *_bn;
};

#endif /* defined(__TUMORS__BigNumber__) */
