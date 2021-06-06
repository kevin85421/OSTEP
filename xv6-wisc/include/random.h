#ifndef _RANDOM_H_
#define _RANDOM_H_
// Ref: https://github.com/mgwang37/PRBS/blob/master/C/PRBS31.c
static uint get_bit (uint pp)
{
	uint p_v1;
	uint p_v2;

	if (pp & (0x1<<30))
	{
		p_v1 = 1;
	}
	else
	{
		p_v1 = 0;
	}

	if (pp & (0x1<<27))
	{
		p_v2 = 1;
	}
	else
	{
		p_v2 = 0;
	}
	return (p_v1 ^ p_v2);
}

int random(int seed)
{
	uint p_index = 0;
	uint move_time = 31;

	while (1)
	{
		if (get_bit(seed))
		{
			seed <<=1;
			seed += 1;
			move_time++;
		}
		else
		{
			seed <<=1;
			move_time++;
		}
		p_index ++;

		if ((seed & 0x7fffffff) == 0x7fffffff)
		{
			break;
		}

		if (move_time == 32)
		{
			move_time =0;
            return seed;
		}
	}

    return seed;
}
#endif // _RANDOM_H_