// Copyright (c) 2009-2010 Satoshi Nakamoto
// Copyright (c) 2009-2019 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <primitives/block.h>

#include <hash.h>
#include <tinyformat.h>

#include <streams.h>
#include <version.h>
#include <stdlib.h> // exit()
#include <sync.h>
#include <crypto/sha512.h>
#include <crypto/argon2d/argon2.h>
#include <crypto/argon2d/blake2/blake2.h>
uint256 CBlockHeader::GetHash() const
{
    return (CHashWriter{PROTOCOL_VERSION} << *this).GetHash();
}

uint256 CBlockHeader::GetArgon2idPoWHash() const
{
    uint256 hash;
    uint256 hash2;
    CDataStream ss(SER_NETWORK, PROTOCOL_VERSION);
    ss << *this;
    
    // Hashing the data using SHA-512 (two rounds)
    std::vector<unsigned char> salt_sha512(CSHA512::OUTPUT_SIZE);
    CSHA512().Write((unsigned char*)&ss[0], ss.size()).Finalize(salt_sha512.data());
    CSHA512().Write(salt_sha512.data(), salt_sha512.size()).Finalize(salt_sha512.data());
    
    // Preparing data for hashing
    const void* pwd = &ss[0];
    size_t pwdlen = ss.size();
    const void* salt = salt_sha512.data();
    size_t saltlen = salt_sha512.size();
    
    // Calling the argon2id_hash_raw function for the first round
    int rc = argon2id_hash_raw(2, 8000, 16, pwd, pwdlen, salt, saltlen, &hash, 32);
    if (rc != ARGON2_OK) {
        printf("Error: Failed to compute Argon2id hash\n");
        exit(1);
    }
    
    // Hashing the result of the first round using SHA-512 (two rounds)
    std::vector<unsigned char> salt_sha512_round2(CSHA512::OUTPUT_SIZE);
    CSHA512().Write((unsigned char*)&hash, 32).Finalize(salt_sha512_round2.data());
    CSHA512().Write(salt_sha512_round2.data(), salt_sha512_round2.size()).Finalize(salt_sha512_round2.data());
    
    // Calling the argon2id_hash_raw function for the second round
    rc = argon2id_hash_raw(2, 16000, 16, &hash, 32, salt_sha512_round2.data(), salt_sha512_round2.size(), &hash2, 32);
    if (rc != ARGON2_OK) {
        printf("Error: Failed to compute Argon2id hash for the second round\n");
        exit(1);
    }
    
    return hash2;
}

uint256 CBlockHeader::GetArgon2idPoWHash2() const
{
    uint256 hash;
    uint256 hash2;
    CDataStream ss(SER_NETWORK, PROTOCOL_VERSION);
    ss << *this;
    
    // Hashing the data using SHA-512 (two rounds)
    std::vector<unsigned char> salt_sha512(CSHA512::OUTPUT_SIZE);
    CSHA512().Write((unsigned char*)&ss[0], ss.size()).Finalize(salt_sha512.data());
    CSHA512().Write(salt_sha512.data(), salt_sha512.size()).Finalize(salt_sha512.data());
    
    // Preparing data for hashing
    const void* pwd = &ss[0];
    size_t pwdlen = ss.size();
    const void* salt = salt_sha512.data();
    size_t saltlen = salt_sha512.size();
    
    // Calling the argon2id_hash_raw function for the first round
    int rc = argon2id_hash_raw(2, 8000, 16, pwd, pwdlen, salt, saltlen, &hash, 32);
    if (rc != ARGON2_OK) {
        printf("Error: Failed to compute Argon2id hash\n");
        exit(1);
    }
    
    // Hashing the result of the first round using SHA-512 (two rounds)
    std::vector<unsigned char> salt_sha512_round2(CSHA512::OUTPUT_SIZE);
    CSHA512().Write((unsigned char*)&hash, 32).Finalize(salt_sha512_round2.data());
    CSHA512().Write(salt_sha512_round2.data(), salt_sha512_round2.size()).Finalize(salt_sha512_round2.data());
    
    // Calling the argon2id_hash_raw function for the second round
    rc = argon2id_hash_raw(2, 32000, 16, &hash, 32, salt_sha512_round2.data(), salt_sha512_round2.size(), &hash2, 32);
    if (rc != ARGON2_OK) {
        printf("Error: Failed to compute Argon2id hash for the second round\n");
        exit(1);
    }
    
    return hash2;
}

std::string CBlock::ToString() const
{
    std::stringstream s;
    s << strprintf("CBlock(hash=%s, ver=0x%08x, hashPrevBlock=%s, hashMerkleRoot=%s, nTime=%u, nBits=%08x, nNonce=%u, vtx=%u)\n",
        GetHash().ToString(),
        nVersion,
        hashPrevBlock.ToString(),
        hashMerkleRoot.ToString(),
        nTime, nBits, nNonce,
        vtx.size());
    for (const auto& tx : vtx) {
        s << "  " << tx->ToString() << "\n";
    }
    return s.str();
}
