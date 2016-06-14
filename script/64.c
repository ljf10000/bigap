#include <stdio.h>
#include <memory.h>

/*
TYPE   		LP32	ILP32  	LP64  	ILP64  	LLP64
CHAR		8		8		8		8		8
SHORT		16		16		16		16  	16
INT			16		32		32		64		32
LONG		32		32		64		64		32
LONG LONG	64		64		64		64		64
POINTER		32		32		64		64		64
*/

enum {
	LP32,
	ILP32,
	LP64,
	ILP64,
	LLP64,
	
	ENDMODE
};

enum {
	CHAR,
	SHORT,
	INT,
	LONG,
	LONGLONG,
	POINTER,
	
	ENDTYPE
};

typedef struct {
	char *name;
	int sizes[ENDTYPE];
} modes_t;

static modes_t modes[ENDMODE] = {
	[LP32] = {
		.name = "LP32",
		.sizes = {
			[CHAR] 		= 8,
			[SHORT] 	= 16,
			[INT] 		= 16,
			[LONG] 		= 32,
			[LONGLONG] 	= 64,
			[POINTER] 	= 32,
		},
	},
	
	[ILP32] = {
		.name = "ILP32",
		.sizes = {
			[CHAR] 		= 8,
			[SHORT] 	= 16,
			[INT] 		= 32,
			[LONG] 		= 32,
			[LONGLONG] 	= 64,
			[POINTER] 	= 32,
		},
	},
	[LP64] = {
		.name = "LP64",
		.sizes = {
			[CHAR] 		= 8,
			[SHORT] 	= 16,
			[INT] 		= 32,
			[LONG] 		= 64,
			[LONGLONG] 	= 64,
			[POINTER] 	= 64,
		},
	},
	[ILP64] = {
		.name = "ILP64",
		.sizes = {
			[CHAR] 		= 8,
			[SHORT] 	= 16,
			[INT] 		= 64,
			[LONG] 		= 64,
			[LONGLONG] 	= 64,
			[POINTER] 	= 64,
		},
	},
	[LLP64] = {
		.name = "LLP64",
		.sizes = {
			[CHAR] 		= 8,
			[SHORT] 	= 16,
			[INT] 		= 32,
			[LONG] 		= 32,
			[LONGLONG] 	= 64,
			[POINTER] 	= 64,
		},
	},
};

static int sizes[ENDTYPE] = {
	[CHAR] 		= 8*sizeof(char),
	[SHORT] 	= 8*sizeof(short),
	[INT] 		= 8*sizeof(int),
	[LONG] 		= 8*sizeof(long),
	[LONGLONG] 	= 8*sizeof(long long),
	[POINTER] 	= 8*sizeof(void *),
};
	
int main(int argc, char *argv[])
{
	int i;
	
	for (i=0; i<ENDMODE; i++) {
		if (0==memcmp(modes[i].sizes, sizes, sizeof(int)*ENDTYPE)) {
			printf("%s\n", modes[i].name);
			
			return 0;
		}
	}
	
	printf("UNKNOW64\n");

	return 0;
}
