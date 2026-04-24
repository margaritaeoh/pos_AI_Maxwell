#ifndef PASSWORD_HASHER_H
#define PASSWORD_HASHER_H

// SHA-256 based password hashing utility for the POS system.
// The SHA-256 algorithm is defined in FIPS 180-4 (public domain algorithm).
//
// Hash formats supported:
//   H:<plaintext>            – legacy format (read-only, upgraded on next login)
//   H2:<sha256hex>           – current format: SHA-256("username:password")

#include <string>
#include <sstream>
#include <iomanip>
#include <cstdint>
#include <vector>

namespace pos {

class PasswordHasher {
public:
    // Produce a new-format hash: "H2:<sha256(username+":"+password)>"
    static std::string hashPassword(const std::string& username,
                                    const std::string& password) {
        return "H2:" + sha256(username + ":" + password);
    }

    // Produce the legacy hash (used only when creating accounts that still
    // use the old scheme, kept for test-compatibility).
    static std::string hashPasswordLegacy(const std::string& password) {
        return "H:" + password;
    }

    // Verify a password against a stored hash.
    // Supports both the legacy "H:" and the current "H2:" formats.
    static bool verifyPassword(const std::string& username,
                               const std::string& password,
                               const std::string& storedHash) {
        if (storedHash.size() >= 3 && storedHash.substr(0, 3) == "H2:") {
            return storedHash == hashPassword(username, password);
        }
        if (storedHash.size() >= 2 && storedHash.substr(0, 2) == "H:") {
            // Legacy: plaintext after the prefix
            return storedHash == hashPasswordLegacy(password);
        }
        return false;
    }

    // Returns true when the stored hash uses the legacy format and should
    // be upgraded to the SHA-256 format on the next successful login.
    static bool needsUpgrade(const std::string& storedHash) {
        if (storedHash.size() >= 3 && storedHash.substr(0, 3) == "H2:") {
            return false;
        }
        return storedHash.size() >= 2 && storedHash.substr(0, 2) == "H:";
    }

private:
    // -----------------------------------------------------------------------
    // Minimal SHA-256 implementation (FIPS 180-4 — public domain algorithm)
    // -----------------------------------------------------------------------
    static uint32_t rotr(uint32_t x, uint32_t n) {
        return (x >> n) | (x << (32u - n));
    }
    static uint32_t ch(uint32_t e, uint32_t f, uint32_t g) {
        return (e & f) ^ (~e & g);
    }
    static uint32_t maj(uint32_t a, uint32_t b, uint32_t c) {
        return (a & b) ^ (a & c) ^ (b & c);
    }
    static uint32_t sig0(uint32_t a) {
        return rotr(a, 2) ^ rotr(a, 13) ^ rotr(a, 22);
    }
    static uint32_t sig1(uint32_t e) {
        return rotr(e, 6) ^ rotr(e, 11) ^ rotr(e, 25);
    }
    static uint32_t gam0(uint32_t x) {
        return rotr(x, 7) ^ rotr(x, 18) ^ (x >> 3);
    }
    static uint32_t gam1(uint32_t x) {
        return rotr(x, 17) ^ rotr(x, 19) ^ (x >> 10);
    }

    static std::string sha256(const std::string& input) {
        // Round constants (first 32 bits of the fractional parts of the
        // cube roots of the first 64 primes — FIPS 180-4 §4.2.2)
        static const uint32_t K[64] = {
            0x428a2f98u, 0x71374491u, 0xb5c0fbcfu, 0xe9b5dba5u,
            0x3956c25bu, 0x59f111f1u, 0x923f82a4u, 0xab1c5ed5u,
            0xd807aa98u, 0x12835b01u, 0x243185beu, 0x550c7dc3u,
            0x72be5d74u, 0x80deb1feu, 0x9bdc06a7u, 0xc19bf174u,
            0xe49b69c1u, 0xefbe4786u, 0x0fc19dc6u, 0x240ca1ccu,
            0x2de92c6fu, 0x4a7484aau, 0x5cb0a9dcu, 0x76f988dau,
            0x983e5152u, 0xa831c66du, 0xb00327c8u, 0xbf597fc7u,
            0xc6e00bf3u, 0xd5a79147u, 0x06ca6351u, 0x14292967u,
            0x27b70a85u, 0x2e1b2138u, 0x4d2c6dfcu, 0x53380d13u,
            0x650a7354u, 0x766a0abbu, 0x81c2c92eu, 0x92722c85u,
            0xa2bfe8a1u, 0xa81a664bu, 0xc24b8b70u, 0xc76c51a3u,
            0xd192e819u, 0xd6990624u, 0xf40e3585u, 0x106aa070u,
            0x19a4c116u, 0x1e376c08u, 0x2748774cu, 0x34b0bcb5u,
            0x391c0cb3u, 0x4ed8aa4au, 0x5b9cca4fu, 0x682e6ff3u,
            0x748f82eeu, 0x78a5636fu, 0x84c87814u, 0x8cc70208u,
            0x90befffau, 0xa4506cebu, 0xbef9a3f7u, 0xc67178f2u
        };

        // Initial hash values (first 32 bits of the fractional parts of the
        // square roots of the first 8 primes — FIPS 180-4 §5.3.3)
        uint32_t h[8] = {
            0x6a09e667u, 0xbb67ae85u, 0x3c6ef372u, 0xa54ff53au,
            0x510e527fu, 0x9b05688cu, 0x1f83d9abu, 0x5be0cd19u
        };

        // Pre-processing: padding
        std::vector<uint8_t> msg(input.begin(), input.end());
        uint64_t bitLen = static_cast<uint64_t>(msg.size()) * 8u;
        msg.push_back(0x80u);
        while (msg.size() % 64u != 56u)
            msg.push_back(0x00u);
        for (int i = 7; i >= 0; --i)
            msg.push_back(static_cast<uint8_t>((bitLen >> (8u * static_cast<unsigned>(i))) & 0xffu));

        // Process each 512-bit (64-byte) chunk
        for (size_t i = 0; i < msg.size(); i += 64u) {
            uint32_t w[64];
            for (int j = 0; j < 16; ++j) {
                w[j] = (static_cast<uint32_t>(msg[i + static_cast<size_t>(j) * 4u])     << 24u) |
                       (static_cast<uint32_t>(msg[i + static_cast<size_t>(j) * 4u + 1u]) << 16u) |
                       (static_cast<uint32_t>(msg[i + static_cast<size_t>(j) * 4u + 2u]) <<  8u) |
                        static_cast<uint32_t>(msg[i + static_cast<size_t>(j) * 4u + 3u]);
            }
            for (int j = 16; j < 64; ++j)
                w[j] = gam1(w[j - 2]) + w[j - 7] + gam0(w[j - 15]) + w[j - 16];

            uint32_t a = h[0], b = h[1], c = h[2], d = h[3];
            uint32_t e = h[4], f = h[5], g = h[6], hh = h[7];

            for (int j = 0; j < 64; ++j) {
                uint32_t t1 = hh + sig1(e) + ch(e, f, g) + K[j] + w[j];
                uint32_t t2 = sig0(a) + maj(a, b, c);
                hh = g; g = f; f = e; e = d + t1;
                d  = c; c = b; b = a; a = t1 + t2;
            }
            h[0] += a; h[1] += b; h[2] += c; h[3] += d;
            h[4] += e; h[5] += f; h[6] += g; h[7] += hh;
        }

        std::ostringstream oss;
        for (int i = 0; i < 8; ++i)
            oss << std::hex << std::setw(8) << std::setfill('0') << h[i];
        return oss.str();
    }
};

} // namespace pos

#endif // PASSWORD_HASHER_H
