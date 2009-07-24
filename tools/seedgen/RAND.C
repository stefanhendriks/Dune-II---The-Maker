extern unsigned long seed;

short random()
{
   unsigned char *s = (unsigned char *)&seed, a, b, x, y;

   a = *(s+0);
   x = (a & 0x2) >> 1;
   a >>= 2;
   b = *(s+2);
   y = (b & 0x80) >> 7;
   b <<= 1;
   b |= x;
   *(s+2) = b;
   b = *(s+1);
   x = (b & 0x80) >> 7;
   b <<= 1;
   b |= y;
   *(s+1) = b;
   x = 1 - x;
   b = *(s+0);
   a -= b + x;
   x = a & 0x1;
   a >>= 1;
   b = *(s+0);
   b >>= 1;
   b |= (x << 7);
   *(s+0) = b;
   a = *(s+0);
   b = *(s+1);
   a = (a | b) & (~(a & b));
   a &= 0xFF;
   return a;
}
