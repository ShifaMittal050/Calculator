﻿// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

//----------------------------------------------------------------------------
//  Package Title  ratpak
//  File           support.c
//  Copyright      (C) 1995-96 Microsoft
//  Date           10-21-96
//
//
//  Description
//
//     Contains support functions for rationals and numbers.
//
//  Special Information
//
//
//
//----------------------------------------------------------------------------

#include "pch.h"
#include "ratpak.h"

using namespace std;

void _readconstants( void );

#if defined( GEN_CONST )
static int cbitsofprecision = 0;
#define READRAWRAT(v)
#define READRAWNUM(v)
#define DUMPRAWRAT(v) _dumprawrat(#v,v, wcout)
#define DUMPRAWNUM(v) fprintf( stderr, \
"// Autogenerated by _dumprawrat in support.cpp\n" ); \
    fprintf( stderr, "inline const NUMBER init_" #v "= {\n" ); \
    _dumprawnum(v, wcout); \
    fprintf( stderr, "};\n" )


#else

#define DUMPRAWRAT(v)
#define DUMPRAWNUM(v)
#define READRAWRAT(v) createrat(v); DUPNUM((v)->pp,(&(init_p_##v))); \
DUPNUM((v)->pq,(&(init_q_##v)));
#define READRAWNUM(v) DUPNUM(v,(&(init_##v)))

#define INIT_AND_DUMP_RAW_NUM_IF_NULL(r, v) if (r == nullptr) { r = i32tonum(v, BASEX); DUMPRAWNUM(v); }
#define INIT_AND_DUMP_RAW_RAT_IF_NULL(r, v) if (r == nullptr) { r = i32torat(v); DUMPRAWRAT(v); }

static constexpr int RATIO_FOR_DECIMAL = 9;
static constexpr int DECIMAL = 10;
static constexpr int CALC_DECIMAL_DIGITS_DEFAULT = 32;

static int cbitsofprecision = RATIO_FOR_DECIMAL * DECIMAL * CALC_DECIMAL_DIGITS_DEFAULT;

#include "ratconst.h"

#endif

bool g_ftrueinfinite = false;  // Set to true if you don't want
                                        // chopping internally
                                        // precision used internally

PNUMBER num_one= nullptr;
PNUMBER num_two= nullptr;
PNUMBER num_five= nullptr;
PNUMBER num_six= nullptr;
PNUMBER num_ten= nullptr;

PRAT ln_ten= nullptr;
PRAT ln_two= nullptr;
PRAT rat_zero= nullptr;
PRAT rat_one= nullptr;
PRAT rat_neg_one= nullptr;
PRAT rat_two= nullptr;
PRAT rat_six= nullptr;
PRAT rat_half= nullptr;
PRAT rat_ten= nullptr;
PRAT pt_eight_five= nullptr;
PRAT pi= nullptr;
PRAT pi_over_two= nullptr;
PRAT two_pi= nullptr;
PRAT one_pt_five_pi= nullptr;
PRAT e_to_one_half= nullptr;
PRAT rat_exp= nullptr;
PRAT rad_to_deg= nullptr;
PRAT rad_to_grad= nullptr;
PRAT rat_qword= nullptr;
PRAT rat_dword= nullptr; // unsigned max ui32
PRAT rat_word= nullptr;
PRAT rat_byte= nullptr;
PRAT rat_360= nullptr;
PRAT rat_400= nullptr;
PRAT rat_180= nullptr;
PRAT rat_200= nullptr;
PRAT rat_nRadix= nullptr;
PRAT rat_smallest= nullptr;
PRAT rat_negsmallest= nullptr;
PRAT rat_max_exp= nullptr;
PRAT rat_min_exp= nullptr;
PRAT rat_max_fact = nullptr;
PRAT rat_min_fact = nullptr;
PRAT rat_min_i32= nullptr; // min signed i32
PRAT rat_max_i32= nullptr; // max signed i32

//----------------------------------------------------------------------------
//
//  FUNCTION: ChangeConstants
//
//  ARGUMENTS:  base changing to, and precision to use.
//
//  RETURN: None
//
//  SIDE EFFECTS: sets a mess of constants.
//
//
//----------------------------------------------------------------------------

void ChangeConstants(uint32_t radix, int32_t precision)
{
    // ratio is set to the number of digits in the current radix, you can get
    // in the internal BASEX radix, this is important for length calculations
    // in translating from radix to BASEX and back.

    uint64_t limit = static_cast<uint64_t>(BASEX) / static_cast<uint64_t>(radix);
    g_ratio = 0;
    for (uint32_t digit = 1; digit < limit; digit *= radix )
    {
        g_ratio++;
    }
    g_ratio += !g_ratio;

    destroyrat(rat_nRadix);
    rat_nRadix=i32torat( radix );

    // Check to see what we have to recalculate and what we don't
    if (cbitsofprecision < (g_ratio * static_cast<int32_t>(radix) * precision))
    {
        g_ftrueinfinite = false;

        INIT_AND_DUMP_RAW_NUM_IF_NULL(num_one, 1L);
        INIT_AND_DUMP_RAW_NUM_IF_NULL(num_two, 2L);
        INIT_AND_DUMP_RAW_NUM_IF_NULL(num_five, 5L);
        INIT_AND_DUMP_RAW_NUM_IF_NULL(num_six, 6L);
        INIT_AND_DUMP_RAW_NUM_IF_NULL(num_ten, 10L);
        INIT_AND_DUMP_RAW_RAT_IF_NULL(rat_six, 6L);
        INIT_AND_DUMP_RAW_RAT_IF_NULL(rat_two, 2L);
        INIT_AND_DUMP_RAW_RAT_IF_NULL(rat_zero, 0L);
        INIT_AND_DUMP_RAW_RAT_IF_NULL(rat_one, 1L);
        INIT_AND_DUMP_RAW_RAT_IF_NULL(rat_neg_one, -1L);
        INIT_AND_DUMP_RAW_RAT_IF_NULL(rat_ten, 10L);
        INIT_AND_DUMP_RAW_RAT_IF_NULL(rat_word, 0xffff);
        INIT_AND_DUMP_RAW_RAT_IF_NULL(rat_word, 0xff);
        INIT_AND_DUMP_RAW_RAT_IF_NULL(rat_400, 400);
        INIT_AND_DUMP_RAW_RAT_IF_NULL(rat_360, 360);
        INIT_AND_DUMP_RAW_RAT_IF_NULL(rat_200, 200);
        INIT_AND_DUMP_RAW_RAT_IF_NULL(rat_180, 180);
        INIT_AND_DUMP_RAW_RAT_IF_NULL(rat_max_exp, 100000);

        // 3248, is the max number for which calc is able to compute factorial, after that it is unable to compute due to overflow.
        // Hence restricted factorial range as at most 3248.Beyond that calc will throw overflow error immediately.
        INIT_AND_DUMP_RAW_RAT_IF_NULL(rat_max_fact, 3249);

        // -1000, is the min number for which calc is able to compute factorial, after that it takes too long to compute.
        INIT_AND_DUMP_RAW_RAT_IF_NULL(rat_min_fact, -1000);

        DUPRAT(rat_smallest, rat_nRadix);
        ratpowi32(&rat_smallest, -precision, precision);
        DUPRAT(rat_negsmallest, rat_smallest);
        rat_negsmallest->pp->sign = -1;
        DUMPRAWRAT(rat_smallest);
        DUMPRAWRAT(rat_negsmallest);

        if (rat_half == nullptr)
        {
            createrat(rat_half);
            DUPNUM(rat_half->pp, num_one);
            DUPNUM(rat_half->pq, num_two);
            DUMPRAWRAT(rat_half);
        }

        if (pt_eight_five == nullptr)
        {
            createrat(pt_eight_five);
            pt_eight_five->pp = i32tonum(85L, BASEX);
            pt_eight_five->pq = i32tonum(100L, BASEX);
            DUMPRAWRAT(pt_eight_five);
        }

        DUPRAT(rat_qword, rat_two);
        numpowi32(&(rat_qword->pp), 64, BASEX, precision);
        subrat(&rat_qword, rat_one, precision);
        DUMPRAWRAT(rat_qword);

        DUPRAT(rat_dword, rat_two);
        numpowi32(&(rat_dword->pp), 32, BASEX, precision);
        subrat(&rat_dword, rat_one, precision);
        DUMPRAWRAT(rat_dword);

        DUPRAT(rat_max_i32, rat_two);
        numpowi32(&(rat_max_i32->pp), 31, BASEX, precision);
        DUPRAT(rat_min_i32, rat_max_i32);
        subrat(&rat_max_i32, rat_one, precision); // rat_max_i32 = 2^31 -1
        DUMPRAWRAT(rat_max_i32);

        rat_min_i32->pp->sign *= -1; // rat_min_i32 = -2^31
        DUMPRAWRAT(rat_min_i32);

        DUPRAT(rat_min_exp, rat_max_exp);
        rat_min_exp->pp->sign *= -1;
        DUMPRAWRAT(rat_min_exp);

        cbitsofprecision = g_ratio * radix * precision;

        // Apparently when dividing 180 by pi, another (internal) digit of
        // precision is needed.
        int32_t extraPrecision = precision + g_ratio;
        DUPRAT(pi, rat_half);
        asinrat(&pi, radix, extraPrecision);
        mulrat(&pi, rat_six, extraPrecision);
        DUMPRAWRAT(pi);

        DUPRAT(two_pi, pi);
        DUPRAT(pi_over_two, pi);
        DUPRAT(one_pt_five_pi, pi);
        addrat(&two_pi, pi, extraPrecision);
        DUMPRAWRAT(two_pi);

        divrat(&pi_over_two, rat_two, extraPrecision);
        DUMPRAWRAT(pi_over_two);

        addrat(&one_pt_five_pi, pi_over_two, extraPrecision);
        DUMPRAWRAT(one_pt_five_pi);

        DUPRAT(e_to_one_half, rat_half);
        _exprat(&e_to_one_half, extraPrecision);
        DUMPRAWRAT(e_to_one_half);

        DUPRAT(rat_exp, rat_one);
        _exprat(&rat_exp, extraPrecision);
        DUMPRAWRAT(rat_exp);

        // WARNING: remember lograt uses exponent constants calculated above...

        DUPRAT(ln_ten, rat_ten);
        lograt(&ln_ten, extraPrecision);
        DUMPRAWRAT(ln_ten);

        DUPRAT(ln_two, rat_two);
        lograt(&ln_two, extraPrecision);
        DUMPRAWRAT(ln_two);


        destroyrat(rad_to_deg);
        rad_to_deg = i32torat(180L);
        divrat(&rad_to_deg, pi, extraPrecision);
        DUMPRAWRAT(rad_to_deg);

        destroyrat(rad_to_grad);
        rad_to_grad = i32torat(200L);
        divrat(&rad_to_grad, pi, extraPrecision);
        DUMPRAWRAT(rad_to_grad);
    }
    else
    {
        _readconstants();

        DUPRAT(rat_smallest, rat_nRadix);
        ratpowi32(&rat_smallest, -precision, precision);
        DUPRAT(rat_negsmallest, rat_smallest);
        rat_negsmallest->pp->sign = -1;
    }

}

//----------------------------------------------------------------------------
//
//  FUNCTION: intrat
//
//  ARGUMENTS:  pointer to x PRAT representation of number
//
//  RETURN: no return value x PRAT is smashed with integral number
//
//
//----------------------------------------------------------------------------

void intrat( PRAT *px, uint32_t radix, int32_t precision)
{
    // Only do the intrat operation if number is nonzero.
    // and only if the bottom part is not one.
    if ( !zernum( (*px)->pp ) && !equnum( (*px)->pq, num_one ) )
    {
        flatrat(*px, radix, precision);

        // Subtract the fractional part of the rational
        PRAT pret = nullptr;
        DUPRAT(pret,*px);
        modrat( &pret, rat_one );

        subrat( px, pret, precision);
        destroyrat( pret );

        // Simplify the value if possible to resolve rounding errors
        flatrat(*px, radix, precision);
    }
}

//---------------------------------------------------------------------------
//
//  FUNCTION: rat_equ
//
//  ARGUMENTS:  PRAT a and PRAT b
//
//  RETURN: true if equal false otherwise.
//
//
//---------------------------------------------------------------------------

bool rat_equ( PRAT a, PRAT b, int32_t precision)

{
    PRAT rattmp= nullptr;
    DUPRAT(rattmp,a);
    rattmp->pp->sign *= -1;
    addrat( &rattmp, b, precision);
    bool bret = zernum( rattmp->pp );
    destroyrat( rattmp );
    return( bret );
}

//---------------------------------------------------------------------------
//
//  FUNCTION: rat_ge
//
//  ARGUMENTS:  PRAT a, PRAT b and int32_t precision
//
//  RETURN: true if a is greater than or equal to b
//
//
//---------------------------------------------------------------------------

bool rat_ge( PRAT a, PRAT b, int32_t precision)

{
    PRAT rattmp= nullptr;
    DUPRAT(rattmp,a);
    b->pp->sign *= -1;
    addrat( &rattmp, b, precision);
    b->pp->sign *= -1;
    bool bret = ( zernum( rattmp->pp ) ||
        rattmp->pp->sign * rattmp->pq->sign == 1 );
    destroyrat( rattmp );
    return( bret );
}


//---------------------------------------------------------------------------
//
//  FUNCTION: rat_gt
//
//  ARGUMENTS:  PRAT a and PRAT b
//
//  RETURN: true if a is greater than b
//
//
//---------------------------------------------------------------------------

bool rat_gt( PRAT a, PRAT b, int32_t precision)

{
    PRAT rattmp= nullptr;
    DUPRAT(rattmp,a);
    b->pp->sign *= -1;
    addrat( &rattmp, b, precision);
    b->pp->sign *= -1;
    bool bret = ( !zernum( rattmp->pp ) &&
        rattmp->pp->sign * rattmp->pq->sign == 1 );
    destroyrat( rattmp );
    return( bret );
}

//---------------------------------------------------------------------------
//
//  FUNCTION: rat_le
//
//  ARGUMENTS:  PRAT a, PRAT b and int32_t precision
//
//  RETURN: true if a is less than or equal to b
//
//
//---------------------------------------------------------------------------

bool rat_le( PRAT a, PRAT b, int32_t precision)

{

    PRAT rattmp= nullptr;
    DUPRAT(rattmp,a);
    b->pp->sign *= -1;
    addrat( &rattmp, b, precision);
    b->pp->sign *= -1;
    bool bret = ( zernum( rattmp->pp ) ||
        rattmp->pp->sign * rattmp->pq->sign == -1 );
    destroyrat( rattmp );
    return( bret );
}


//---------------------------------------------------------------------------
//
//  FUNCTION: rat_lt
//
//  ARGUMENTS:  PRAT a, PRAT b and int32_t precision
//
//  RETURN: true if a is less than b
//
//
//---------------------------------------------------------------------------

bool rat_lt( PRAT a, PRAT b, int32_t precision)

{
    PRAT rattmp= nullptr;
    DUPRAT(rattmp,a);
    b->pp->sign *= -1;
    addrat( &rattmp, b, precision);
    b->pp->sign *= -1;
    bool bret = ( !zernum( rattmp->pp ) &&
        rattmp->pp->sign * rattmp->pq->sign == -1 );
    destroyrat( rattmp );
    return( bret );
}


//---------------------------------------------------------------------------
//
//  FUNCTION: rat_neq
//
//  ARGUMENTS:  PRAT a and PRAT b
//
//  RETURN: true if a is not equal to b
//
//
//---------------------------------------------------------------------------


bool rat_neq( PRAT a, PRAT b, int32_t precision)

{
    PRAT rattmp= nullptr;
    DUPRAT(rattmp,a);
    rattmp->pp->sign *= -1;
    addrat( &rattmp, b, precision);
    bool bret = !( zernum( rattmp->pp ) );
    destroyrat( rattmp );
    return( bret );
}

//---------------------------------------------------------------------------
//
//  function: scale
//
//  ARGUMENTS:  pointer to x PRAT representation of number, and scaling factor
//
//  RETURN: no return, value x PRAT is smashed with a scaled number in the
//          range of the scalefact.
//
//---------------------------------------------------------------------------

void scale( PRAT *px, PRAT scalefact, uint32_t radix, int32_t precision )
{
    PRAT pret = nullptr;
    DUPRAT(pret,*px);

    // Logscale is a quick way to tell how much extra precision is needed for
    // scaling by scalefact.
    int32_t logscale = g_ratio * ( (pret->pp->cdigit+pret->pp->exp) -
            (pret->pq->cdigit+pret->pq->exp) );
    if ( logscale > 0 )
    {
        precision += logscale;
    }

    divrat( &pret, scalefact, precision);
    intrat(&pret, radix, precision);
    mulrat( &pret, scalefact, precision);
    pret->pp->sign *= -1;
    addrat( px, pret, precision);

    destroyrat( pret );
}

//---------------------------------------------------------------------------
//
//  function: scale2pi
//
//  ARGUMENTS:  pointer to x PRAT representation of number
//
//  RETURN: no return, value x PRAT is smashed with a scaled number in the
//          range of 0..2pi
//
//---------------------------------------------------------------------------

void scale2pi( PRAT *px, uint32_t radix, int32_t precision )
{
    PRAT pret = nullptr;
    PRAT my_two_pi = nullptr;
    DUPRAT(pret,*px);

    // Logscale is a quick way to tell how much extra precision is needed for
    // scaling by 2 pi.
    int32_t logscale = g_ratio * ( (pret->pp->cdigit+pret->pp->exp) -
            (pret->pq->cdigit+pret->pq->exp) );
    if ( logscale > 0 )
    {
        precision += logscale;
        DUPRAT(my_two_pi,rat_half);
        asinrat( &my_two_pi, radix, precision);
        mulrat( &my_two_pi, rat_six, precision);
        mulrat( &my_two_pi, rat_two, precision);
    }
    else
    {
        DUPRAT(my_two_pi,two_pi);
        logscale = 0;
    }

    divrat( &pret, my_two_pi, precision);
    intrat(&pret, radix, precision);
    mulrat( &pret, my_two_pi, precision);
    pret->pp->sign *= -1;
    addrat( px, pret, precision);

    destroyrat( my_two_pi );
    destroyrat( pret );
}

//---------------------------------------------------------------------------
//
//  FUNCTION: inbetween
//
//  ARGUMENTS:  PRAT *px, and PRAT range.
//
//  RETURN: none, changes *px to -/+range, if px is outside -range..+range
//
//---------------------------------------------------------------------------

void inbetween( PRAT *px, PRAT range, int32_t precision)

{
    if ( rat_gt(*px,range, precision) )
        {
        DUPRAT(*px,range);
        }
    else
        {
        range->pp->sign *= -1;
        if ( rat_lt(*px, range, precision) )
            {
            DUPRAT(*px,range);
            }
        range->pp->sign *= -1;
        }
}

//---------------------------------------------------------------------------
//
//  FUNCTION: _dumprawrat
//
//  ARGUMENTS:  const wchar *name of variable, PRAT x, output stream out
//
//  RETURN: none, prints the results of a dump of the internal structures
//          of a PRAT, suitable for READRAWRAT to stderr.
//
//---------------------------------------------------------------------------

void _dumprawrat( const wchar_t *varname, PRAT rat, wostream& out)

{
    _dumprawnum(varname, rat->pp, out );
    _dumprawnum(varname, rat->pq, out );
}

//---------------------------------------------------------------------------
//
//  FUNCTION: _dumprawnum
//
//  ARGUMENTS:  const wchar *name of variable, PNUMBER num, output stream out
//
//  RETURN: none, prints the results of a dump of the internal structures
//          of a PNUMBER, suitable for READRAWNUM to stderr.
//
//---------------------------------------------------------------------------

void _dumprawnum(const wchar_t *varname, PNUMBER num, wostream& out)

{
    int i;

    out << L"NUMBER " << varname << L" = {\n";
    out << L"\t"<< num->sign << L",\n";
    out << L"\t" << num->cdigit << L",\n";
    out << L"\t" << num->exp << L",\n";
    out << L"\t{ ";

    for ( i = 0; i < num->cdigit; i++ )
        {
        out << L" "<< num->mant[i] << L",";
        }
    out << L"}\n";
    out << L"};\n";
}

void _readconstants( void )

{
    READRAWNUM(num_one);
    READRAWNUM(num_two);
    READRAWNUM(num_five);
    READRAWNUM(num_six);
    READRAWNUM(num_ten);
    READRAWRAT(pt_eight_five);
    READRAWRAT(rat_six);
    READRAWRAT(rat_two);
    READRAWRAT(rat_zero);
    READRAWRAT(rat_one);
    READRAWRAT(rat_neg_one);
    READRAWRAT(rat_half);
    READRAWRAT(rat_ten);
    READRAWRAT(pi);
    READRAWRAT(two_pi);
    READRAWRAT(pi_over_two);
    READRAWRAT(one_pt_five_pi);
    READRAWRAT(e_to_one_half);
    READRAWRAT(rat_exp);
    READRAWRAT(ln_ten);
    READRAWRAT(ln_two);
    READRAWRAT(rad_to_deg);
    READRAWRAT(rad_to_grad);
    READRAWRAT(rat_qword);
    READRAWRAT(rat_dword);
    READRAWRAT(rat_word);
    READRAWRAT(rat_byte);
    READRAWRAT(rat_360);
    READRAWRAT(rat_400);
    READRAWRAT(rat_180);
    READRAWRAT(rat_200);
    READRAWRAT(rat_smallest);
    READRAWRAT(rat_negsmallest);
    READRAWRAT(rat_max_exp);
    READRAWRAT(rat_min_exp);
    READRAWRAT(rat_max_fact);
    READRAWRAT(rat_min_fact);
    READRAWRAT(rat_min_i32);
    READRAWRAT(rat_max_i32);
}

//---------------------------------------------------------------------------
//
//  FUNCTION: trimit
//
//  ARGUMENTS:  PRAT *px, int32_t precision
//
//
//  DESCRIPTION: Chops off digits from rational numbers to avoid time
//  explosions in calculations of functions using series.
//  It can be shown that it is enough to only keep the first n digits
//  of the largest of p or q in the rational p over q form, and of course
//  scale the smaller by the same number of digits.  This will give you
//  n-1 digits of accuracy.  This dramatically speeds up calculations
//  involving hundreds of digits or more.
//  The last part of this trim dealing with exponents never affects accuracy
//
//  RETURN: none, modifies the pointed to PRAT
//
//---------------------------------------------------------------------------

void trimit( PRAT *px, int32_t precision)

{
    if ( !g_ftrueinfinite )
        {
        int32_t trim;
        PNUMBER pp=(*px)->pp;
        PNUMBER pq=(*px)->pq;
        trim = g_ratio * (min((pp->cdigit+pp->exp),(pq->cdigit+pq->exp))-1) - precision;
        if ( trim > g_ratio )
            {
            trim /= g_ratio;

            if ( trim <= pp->exp )
                {
                pp->exp -= trim;
                }
            else
                {
                copy(pp->mant.begin() + (trim-pp->exp), pp->mant.begin(), pp->cdigit-trim+pp->exp);
                pp->cdigit -= trim-pp->exp;
                pp->exp = 0;
                }

            if ( trim <= pq->exp )
                {
                pq->exp -= trim;
                }
            else
                {
                copy(pq->mant.begin() + (trim-pq->exp), pq->mant.begin(), pq->cdigit-trim+pq->exp);
                pq->cdigit -= trim-pq->exp;
                pq->exp = 0;
                }
            }
        trim = min(pp->exp,pq->exp);
        pp->exp -= trim;
        pq->exp -= trim;
        }
}
