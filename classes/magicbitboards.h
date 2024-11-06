#ifndef MAGIC_BITBOARDS_H
#define MAGIC_BITBOARDS_H

#include <stdint.h>

typedef uint64_t Bitboard;

// Generate rook attacks for a given square and blocking pieces
static inline uint64_t ratt(int sq, uint64_t block) {
    uint64_t result = 0ULL;
    int rk = sq / 8, fl = sq % 8, r, f;

    // North
    for (r = rk + 1; r <= 7; r++) {
        result |= (1ULL << (fl + r * 8));
        if (block & (1ULL << (fl + r * 8))) break;
    }
    // South
    for (r = rk - 1; r >= 0; r--) {
        result |= (1ULL << (fl + r * 8));
        if (block & (1ULL << (fl + r * 8))) break;
    }
    // East
    for (f = fl + 1; f <= 7; f++) {
        result |= (1ULL << (f + rk * 8));
        if (block & (1ULL << (f + rk * 8))) break;
    }
    // West
    for (f = fl - 1; f >= 0; f--) {
        result |= (1ULL << (f + rk * 8));
        if (block & (1ULL << (f + rk * 8))) break;
    }
    return result;
}

// Generate bishop attacks for a given square and blocking pieces
static inline uint64_t batt(int sq, uint64_t block) {
    uint64_t result = 0ULL;
    int rk = sq / 8, fl = sq % 8, r, f;

    // Northeast
    for (r = rk + 1, f = fl + 1; r <= 7 && f <= 7; r++, f++) {
        result |= (1ULL << (f + r * 8));
        if (block & (1ULL << (f + r * 8))) break;
    }
    // Southeast
    for (r = rk - 1, f = fl + 1; r >= 0 && f <= 7; r--, f++) {
        result |= (1ULL << (f + r * 8));
        if (block & (1ULL << (f + r * 8))) break;
    }
    // Southwest
    for (r = rk - 1, f = fl - 1; r >= 0 && f >= 0; r--, f--) {
        result |= (1ULL << (f + r * 8));
        if (block & (1ULL << (f + r * 8))) break;
    }
    // Northwest
    for (r = rk + 1, f = fl - 1; r <= 7 && f >= 0; r++, f--) {
        result |= (1ULL << (f + r * 8));
        if (block & (1ULL << (f + r * 8))) break;
    }
    return result;
}

// Compiler-specific bit manipulation functions
#ifdef __clang__
    // Clang/LLVM specific bit counting
    static inline int countOnes(uint64_t b) {
        return __builtin_popcountll(b);
    }

    // Find first set bit (returns 0-63, undefined for b==0)
    static inline int getFirstBit(uint64_t b) {
        return __builtin_ctzll(b);
    }
#else
    // Fallback bit counting implementation
    static inline int countOnes(uint64_t b) {
        int r = 0;
        while (b) {
            r++;
            b &= b - 1;
        }
        return r;
    }

    // Fallback first bit implementation
    static inline int getFirstBit(uint64_t b) {
        const int BitTable[64] = {
            63, 30, 3, 32, 25, 41, 22, 33, 15, 50, 42, 13, 11, 53, 19, 34,
            61, 29, 2, 51, 21, 43, 45, 10, 18, 47, 1, 54, 9, 57, 0, 35,
            62, 31, 40, 4, 49, 5, 52, 26, 60, 6, 23, 44, 46, 27, 56, 16,
            7, 39, 48, 24, 59, 14, 12, 55, 38, 28, 58, 20, 37, 17, 36, 8
        };
        uint64_t debruijn = 0x03f79d71b4cb0a89ULL;
        return BitTable[((b ^ (b-1)) * debruijn) >> 58];
    }
#endif

// Convert index to bitboard configuration
static inline uint64_t indexToUint64(int index, int bits, uint64_t m) {
    uint64_t result = 0ULL;
    for (int i = 0; i < bits; i++) {
        uint64_t least_bit = m & -m;  // get least significant bit
        if (index & (1 << i)) {
            result |= least_bit;
        }
        m &= (m - 1);  // clear least significant bit
    }
    return result;
}

// Bitboard manipulation macros
#define SET_BIT(bb, sq) ((bb) |= (1ULL << (sq)))
#define CLEAR_BIT(bb, sq) ((bb) &= ~(1ULL << (sq)))
#define GET_BIT(bb, sq) ((bb) & (1ULL << (sq)))
#define SQUARE(rank, file) ((rank) * 8 + (file))

// Directional shift macros
#define NORTH(bb) ((bb) << 8)
#define SOUTH(bb) ((bb) >> 8)
#define EAST(bb) (((bb) & ~0x8080808080808080ULL) << 1)
#define WEST(bb) (((bb) & ~0x0101010101010101ULL) >> 1)
#define NORTH_EAST(bb) (((bb) & ~0x8080808080808080ULL) << 9)
#define NORTH_WEST(bb) (((bb) & ~0x0101010101010101ULL) << 7)
#define SOUTH_EAST(bb) (((bb) & ~0x8080808080808080ULL) >> 7)
#define SOUTH_WEST(bb) (((bb) & ~0x0101010101010101ULL) >> 9)

// Pawn attack macros
#define WHITE_PAWN_ATTACKS(pawns) (NORTH_EAST(pawns) | NORTH_WEST(pawns))
#define BLACK_PAWN_ATTACKS(pawns) (SOUTH_EAST(pawns) | SOUTH_WEST(pawns))

// Size of attack tables for each square
const int RAttackSize[64] = {
  4096,
  2048,
  2048,
  2048,
  2048,
  2048,
  2048,
  4096,
  2048,
  1024,
  1024,
  1024,
  1024,
  1024,
  1024,
  2048,
  2048,
  1024,
  1024,
  1024,
  1024,
  1024,
  1024,
  2048,
  2048,
  1024,
  1024,
  1024,
  1024,
  1024,
  1024,
  2048,
  2048,
  1024,
  1024,
  1024,
  1024,
  1024,
  1024,
  2048,
  2048,
  1024,
  1024,
  1024,
  1024,
  1024,
  1024,
  2048,
  2048,
  1024,
  1024,
  1024,
  1024,
  1024,
  1024,
  2048,
  4096,
  2048,
  2048,
  2048,
  2048,
  2048,
  2048,
  4096,
};

const int BAttackSize[64] = {
  64,
  32,
  32,
  32,
  32,
  32,
  32,
  64,
  32,
  32,
  32,
  32,
  32,
  32,
  32,
  32,
  32,
  32,
  128,
  128,
  128,
  128,
  32,
  32,
  32,
  32,
  128,
  512,
  512,
  128,
  32,
  32,
  32,
  32,
  128,
  512,
  512,
  128,
  32,
  32,
  32,
  32,
  128,
  128,
  128,
  128,
  32,
  32,
  32,
  32,
  32,
  32,
  32,
  32,
  32,
  32,
  64,
  32,
  32,
  32,
  32,
  32,
  32,
  64,
};

// Attack lookup tables
static Bitboard* RAttacks[64];
static Bitboard* BAttacks[64];

// Magic bitboard shift amounts
const int RShifts[64] = {
  52,
  53,
  53,
  53,
  53,
  53,
  53,
  52,
  53,
  54,
  54,
  54,
  54,
  54,
  54,
  53,
  53,
  54,
  54,
  54,
  54,
  54,
  54,
  53,
  53,
  54,
  54,
  54,
  54,
  54,
  54,
  53,
  53,
  54,
  54,
  54,
  54,
  54,
  54,
  53,
  53,
  54,
  54,
  54,
  54,
  54,
  54,
  53,
  53,
  54,
  54,
  54,
  54,
  54,
  54,
  53,
  52,
  53,
  53,
  53,
  53,
  53,
  53,
  52,
};

const int BShifts[64] = {
  58,
  59,
  59,
  59,
  59,
  59,
  59,
  58,
  59,
  59,
  59,
  59,
  59,
  59,
  59,
  59,
  59,
  59,
  57,
  57,
  57,
  57,
  59,
  59,
  59,
  59,
  57,
  55,
  55,
  57,
  59,
  59,
  59,
  59,
  57,
  55,
  55,
  57,
  59,
  59,
  59,
  59,
  57,
  57,
  57,
  57,
  59,
  59,
  59,
  59,
  59,
  59,
  59,
  59,
  59,
  59,
  58,
  59,
  59,
  59,
  59,
  59,
  59,
  58,
};

// Magic numbers for rooks
const uint64_t RMagic[64] = {
  0xa8002c000108020ULL,
  0x6c00049b0002001ULL,
  0x100200010090040ULL,
  0x2480041000800801ULL,
  0x280028004000800ULL,
  0x900410008040022ULL,
  0x280020001001080ULL,
  0x2880002041000080ULL,
  0xa000800080400034ULL,
  0x4808020004000ULL,
  0x2290802004801000ULL,
  0x411000d00100020ULL,
  0x402800800040080ULL,
  0xb000401004208ULL,
  0x2409000100040200ULL,
  0x1002100004082ULL,
  0x22878001e24000ULL,
  0x1090810021004010ULL,
  0x801030040200012ULL,
  0x500808008001000ULL,
  0xa08018014000880ULL,
  0x8000808004000200ULL,
  0x201008080010200ULL,
  0x801020000441091ULL,
  0x800080204005ULL,
  0x1040200040100048ULL,
  0x120200402082ULL,
  0xd14880480100080ULL,
  0x12040280080080ULL,
  0x100040080020080ULL,
  0x9020010080800200ULL,
  0x813241200148449ULL,
  0x491604001800080ULL,
  0x100401000402001ULL,
  0x4820010021001040ULL,
  0x400402202000812ULL,
  0x209009005000802ULL,
  0x810800601800400ULL,
  0x4301083214000150ULL,
  0x204026458e001401ULL,
  0x40204000808000ULL,
  0x8001008040010020ULL,
  0x8410820820420010ULL,
  0x1003001000090020ULL,
  0x804040008008080ULL,
  0x12000810020004ULL,
  0x1000100200040208ULL,
  0x430000a044020001ULL,
  0x280009023410300ULL,
  0xe0100040002240ULL,
  0x200100401700ULL,
  0x2244100408008080ULL,
  0x8000400801980ULL,
  0x2000810040200ULL,
  0x8010100228810400ULL,
  0x2000009044210200ULL,
  0x4080008040102101ULL,
  0x40002080411d01ULL,
  0x2005524060000901ULL,
  0x502001008400422ULL,
  0x489a000810200402ULL,
  0x1004400080a13ULL,
  0x4000011008020084ULL,
  0x26002114058042ULL,
};

// Magic numbers for bishops
const uint64_t BMagic[64] = {
  0x89a1121896040240ULL,
  0x2004844802002010ULL,
  0x2068080051921000ULL,
  0x62880a0220200808ULL,
  0x4042004000000ULL,
  0x100822020200011ULL,
  0xc00444222012000aULL,
  0x28808801216001ULL,
  0x400492088408100ULL,
  0x201c401040c0084ULL,
  0x840800910a0010ULL,
  0x82080240060ULL,
  0x2000840504006000ULL,
  0x30010c4108405004ULL,
  0x1008005410080802ULL,
  0x8144042209100900ULL,
  0x208081020014400ULL,
  0x4800201208ca00ULL,
  0xf18140408012008ULL,
  0x1004002802102001ULL,
  0x841000820080811ULL,
  0x40200200a42008ULL,
  0x800054042000ULL,
  0x88010400410c9000ULL,
  0x520040470104290ULL,
  0x1004040051500081ULL,
  0x2002081833080021ULL,
  0x400c00c010142ULL,
  0x941408200c002000ULL,
  0x658810000806011ULL,
  0x188071040440a00ULL,
  0x4800404002011c00ULL,
  0x104442040404200ULL,
  0x511080202091021ULL,
  0x4022401120400ULL,
  0x80c0040400080120ULL,
  0x8040010040820802ULL,
  0x480810700020090ULL,
  0x102008e00040242ULL,
  0x809005202050100ULL,
  0x8002024220104080ULL,
  0x431008804142000ULL,
  0x19001802081400ULL,
  0x200014208040080ULL,
  0x3308082008200100ULL,
  0x41010500040c020ULL,
  0x4012020c04210308ULL,
  0x208220a202004080ULL,
  0x111040120082000ULL,
  0x6803040141280a00ULL,
  0x2101004202410000ULL,
  0x8200000041108022ULL,
  0x21082088000ULL,
  0x2410204010040ULL,
  0x40100400809000ULL,
  0x822088220820214ULL,
  0x40808090012004ULL,
  0x910224040218c9ULL,
  0x402814422015008ULL,
  0x90014004842410ULL,
  0x1000042304105ULL,
  0x10008830412a00ULL,
  0x2520081090008908ULL,
  0x40102000a0a60140ULL,
};

// Attack masks for each square
const uint64_t RMasks[64] = {
  0x101010101017eULL,
  0x202020202027cULL,
  0x404040404047aULL,
  0x8080808080876ULL,
  0x1010101010106eULL,
  0x2020202020205eULL,
  0x4040404040403eULL,
  0x8080808080807eULL,
  0x1010101017e00ULL,
  0x2020202027c00ULL,
  0x4040404047a00ULL,
  0x8080808087600ULL,
  0x10101010106e00ULL,
  0x20202020205e00ULL,
  0x40404040403e00ULL,
  0x80808080807e00ULL,
  0x10101017e0100ULL,
  0x20202027c0200ULL,
  0x40404047a0400ULL,
  0x8080808760800ULL,
  0x101010106e1000ULL,
  0x202020205e2000ULL,
  0x404040403e4000ULL,
  0x808080807e8000ULL,
  0x101017e010100ULL,
  0x202027c020200ULL,
  0x404047a040400ULL,
  0x8080876080800ULL,
  0x1010106e101000ULL,
  0x2020205e202000ULL,
  0x4040403e404000ULL,
  0x8080807e808000ULL,
  0x1017e01010100ULL,
  0x2027c02020200ULL,
  0x4047a04040400ULL,
  0x8087608080800ULL,
  0x10106e10101000ULL,
  0x20205e20202000ULL,
  0x40403e40404000ULL,
  0x80807e80808000ULL,
  0x17e0101010100ULL,
  0x27c0202020200ULL,
  0x47a0404040400ULL,
  0x8760808080800ULL,
  0x106e1010101000ULL,
  0x205e2020202000ULL,
  0x403e4040404000ULL,
  0x807e8080808000ULL,
  0x7e010101010100ULL,
  0x7c020202020200ULL,
  0x7a040404040400ULL,
  0x76080808080800ULL,
  0x6e101010101000ULL,
  0x5e202020202000ULL,
  0x3e404040404000ULL,
  0x7e808080808000ULL,
  0x7e01010101010100ULL,
  0x7c02020202020200ULL,
  0x7a04040404040400ULL,
  0x7608080808080800ULL,
  0x6e10101010101000ULL,
  0x5e20202020202000ULL,
  0x3e40404040404000ULL,
  0x7e80808080808000ULL,
};

const uint64_t BMasks[64] = {
  0x40201008040200ULL,
  0x402010080400ULL,
  0x4020100a00ULL,
  0x40221400ULL,
  0x2442800ULL,
  0x204085000ULL,
  0x20408102000ULL,
  0x2040810204000ULL,
  0x20100804020000ULL,
  0x40201008040000ULL,
  0x4020100a0000ULL,
  0x4022140000ULL,
  0x244280000ULL,
  0x20408500000ULL,
  0x2040810200000ULL,
  0x4081020400000ULL,
  0x10080402000200ULL,
  0x20100804000400ULL,
  0x4020100a000a00ULL,
  0x402214001400ULL,
  0x24428002800ULL,
  0x2040850005000ULL,
  0x4081020002000ULL,
  0x8102040004000ULL,
  0x8040200020400ULL,
  0x10080400040800ULL,
  0x20100a000a1000ULL,
  0x40221400142200ULL,
  0x2442800284400ULL,
  0x4085000500800ULL,
  0x8102000201000ULL,
  0x10204000402000ULL,
  0x4020002040800ULL,
  0x8040004081000ULL,
  0x100a000a102000ULL,
  0x22140014224000ULL,
  0x44280028440200ULL,
  0x8500050080400ULL,
  0x10200020100800ULL,
  0x20400040201000ULL,
  0x2000204081000ULL,
  0x4000408102000ULL,
  0xa000a10204000ULL,
  0x14001422400000ULL,
  0x28002844020000ULL,
  0x50005008040200ULL,
  0x20002010080400ULL,
  0x40004020100800ULL,
  0x20408102000ULL,
  0x40810204000ULL,
  0xa1020400000ULL,
  0x142240000000ULL,
  0x284402000000ULL,
  0x500804020000ULL,
  0x201008040200ULL,
  0x402010080400ULL,
  0x2040810204000ULL,
  0x4081020400000ULL,
  0xa102040000000ULL,
  0x14224000000000ULL,
  0x28440200000000ULL,
  0x50080402000000ULL,
  0x20100804020000ULL,
  0x40201008040200ULL,
};

// Pre-calculated knight attack bitboards
const uint64_t KnightAttacks[64] = {
  0x20400ULL,
  0x50800ULL,
  0xa1100ULL,
  0x142200ULL,
  0x284400ULL,
  0x508800ULL,
  0xa01000ULL,
  0x402000ULL,
  0x2040004ULL,
  0x5080008ULL,
  0xa110011ULL,
  0x14220022ULL,
  0x28440044ULL,
  0x50880088ULL,
  0xa0100010ULL,
  0x40200020ULL,
  0x204000402ULL,
  0x508000805ULL,
  0xa1100110aULL,
  0x1422002214ULL,
  0x2844004428ULL,
  0x5088008850ULL,
  0xa0100010a0ULL,
  0x4020002040ULL,
  0x20400040200ULL,
  0x50800080500ULL,
  0xa1100110a00ULL,
  0x142200221400ULL,
  0x284400442800ULL,
  0x508800885000ULL,
  0xa0100010a000ULL,
  0x402000204000ULL,
  0x2040004020000ULL,
  0x5080008050000ULL,
  0xa1100110a0000ULL,
  0x14220022140000ULL,
  0x28440044280000ULL,
  0x50880088500000ULL,
  0xa0100010a00000ULL,
  0x40200020400000ULL,
  0x204000402000000ULL,
  0x508000805000000ULL,
  0xa1100110a000000ULL,
  0x1422002214000000ULL,
  0x2844004428000000ULL,
  0x5088008850000000ULL,
  0xa0100010a0000000ULL,
  0x4020002040000000ULL,
  0x400040200000000ULL,
  0x800080500000000ULL,
  0x1100110a00000000ULL,
  0x2200221400000000ULL,
  0x4400442800000000ULL,
  0x8800885000000000ULL,
  0x100010a000000000ULL,
  0x2000204000000000ULL,
  0x4020000000000ULL,
  0x8050000000000ULL,
  0x110a0000000000ULL,
  0x22140000000000ULL,
  0x44280000000000ULL,
  0x88500000000000ULL,
  0x10a00000000000ULL,
  0x20400000000000ULL,
};

// Pre-calculated king attack bitboards
const uint64_t KingAttacks[64] = {
  0x302ULL,
  0x705ULL,
  0xe0aULL,
  0x1c14ULL,
  0x3828ULL,
  0x7050ULL,
  0xe0a0ULL,
  0xc040ULL,
  0x30203ULL,
  0x70507ULL,
  0xe0a0eULL,
  0x1c141cULL,
  0x382838ULL,
  0x705070ULL,
  0xe0a0e0ULL,
  0xc040c0ULL,
  0x3020300ULL,
  0x7050700ULL,
  0xe0a0e00ULL,
  0x1c141c00ULL,
  0x38283800ULL,
  0x70507000ULL,
  0xe0a0e000ULL,
  0xc040c000ULL,
  0x302030000ULL,
  0x705070000ULL,
  0xe0a0e0000ULL,
  0x1c141c0000ULL,
  0x3828380000ULL,
  0x7050700000ULL,
  0xe0a0e00000ULL,
  0xc040c00000ULL,
  0x30203000000ULL,
  0x70507000000ULL,
  0xe0a0e000000ULL,
  0x1c141c000000ULL,
  0x382838000000ULL,
  0x705070000000ULL,
  0xe0a0e0000000ULL,
  0xc040c0000000ULL,
  0x3020300000000ULL,
  0x7050700000000ULL,
  0xe0a0e00000000ULL,
  0x1c141c00000000ULL,
  0x38283800000000ULL,
  0x70507000000000ULL,
  0xe0a0e000000000ULL,
  0xc040c000000000ULL,
  0x302030000000000ULL,
  0x705070000000000ULL,
  0xe0a0e0000000000ULL,
  0x1c141c0000000000ULL,
  0x3828380000000000ULL,
  0x7050700000000000ULL,
  0xe0a0e00000000000ULL,
  0xc040c00000000000ULL,
  0x203000000000000ULL,
  0x507000000000000ULL,
  0xa0e000000000000ULL,
  0x141c000000000000ULL,
  0x2838000000000000ULL,
  0x5070000000000000ULL,
  0xa0e0000000000000ULL,
  0x40c0000000000000ULL,
};

// Helper functions for move generation
static inline uint64_t getRookAttacks(int square, uint64_t occupied) {
    occupied &= RMasks[square];
    occupied *= RMagic[square];
    occupied >>= RShifts[square];
    return RAttacks[square][occupied];
}

static inline uint64_t getBishopAttacks(int square, uint64_t occupied) {
    occupied &= BMasks[square];
    occupied *= BMagic[square];
    occupied >>= BShifts[square];
    return BAttacks[square][occupied];
}

static inline uint64_t getQueenAttacks(int square, uint64_t occupied) {
    return getRookAttacks(square, occupied) | getBishopAttacks(square, occupied);
}

// Initialize magic bitboards
void initMagicBitboards(void) {
    int square, i;
    uint64_t subset, index;

    // Initialize rook attack tables
    for (square = 0; square < 64; square++) {
        RAttacks[square] = new Bitboard[RAttackSize[square]];
        uint64_t mask = RMasks[square];
        int bits = countOnes(mask);
        int n = 1 << bits;

        for (i = 0; i < n; i++) {
            subset = indexToUint64(i, bits, mask);
            index = (subset * RMagic[square]) >> RShifts[square];
            RAttacks[square][index] = ratt(square, subset);
        }
    }

    // Initialize bishop attack tables
    for (square = 0; square < 64; square++) {
        BAttacks[square] = new Bitboard[BAttackSize[square]];
        uint64_t mask = BMasks[square];
        int bits = countOnes(mask);
        int n = 1 << bits;

        for (i = 0; i < n; i++) {
            subset = indexToUint64(i, bits, mask);
            index = (subset * BMagic[square]) >> BShifts[square];
            BAttacks[square][index] = batt(square, subset);
        }
    }
}

// Cleanup magic bitboard tables
void cleanupMagicBitboards(void) {
    int square;
    for (square = 0; square < 64; square++) {
        delete[] RAttacks[square];
        delete[] BAttacks[square];
    }
}

#endif // MAGIC_BITBOARDS_H
