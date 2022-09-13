
//
// @@ Copyright (c) 2014, ramenhut. All rights reserved @@
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// * Redistributions of source code must retain the above copyright notice, this
//   list of conditions and the following disclaimer.
//
// * Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions and the following disclaimer in the documentation
//   and/or other materials provided with the distribution.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
// FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
// DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
// CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
// OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//

#ifndef __HALF_H__
#define __HALF_H__

#ifndef CONST
#define CONST                           const
#endif

//
// FLOAT16 Helpers
//

#define HALF_SIGN_SHIFT                 (15)
#define HALF_EXP_SHIFT                  (10)
#define HALF_MANT_SHIFT                 (0)

#define HALF_SIGN_MASK                  (0x8000)
#define HALF_EXP_MASK                   (0x7C00)
#define HALF_MANT_MASK                  (0x03FF)

#define HALF_POS_INFINITY               (0x7C00)
#define HALF_NEG_INFINITY               (0xFC00)

#define GET_HALF_SIGN_BIT(x)            ((x) >> HALF_SIGN_SHIFT)
#define GET_HALF_EXP_BITS(x)            (((x) >> HALF_EXP_SHIFT) & 0x1F)
#define GET_HALF_MANT_BITS(x)           ((x) & HALF_MANT_MASK)

#define SET_HALF_SIGN_BIT(x,dest)       ((dest) = ((((x) << HALF_SIGN_SHIFT) & HALF_SIGN_MASK) | ( (dest) & ( HALF_EXP_MASK  | HALF_MANT_MASK ))))
#define SET_HALF_EXP_BITS(x,dest)       ((dest) = ((((x) << HALF_EXP_SHIFT)  & HALF_EXP_MASK)  | ( (dest) & ( HALF_SIGN_MASK | HALF_MANT_MASK ))))
#define SET_HALF_MANT_BITS(x,dest)      ((dest) = ((((x) << HALF_MANT_SHIFT) & HALF_MANT_MASK) | ( (dest) & ( HALF_SIGN_MASK | HALF_EXP_MASK ))))

//
// float Helpers
//

#define SINGLE_SIGN_SHIFT               (31)
#define SINGLE_EXP_SHIFT                (23)
#define SINGLE_MANT_SHIFT               (0)

#define SINGLE_SIGN_MASK                (0x80000000)
#define SINGLE_EXP_MASK                 (0x7F800000)
#define SINGLE_MANT_MASK                (0x007FFFFF)

#define SINGLE_POS_INFINITY             (0x7F800000)
#define SINGLE_NEG_INFINITY             (0xFF800000)

#define GET_SINGLE_SIGN_BIT(x)          ((x) >> SINGLE_SIGN_SHIFT)
#define GET_SINGLE_EXP_BITS(x)          (((x) >> SINGLE_EXP_SHIFT) & 0xFF)
#define GET_SINGLE_MANT_BITS(x)         ((x) & SINGLE_MANT_MASK)

#define SET_SINGLE_SIGN_BIT(x,dest)     ((dest) = ((((x) << SINGLE_SIGN_SHIFT) & SINGLE_SIGN_MASK) | ( (dest) & ( SINGLE_EXP_MASK  | SINGLE_MANT_MASK ))))
#define SET_SINGLE_EXP_BITS(x,dest)     ((dest) = ((((x) << SINGLE_EXP_SHIFT)  & SINGLE_EXP_MASK)  | ( (dest) & ( SINGLE_SIGN_MASK | SINGLE_MANT_MASK ))))
#define SET_SINGLE_MANT_BITS(x,dest)    ((dest) = ((((x) << SINGLE_MANT_SHIFT) & SINGLE_MANT_MASK) | ( (dest) & ( SINGLE_SIGN_MASK | SINGLE_EXP_MASK ))))


#define CONVERT_PATTERN( x )  ( reinterpret_cast<unsigned int *>( &x ) )


class FLOAT16
{
    unsigned short  m_uiFormat;

public:

    FLOAT16() : m_uiFormat(0) {}
    FLOAT16( CONST FLOAT16 & rhs ) : m_uiFormat(rhs.m_uiFormat) {}
    FLOAT16( CONST float & rhs ) {
        (*this) = rhs;
    }
    ~FLOAT16() {}

    //
    // Member operations
    //
    // (!) Note: the float16 (i.e. half) format is provided for storage purposes 
    //           only, and should not be used for computation. As a result, we do 
    //           not provide any arithmetic operators.
    //

    bool      operator == ( CONST FLOAT16 & rhs ) CONST {
        return !((*this) == rhs);
    }
    bool      operator != ( CONST FLOAT16 & rhs ) CONST {
        return m_uiFormat == rhs.m_uiFormat;
    }

    FLOAT16 & operator = ( CONST FLOAT16 & rhs ) {
        m_uiFormat = rhs.m_uiFormat;

        return (*this);
    }
    FLOAT16 & operator = ( CONST float & rhs )
    {
        (*this) = ToFloat16(rhs);

        return (*this);
    }
              operator float()
              {
                  return Tofloat(*this);
              }


    //
    // Conversion control
    //

    static float Tofloat( FLOAT16 rhs )
    {
        float fOutput = 0;                                  // floating point result
        unsigned int* uiOutput = CONVERT_PATTERN(fOutput);         // bit manipulated output

        if (0 == rhs.m_uiFormat)           return 0.0f;       // +zero
        else if (0x8000 == rhs.m_uiFormat) return -0.0f;      // -zero

        unsigned int uiHalfSignBit = GET_HALF_SIGN_BIT(rhs.m_uiFormat);
        unsigned int uiHalfMantBits = GET_HALF_MANT_BITS(rhs.m_uiFormat) << 13;
        INT32  iHalfExpBits = GET_HALF_EXP_BITS(rhs.m_uiFormat);

        //
        // Next we check for additional special cases:
        //

        if (0 == iHalfExpBits)
        {
            //
            // Denormalized values
            //

            SET_SINGLE_SIGN_BIT(uiHalfSignBit, (*uiOutput));
            SET_SINGLE_EXP_BITS(0, (*uiOutput));
            SET_SINGLE_MANT_BITS(uiHalfMantBits, (*uiOutput));
        }

        else if (0x1F == iHalfExpBits)
        {
            if (0 == uiHalfMantBits)
            {
                //
                // +- Infinity
                //

                (*uiOutput) = (uiHalfSignBit ? SINGLE_NEG_INFINITY : SINGLE_POS_INFINITY);
            }
            else
            {
                //
                // (S/Q)NaN
                //

                SET_SINGLE_SIGN_BIT(uiHalfSignBit, (*uiOutput));
                SET_SINGLE_EXP_BITS(0xFF, (*uiOutput));
                SET_SINGLE_MANT_BITS(uiHalfMantBits, (*uiOutput));
            }
        }

        else
        {
            //
            // Normalized values
            //

            SET_SINGLE_SIGN_BIT(uiHalfSignBit, (*uiOutput));
            SET_SINGLE_EXP_BITS((iHalfExpBits - 15) + 127, (*uiOutput));
            SET_SINGLE_MANT_BITS(uiHalfMantBits, (*uiOutput));
        }

        //
        // ATP: uiOutput equals the bit pattern of our floating point result.
        //

        return fOutput;
    }

    static FLOAT16 ToFloat16( float rhs )
    {
        //
        // (!) Truncation will occur for values outside the representable range for float16.
        //   

        FLOAT16 fOutput;
        unsigned int uiInput = *CONVERT_PATTERN(rhs);

        if (0.0f == rhs)
        {
            fOutput.m_uiFormat = 0;
            return fOutput;
        }

        else if (-0.0f == rhs)
        {
            fOutput.m_uiFormat = 0x8000;
            return fOutput;
        }

        unsigned int uiSignBit = GET_SINGLE_SIGN_BIT(uiInput);
        unsigned int uiMantBits = GET_SINGLE_MANT_BITS(uiInput) >> 13;
        INT32  iExpBits = GET_SINGLE_EXP_BITS(uiInput);

        //
        // Next we check for additional special cases:
        //

        if (0 == iExpBits)
        {
            //
            // Denormalized values
            //

            SET_HALF_SIGN_BIT(uiSignBit, fOutput.m_uiFormat);
            SET_HALF_EXP_BITS(0, fOutput.m_uiFormat);
            SET_HALF_MANT_BITS(uiMantBits, fOutput.m_uiFormat);
        }

        else if (0xFF == iExpBits)
        {
            if (0 == uiMantBits)
            {
                //
                // +- Infinity
                //

                fOutput.m_uiFormat = (uiSignBit ? HALF_NEG_INFINITY : HALF_POS_INFINITY);
            }
            else
            {
                //
                // (S/Q)NaN
                //

                SET_HALF_SIGN_BIT(uiSignBit, fOutput.m_uiFormat);
                SET_HALF_EXP_BITS(0x1F, fOutput.m_uiFormat);
                SET_HALF_MANT_BITS(uiMantBits, fOutput.m_uiFormat);
            }
        }

        else
        {
            //
            // Normalized values
            //

            INT32 iExponent = iExpBits - 127 + 15;

            if (iExponent < 0) { iExponent = 0; }
            else if (iExponent > 31) iExponent = 31;

            SET_HALF_SIGN_BIT(uiSignBit, fOutput.m_uiFormat);
            SET_HALF_EXP_BITS(iExponent, fOutput.m_uiFormat);
            SET_HALF_MANT_BITS(uiMantBits, fOutput.m_uiFormat);
        }

        //
        // ATP: uiOutput equals the bit pattern of our floating point result.
        //

        return fOutput;
    }

    //
    // The faster variants handle only the most common normalized conversion case.
    // If a conversion requires QNaN, SNaN, Inf, or denormalized handling, do not
    // use these.
    //

    static float TofloatFast( FLOAT16 rhs )
    {
        float fOutput = 0;                                  // floating point result
        unsigned int* uiOutput = CONVERT_PATTERN(fOutput);         // bit manipulated output

        if (0 == rhs.m_uiFormat)           return 0.0f;       // +zero
        else if (0x8000 == rhs.m_uiFormat) return -0.0f;      // -zero

        unsigned int uiHalfSignBit = GET_HALF_SIGN_BIT(rhs.m_uiFormat);
        unsigned int uiHalfMantBits = GET_HALF_MANT_BITS(rhs.m_uiFormat) << 13;
        INT32  iHalfExpBits = GET_HALF_EXP_BITS(rhs.m_uiFormat);

        //
        // Normalized values
        //

        SET_SINGLE_SIGN_BIT(uiHalfSignBit, (*uiOutput));
        SET_SINGLE_EXP_BITS((iHalfExpBits - 15) + 127, (*uiOutput));
        SET_SINGLE_MANT_BITS(uiHalfMantBits, (*uiOutput));

        //
        // ATP: uiOutput equals the bit pattern of our floating point result.
        //

        return fOutput;
    }

    static FLOAT16 ToFloat16Fast( float rhs )
    {
        //
        // (!) Truncation will occur for values outside the representable range for float16.
        //   

        FLOAT16 fOutput;
        unsigned int uiInput = *CONVERT_PATTERN(rhs);

        if (0.0f == rhs)
        {
            fOutput.m_uiFormat = 0;
            return fOutput;
        }

        else if (-0.0f == rhs)
        {
            fOutput.m_uiFormat = 0x8000;
            return fOutput;
        }

        unsigned int uiSignBit = GET_SINGLE_SIGN_BIT(uiInput);
        unsigned int uiMantBits = GET_SINGLE_MANT_BITS(uiInput) >> 13;
        INT32  iExpBits = GET_SINGLE_EXP_BITS(uiInput);

        //
        // Normalized values
        //

        INT32 iExponent = iExpBits - 127 + 15;

        if (iExponent < 0) { iExponent = 0; }
        else if (iExponent > 31) iExponent = 31;

        SET_HALF_SIGN_BIT(uiSignBit, fOutput.m_uiFormat);
        SET_HALF_EXP_BITS(iExponent, fOutput.m_uiFormat);
        SET_HALF_MANT_BITS(uiMantBits, fOutput.m_uiFormat);

        //
        // ATP: uiOutput equals the bit pattern of our floating point result.
        //

        return fOutput;
    }

    unsigned short GetRaw() {
        return m_uiFormat;
    }
};

FLOAT16 operator "" _h(long double);

#endif // __HALF_H__
