#include <stdio.h>
#include <stdlib.h>
#include <string.h>

unsigned char * arena;
int n;

void alloc(int );
void fill(int, int, int);
void dump();
void free_arena(int );
void show(char *);

int byte2int(int pos)
{
    return ((arena[pos+3]<<24)|(arena[pos+2]<<16)|(arena[pos+1]<<8)|arena[pos]);
}

void int2byte(int pos, int value)
{
    arena[pos]=value&((1<<8)-1);
    value=value>>8;

    arena[pos+1]=value&((1<<8)-1);
    value=value>>8;

    arena[pos+2]=value&((1<<8)-1);
    value=value>>8;

    arena[pos+3]=value&((1<<8)-1);
}

int main()
{

    char instruction[20];
    int size, index, val;

    scanf("%s%d", &instruction, &n);
    arena=(unsigned char * )calloc(n, sizeof(unsigned char));

    while(strcmp(instruction, "FINALIZE")!=0)
    {
        scanf("%s", &instruction);

        if(strcmp(instruction, "ALLOC")==0)
        {
            scanf("%d", &size);
            alloc(size);
        }

        if(strcmp(instruction, "FILL")==0)
        {
            scanf("%d%d%d", &index, &size, &val);
            fill(index, size, val);
        }

        if(strcmp(instruction, "FREE")==0)
        {
            scanf("%d", &index);
            free_arena(index);
        }

        if(strcmp(instruction, "SHOW")==0)
        {
            scanf("%s", &instruction);
            show(instruction);
        }
        if(strcmp(instruction, "DUMP")==0)
            dump();
    }
	
	free(arena);
    
	return 0;
}

void alloc(int size)
{
    size+=12;

    int pos=byte2int(0);
    int i=4, ant=0, ok=1;

    while(ok)
    {
        if(pos!=0)
        {
            if(pos-i>=size)
            {
                int2byte(i, pos);
                int2byte(i+4, ant);
                int2byte(i+8, size);

                int2byte(ant, i);
                int2byte(pos+4, i);

                ok=0;
                printf("%d\n", i+12);

            }
        }
        else
        {
            if(n-1-i>=size)
            {
                int2byte(i, 0);
                int2byte(i+4, ant);
                int2byte(i+8, size);

                int2byte(ant, i);

                ok=0;
                printf("%d\n", i+12);
            }
            else break;
        }

        ant=pos;
        pos=byte2int(pos);
        i=ant+byte2int(ant+8);
    }

    if(ok) printf("0\n");
}

void fill(int index, int size, int val)
{
    int i;
    for(i=0; i<size; ++i)
        arena[index+i]=val;
}

void dump()
{
    int i;
    for(i=0; i<n; ++i)
    {
        if(i%16==0)
        {
            if(i!=0) printf("\n");
            printf("%08X\t", i);
        }
        printf("%02X ", arena[i]);
        if((i+1)%16==8) printf(" ");
    }
    printf("\n%08X\n", i);
}

void free_arena(int index)
{
    int next=byte2int(index-12), prev=byte2int(index-8);

    if(next!=0) int2byte(next+4, prev);
    int2byte(prev, next);

}

void show(char * instruction)
{
    if(strcmp(instruction, "FREE")==0)
    {
        int pos=byte2int(0);
        int ant=0, i=4;
        int nr_blocks=0, nr_bytes=0;

        if(pos==0)
        {
            printf("1 blocks (%d bytes) free\n", n-4);
        }
        else
        {
            while(pos!=0)
            {
                if(pos-i>0)
                {
                    ++nr_blocks;
                    nr_bytes+=(pos-i);
                }
                ant=pos;
                pos=byte2int(pos);
                i=ant+byte2int(ant+8);
            }

            if(n-i>0)
            {
                ++nr_blocks;
                nr_bytes+=(n-i);
            }

            printf("%d blocks (%d bytes) free\n", nr_blocks, nr_bytes);
        }
        
    }

    if(strcmp(instruction, "USAGE")==0)
    {
        int pos=byte2int(0);
        int ant=0, i=4;
        int nr_blocksfree=0, nr_bytes=0, nr_blocksused=0, nr_bytesused=4;

        if(pos==0)
        {
           	printf("0 blocks (0 bytes) used\n");
            printf("100\n0\n");
        }
        else
        {
            while(pos!=0)
            {
                if(pos-i>0) ++nr_blocksfree;
                nr_bytes+=(byte2int(pos+8)-12);
                ++nr_blocksused;

                nr_bytesused+=byte2int(pos+8);

                ant=pos;
                pos=byte2int(pos);
                i=ant+byte2int(ant+8);
            }

            if(n-i>0) ++nr_blocksfree;

            printf("%d blocks (%d bytes) used\n", nr_blocksused, nr_bytes);
            printf("%d%% efficiency\n", 100*nr_bytes/nr_bytesused);
            printf("%d%% fragmentation\n", 100*(nr_blocksfree-1)/nr_blocksused);
        }
    }

    if(strcmp(instruction, "ALLOCATIONS")==0)
    {
        printf("OCCUPIED 4 bytes\n");
        int pos=byte2int(0), i=4, ant;

        while(pos!=0)
        {
            if(pos-i>0) printf("FREE %d bytes\n", pos-i);
            printf("OCCUPIED %d bytes\n", byte2int(pos+8));

            ant=pos;
            pos=byte2int(pos);
            i=ant+byte2int(ant+8);
        }

        if(n-i>0)
        {
            printf("FREE %d bytes\n", n-i);
        }
    }
}