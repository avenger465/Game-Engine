//--------------------------------------------------------------------------------------
// Vector3 class (cut down version), to hold points and vectors
//--------------------------------------------------------------------------------------

#include "epch.h"
#include "CVector3.h"

/*-----------------------------------------------------------------------------------------
    Operators
-----------------------------------------------------------------------------------------*/

// Addition of another vector to this one, e.g. Position += Velocity
CVector3& CVector3::operator+= (const CVector3& v)
{
    x += v.x;
    y += v.y;
    z += v.z;
    return *this;
}

// Subtraction of another vector from this one, e.g. Velocity -= Gravity
CVector3& CVector3::operator-= (const CVector3& v)
{
    x -= v.x;
    y -= v.y;
    z -= v.z;
    return *this;
}

CVector3& CVector3::operator*=(const CVector3& v)
{
    x *= v.x;
    y *= v.y;
    z *= v.z;
    return *this;
}

// Negate this vector (e.g. Velocity = -Velocity)
CVector3& CVector3::operator- ()
{
    x = -x;
    y = -y;
    z = -z;
    return *this;
}

// Plus sign in front of vector - called unary positive and usually does nothing. Included for completeness (e.g. Velocity = +Velocity)
CVector3& CVector3::operator+ ()
{
    return *this;
}


// Multiply vector by scalar (scales vector);
CVector3& CVector3::operator*= (const float s)
{
    x *= s;
    y *= s;
    z *= s;
    return *this;
}

CVector3& CVector3::operator^(const CVector3& v)
{
    return Cross(v);
    // TODO: insert return statement here
}

void CVector3::Normalise()
{
    // Reduce vector to unit length - member function
    float lengthSq = x * x + y * y + z * z;

    // Ensure vector is not zero length (use BaseMath.h float approx. fn with default epsilon)
    if (IsZero(lengthSq))
    {
        x = y = z = 0.0f;
    }
    else
    {
        float invLength = InvSqrt(lengthSq);
        x *= invLength;
        y *= invLength;
        z *= invLength;
    }
}

CVector3 CVector3::Cross(const CVector3& v)
{

    return CVector3{ y * v.z - z * v.y, z * v.x - x * v.z, x * v.y - y * v.x };
}


// Vector-vector addition
CVector3 operator+ (const CVector3& v, const CVector3& w)
{
    return CVector3{ v.x + w.x, v.y + w.y, v.z + w.z };
}

// Vector-vector subtraction
CVector3 operator- (const CVector3& v, const CVector3& w)
{
    return CVector3{ v.x - w.x, v.y - w.y, v.z - w.z };
}

// Vector-scalar multiplication
CVector3 operator* (const CVector3& v, float s)
{
    return CVector3{ v.x * s, v.y * s, v.z * s };
}
CVector3 operator* (float s, const CVector3& v)
{
    return CVector3{ v.x * s, v.y * s, v.z * s };
}

/*-----------------------------------------------------------------------------------------
    Non-member functions
-----------------------------------------------------------------------------------------*/

// Dot product of two given vectors (order not important) - non-member version
float Dot(const CVector3& v1, const CVector3& v2)
{
    return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
}

// Cross product of two given vectors (order is important) - non-member version
CVector3 Cross(const CVector3& v1, const CVector3& v2)
{
    return CVector3{ v1.y * v2.z - v1.z * v2.y, v1.z * v2.x - v1.x * v2.z, v1.x * v2.y - v1.y * v2.x };
}

// Return unit length vector in the same direction as given one
CVector3 Normalise(const CVector3& v)
{
    float lengthSq = v.x*v.x + v.y*v.y + v.z*v.z;

    // Ensure vector is not zero length (use BaseMath.h float approx. fn with default epsilon)
    if (IsZero(lengthSq))
    {
        return CVector3{ 0.0f, 0.0f, 0.0f };
    }
    else
    {
        float invLength = InvSqrt(lengthSq);
        return CVector3{ v.x * invLength, v.y * invLength, v.z * invLength };
    }
}


// Returns length of a vector
float Length(const CVector3& v)
{
    return sqrt(Dot(v, v));
}

const CVector3 CVector3::kZero(0.0f, 0.0f, 0.0f);
