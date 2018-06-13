#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

const int MAX_HEAP=20*(1<<20);

char *g_headptr;
char *g_tailptr;

unsigned int* cvt(char* p)
{
    return (unsigned int*)p;
}

//b代表block,这里是八字节对齐
int b_size(char *p)
{
    return *cvt(p)&(~0x7);
}

int pack(int sz,int allo)
{
    return sz|allo;
}

//当前block有没有被分配
int b_allo(char *p)
{
    return *cvt(p)&0x1;    
}

int b_pre_allo(char *p)
{
    return *cvt(p)&0x2;
}

//第一个块会返回head的地址，注意，此函数只在空块中被调用
char* pre_blk(char *p)
{
    if(p == g_headptr){
        return p;
    }
    int sz = b_size((char*)(cvt(p)-1));
    return p-sz;
}

void w_empty(char *p,unsigned int info)
{
    *cvt(p) = info;
    *cvt(p + b_size(p)-4) = info;
    return; 
}

char* nxt_blk(char *p)
{
    if(p == g_tailptr){
        return g_tailptr;
    }
    return p+b_size(p);
}

void pool_init()
{
    g_headptr = (char*)malloc(MAX_HEAP);
    g_tailptr = (char*)(cvt(g_headptr)+2);
}

//返回处理后，最后一段空白的地址
char* extend_heap(int sz)
{
    char *h;
    if(b_pre_allo(g_tailptr) == 0){
        h = pre_blk(g_tailptr);
    }else{
        h=g_tailptr;
    }
    g_tailptr += sz - b_size(h);
    w_empty(h,pack(sz,b_pre_allo(h)));
    //8是因为tail占了两个int字节
    w_empty(g_tailptr,pack(8,0));
    return h;
}

void *alloc(int sz)
{
    char *cur = g_headptr;
    //4是因为头部需要4个char放块的大小
    sz = (sz-1+4)/8*8+8;
    while(b_size(cur)<sz&&b_allo(cur)==0&&cur!=g_tailptr){
        cur= nxt_blk(cur);
    }
    
    if(cur == g_tailptr){
        cur = extend_heap(sz);    
    }
    int ori_sz = b_size(cur);
    //少于两个字节空闲则直接加入占用
    if(ori_sz-sz<8){
        *cvt(cur) = pack(ori_sz,b_pre_allo(cur) & 0x1);
    }else{
        *cvt(cur) = pack(sz,b_pre_allo(cur) & 0x1);
        
        w_empty(cur+sz,pack(ori_sz-sz,0x2));
    }
    return (void*)(cur+4);
}


void release(void *p)
{
    char *c = (char*)p;
    if(b_pre_allo((char*)p)>0){
        c = pre_blk((char*)p);
        w_empty(c,pack(b_size(c) + b_size((char*)p),b_pre_allo(c)));
    }
    char *n = nxt_blk(c);
    if(n!=g_tailptr&& b_allo(n)==0){
        w_empty(c,pack(b_size(c)+b_size(n),b_pre_allo(c)));
    }
    return;
}



