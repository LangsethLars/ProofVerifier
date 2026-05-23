#pragma once

#include "LexerBase.h"

class Proof_Lexer : public ::LexerBase {

public:

    Proof_Lexer() : ::LexerBase(g_TokenClasses, g_LexerTable) {}

    static const TokenClasses g_TokenClasses;
    static const LexerTable g_LexerTable;

    enum class TokenId {
        END_OF_FILE = 0,
        COMMENT = 1,
        SPACE = 2,
        NL = 3,
        IDENT = 4,
        NUMBER = 5,
        _Int = 6,
        _Int_Add = 7,
        _Int_Sub = 8,
        _Int_Mul = 9,
        _Int_Neg = 10,
        _Prop = 11,
        _Prop_True = 12,
        _Prop_False = 13,
        _Prop_Not = 14,
        _Prop_And = 15,
        _Prop_Or = 16,
        _Prop_Implies = 17,
        _Prop_Iff = 18,
        _Prop_AndLeft = 19,
        _Prop_AndRight = 20,
        _Prop_AndIntro = 21,
        _Prop_OrLeft = 22,
        _Prop_OrRight = 23,
        _Prop_FalseElim = 24,
        _Prop_Int_Eq = 25,
        _Theorem = 26,
        _Proof = 27,
        _End = 28,
        _Assume = 29,
        _Have = 30,
        _Exact = 31,
        _Show = 32,
        _Cases = 33,
        _Case = 34,
        _Refl = 35,
        _Trivial = 36,
        _By = 37,
        _Rewrite = 38,
        _Symm = 39,
        _Trans = 40,
        _Reduce = 41,
        _Def = 42,
        _ForAll = 43,
        _Exists = 44,
        _Induction = 45,
        _Fun = 46,
        _Pi = 47,
        _Type = 48,
        _Nat = 49,
        EQUAL = 50,
        OPEN_PAR = 51,
        CLOSE_PAR = 52,
        COLON = 53,
        COLONEQUAL = 54,
        COMMA = 55,
        DOT = 56,
        SLIM_ARROW = 57,
        FAT_ARROW = 58,
        SEMICOLON = 59
    }; // End of enum class TokenId

}; // End of class Proof_Lexer
