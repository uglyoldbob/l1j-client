#include <stdio.h>

#include "mpuint.h" 

mpuint::mpuint(unsigned len)
{ 
  length = len; 
  value = new unsigned short[len]; 
}

mpuint::mpuint(const mpuint &n) 
{ 
  length = n.length;
  value = new unsigned short[length]; 
  unsigned i; 
  for (i = 0; i < length; i++) 
    value[i] = n.value[i]; 
} 

mpuint::~mpuint()
{
  delete [] value;
}

void mpuint::operator = (const mpuint &n)
{
  unsigned i;
  for (i = 0; i < length && i < n.length; i++)
    value[i] = n.value[i];
  for (; i < length; i++)
    value[i] = 0;
  for (; i < n.length; i++)
  {
    if (n.value[i] != 0)
      numeric_overflow();
  }
}

void mpuint::operator = (unsigned short n)
{
  value[0] = n;
  unsigned i;
  for (i = 1; i < length; i++)
    value[i] = 0;
}

void mpuint::operator += (const mpuint &n)
{
  unsigned i;
  unsigned long carry = 0;
  for (i = 0; i < length; i++)
  {
    unsigned long sum = value[i] + (i < n.length ? n.value[i] : 0) + carry;
    value[i] = sum;
    carry = sum >> 16;
  }
  if (carry != 0)
    numeric_overflow();
  for (; i < n.length; i++)
  {
    if (n.value[i] != 0)
      numeric_overflow();
  }
}

void mpuint::operator += (unsigned short n)
{
  value[0] += n;
  if (value[0] < n)
  {
    unsigned i;
    for (i = 1; i < length; i++)
    {
      if (++value[i] != 0)
        return;
    }
    numeric_overflow();
  }
}

void mpuint::operator -= (const mpuint &n)
{
  unsigned i;
  unsigned long borrow = 0;
  for (i = 0; i < length; i++)
  {
    unsigned long subtrahend = (i < n.length ? n.value[i] : 0) + borrow;
    borrow = (unsigned long)(value[i]) < subtrahend;
    value[i] -= subtrahend;
  }
  if (borrow != 0)
    numeric_overflow();
  for (; i < n.length; i++)
  {
    if (n.value[i] != 0)
      numeric_overflow();
  }
}

void mpuint::operator -= (unsigned short n)
{
  if (value[0] >= n)
    value[0] -= n;
  else
  {
    value[0] -= n;
    for (unsigned i = 1; i < length; i++)
    {
      if (--value[i] != 0xFFFF)
        return;
    }
    numeric_overflow();
  }
}

void mpuint::operator *= (const mpuint &n)
{
  unsigned i;
  unsigned short *multiplier = new unsigned short[length];
  for (i = 0; i < length; i++)
  {
    multiplier[i] = value[i];
    value[i] = 0;
  }
  for (i = 0; i < length; i++)
  {
    unsigned j;
    for (j = 0; j < n.length; j++)
    {
      unsigned long product = multiplier[i] * n.value[j];
      unsigned k = i + j;
      while (product != 0)
      {
        if (k >= length)
          numeric_overflow();
        product += value[k];
        value[k] = product;
        product >>= 16;
        k++;
      }
    }
  }
  delete [] multiplier;
}

void mpuint::operator *= (unsigned short n)
{
  unsigned i;
  unsigned long product = 0;
  for (i = 0; i < length; i++)
  {
    product += n * value[i];
    value[i] = product;
    product >>= 16;
  }
  if (product != 0)
    numeric_overflow();
}

unsigned short mpuint::remainder(unsigned short n)
{
  unsigned i = length;
  unsigned rem = 0;
  while (i-- != 0)
  {
    unsigned long dividend = (unsigned long) rem << 16 |
      (unsigned long) value[i];
    value[i] = dividend / n;
    rem = dividend % n;
  }
  return rem;
}

void mpuint::operator /= (unsigned short n)
{
  (void) remainder(n);
}

void mpuint::operator %= (unsigned short n)
{
  *this = remainder(n);
}

int mpuint::Compare(const mpuint &n) const
{
  unsigned i;
  if (length > n.length)
  {
    for (i = length-1; i >= n.length; i--)
    {
      if (value[i] != 0)
        return 1;
    }
  }
  else if (n.length > length)
  {
    for (i = n.length-1; i >= length; i--)
    {
      if (n.value[i] != 0)
        return -1;
    }
  }
  else
    i = length-1;
  while (true)
  {
    if (value[i] > n.value[i])
      return 1;
    if (value[i] < n.value[i])
      return -1;
    if (i == 0)
      return 0;
    i--;
  }
}

int mpuint::Compare(unsigned short n) const
{
  unsigned i;
  for (i = length-1; i >= 1; i--)
  {
    if (value[i] != 0)
      return 1;
  }
  return value[0] > n ? 1 : value[0] < n ? -1 : 0;
}


bool mpuint::IsZero(void) const
{
  unsigned i;
  for (i = 0; i < length; i++)
  {
    if (value[i] != 0)
      return false;
  }
  return true;
}

char *mpuint::edit(char *s) const
{
  mpuint n(*this);
  unsigned i = 0; 
  do 
    s[i++] = n.remainder(10) + '0'; 
  while (!n.IsZero());
  s[i] = 0; 
  unsigned j; 
  for (j = 0; --i > j; j++) 
  { 
    char c = s[i]; 
    s[i] = s[j];
    s[j] = c; 
  }
  return s; 
}

bool mpuint::scan(const char *&s) 
{ 
  const char *t = s; 
  bool found = false; 
  while (*t == ' ' || *t == '\t') 
    t++;
  *this = 0; 
  while ('0' <= *t && *t <= '9') 
  { 
    found = true;
    *this *= 10; 
    *this += (unsigned short) (*t++ - '0'); 
  } 
  s = t; 
  return found; 
}

void mpuint::shift(unsigned bit)
{ 
  for (unsigned i = 0; i < length; i++)
  { 
    unsigned long x = value[i] << 1 | bit; 
    value[i] = x; 
    bit = x >> 16; 
  } 
  if (bit != 0) 
    numeric_overflow();
} 


void mpuint::Divide(const mpuint &dividend, const mpuint &divisor, mpuint &quotient,
  mpuint &remainder)
{ 
  if (divisor.IsZero())
    numeric_overflow(); 
  remainder = 0; 
  quotient = 0;
  unsigned i = dividend.length; 
  while (i-- != 0)
  { 
    unsigned bit = 16;
    while (bit-- != 0) 
    { 
      remainder.shift(dividend.value[i] >> bit & 1); 
      if (divisor <= remainder) 
      { 
        quotient.shift(1); 
        // unsigned borrow = 0;
        // for (unsigned j = 0; j < divisor.length; j++) 
        // { 
        //   unsigned long subtrahend = divisor.value[j] + borrow; 
        //   borrow = remainder.value[j] < subtrahend;
        //   remainder.value[j] -= subtrahend;
        // } 
        remainder -= divisor; 
      } 
      else 
        quotient.shift(false);
    }
  }
} 

void mpuint::operator /= (const mpuint &n) 
{ 
  mpuint remainder(n.length); 
  mpuint dividend(*this); 
  Divide(dividend, n, *this, remainder);
} 

void mpuint::operator %= (const mpuint &n) 
{ 
  mpuint quotient(length); 
  mpuint dividend(*this);
  Divide(dividend, n, quotient, *this);
} 

void mpuint::Power(const mpuint &base, const mpuint &exponent,
  const mpuint &modulus, mpuint &result)
{
  mpuint r(2*base.length+1);
  r = 1;
  bool one = true;
  unsigned i = exponent.length;
  while (i-- != 0)
  {
    unsigned bit = 1 << 15;
    do
    {
      if (!one)
      {
        mpuint n(r);
        r *= n;
        r %= modulus;
      }
      if (exponent.value[i] & bit)
      {
        r *= base;
        r %= modulus;
        one = false;
      }
      bit >>= 1;
    } while (bit != 0);
  }
  result = r;
}


void mpuint::dump() const
{
  unsigned i;
  for (i = 0; i < length; i++)
    printf(" %x", value[i]);
  putchar('\n');
}
