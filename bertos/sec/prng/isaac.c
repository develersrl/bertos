/**
 * \file
 * <!--
 * This file is part of BeRTOS.
 *
 * Bertos is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 * As a special exception, you may use this file as part of a free software
 * library without restriction.  Specifically, if other files instantiate
 * templates or use macros or inline functions from this file, or you compile
 * this file and link it with other files to produce an executable, this
 * file does not by itself cause the resulting executable to be covered by
 * the GNU General Public License.  This exception does not however
 * invalidate any other reasons why the executable file might be covered by
 * the GNU General Public License.
 *
 * Copyright 2010 Develer S.r.l. (http://www.develer.com/)
 *
 * -->
 *
 * \brief ISAAC implementation
 * \author Giovanni Bajo <rasky@develer.com>
 *
 */

/*
------------------------------------------------------------------------------
rand.c: By Bob Jenkins.  My random number generator, ISAAC.  Public Domain.
MODIFIED:
  960327: Creation (addition of randinit, really)
  970719: use context, not global variables, for internal state
  980324: added main (ifdef'ed out), also rearranged randinit()
  010626: Note that this is public domain
------------------------------------------------------------------------------
*/

#include "isaac.h"
#include <sec/prng.h>
#include <sec/util.h>
#include <cfg/compiler.h>
#include <cfg/macros.h>
#include <string.h>

typedef uint32_t ub4;
typedef uint16_t ub2;
typedef uint8_t ub1;

#define ind(mm,x)  (*(ub4 *)((size_t)(mm) + ((x) & ((CONFIG_ISAAC_RANDSIZ-1)<<2))))
#define rngstep(mix,a,b,mm,m,m2,r,x) \
{ \
  x = *m;  \
  a = (a^(mix)) + *(m2++); \
  *(m++) = y = ind(mm,x) + a + b; \
  *(r++) = b = ind(mm,y>>CONFIG_ISAAC_RANDSIZL) + x; \
}

static void isaac(IsaacContext *ctx)
{
	register ub4 a,b,x,y,*m,*mm,*m2,*r,*mend;
	mm=ctx->randmem; r=ctx->randrsl;
	a = ctx->randa; b = ctx->randb + (++ctx->randc);
	for (m = mm, mend = m2 = m+(CONFIG_ISAAC_RANDSIZ/2); m<mend; )
	{
		rngstep( a<<13, a, b, mm, m, m2, r, x);
		rngstep( a>>6 , a, b, mm, m, m2, r, x);
		rngstep( a<<2 , a, b, mm, m, m2, r, x);
		rngstep( a>>16, a, b, mm, m, m2, r, x);
	}
	for (m2 = mm; m2<mend; )
	{
		rngstep( a<<13, a, b, mm, m, m2, r, x);
		rngstep( a>>6 , a, b, mm, m, m2, r, x);
		rngstep( a<<2 , a, b, mm, m, m2, r, x);
		rngstep( a>>16, a, b, mm, m, m2, r, x);
	}
	ctx->randb = b; ctx->randa = a;
}


#define mix(a,b,c,d,e,f,g,h) \
{ \
   a^=b<<11; d+=a; b+=c; \
   b^=c>>2;  e+=b; c+=d; \
   c^=d<<8;  f+=c; d+=e; \
   d^=e>>16; g+=d; e+=f; \
   e^=f<<10; h+=e; f+=g; \
   f^=g>>4;  a+=f; g+=h; \
   g^=h<<8;  b+=g; h+=a; \
   h^=a>>9;  c+=h; a+=b; \
}

static void isaac_reseed(PRNG *ctx_, const uint8_t *seed)
{
	IsaacContext *ctx = (IsaacContext *)ctx_;
	int i;
	ub4 a,b,c,d,e,f,g,h;
	ub4 *m,*r;

	// XOR the new seed over the current state, so to depend on
	// the previously-generated output.
	xor_block(ctx->randrsl, ctx->randrsl, seed, sizeof(ctx->randrsl));

	ctx->randa = ctx->randb = ctx->randc = 0;
	m=ctx->randmem;
	r=ctx->randrsl;
	a=b=c=d=e=f=g=h=0x9e3779b9;  /* the golden ratio */

	for (i=0; i<4; ++i)          /* scramble it */
	{
		mix(a,b,c,d,e,f,g,h);
	}

	/* initialize using the contents of r[] as the seed */
	for (i=0; i<CONFIG_ISAAC_RANDSIZ; i+=8)
	{
		a+=r[i  ]; b+=r[i+1]; c+=r[i+2]; d+=r[i+3];
		e+=r[i+4]; f+=r[i+5]; g+=r[i+6]; h+=r[i+7];
		mix(a,b,c,d,e,f,g,h);
		m[i  ]=a; m[i+1]=b; m[i+2]=c; m[i+3]=d;
		m[i+4]=e; m[i+5]=f; m[i+6]=g; m[i+7]=h;
	}
	/* do a second pass to make all of the seed affect all of m */
	for (i=0; i<CONFIG_ISAAC_RANDSIZ; i+=8)
	{
		a+=m[i  ]; b+=m[i+1]; c+=m[i+2]; d+=m[i+3];
		e+=m[i+4]; f+=m[i+5]; g+=m[i+6]; h+=m[i+7];
		mix(a,b,c,d,e,f,g,h);
		m[i  ]=a; m[i+1]=b; m[i+2]=c; m[i+3]=d;
		m[i+4]=e; m[i+5]=f; m[i+6]=g; m[i+7]=h;
	}
}

static void isaac_generate(PRNG *ctx_, uint8_t *data, size_t len)
{
	IsaacContext *ctx = (IsaacContext *)ctx_;

	STATIC_ASSERT(sizeof(ctx->randrsl) == CONFIG_ISAAC_RANDSIZ*4);

	while (len)
	{
		ASSERT(ctx->randcnt <= CONFIG_ISAAC_RANDSIZ*4);

		if (ctx->randcnt == CONFIG_ISAAC_RANDSIZ*4)
		{
			isaac(ctx);
			ctx->randcnt = 0;
		}

		size_t L = MIN(len, CONFIG_ISAAC_RANDSIZ*4 - (size_t)ctx->randcnt);
		memcpy(data, (uint8_t*)ctx->randrsl + ctx->randcnt, L);
		data += L;
		ctx->randcnt += L;
		len -= L;
	}
}


/**********************************************************************/

void isaac_init(IsaacContext *ctx)
{
	ctx->prng.reseed = isaac_reseed;
	ctx->prng.generate = isaac_generate;
	ctx->prng.seed_len = sizeof(ctx->randrsl);
	ctx->prng.seeded = 0;

	ctx->randcnt = CONFIG_ISAAC_RANDSIZ*4;
	memset(ctx->randrsl, 0, sizeof(ctx->randrsl));
}




#ifdef NEVER
int main()
{
  ub4 i,j;
  randctx ctx;
  ctx.randa=ctx.randb=ctx.randc=(ub4)0;
  for (i=0; i<256; ++i) ctx.randrsl[i]=(ub4)0;
  randinit(&ctx, TRUE);
  for (i=0; i<2; ++i)
  {
    isaac(&ctx);
    for (j=0; j<256; ++j)
    {
      printf("%.8lx",ctx.randrsl[j]);
      if ((j&7)==7) printf("\n");
    }
  }
}
#endif
