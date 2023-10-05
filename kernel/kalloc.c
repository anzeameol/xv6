// Physical memory allocator, for user processes,
// kernel stacks, page-table pages,
// and pipe buffers. Allocates whole 4096-byte pages.

#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "riscv.h"
#include "defs.h"
#include "proc.h"

void freerange(void *pa_start, void *pa_end);

extern char end[]; // first address after kernel.
                   // defined by kernel.ld.

struct run
{
  struct run *next;
};

struct
{
  struct spinlock lock;
  struct run *freelist;
} kmem[NCPU];

void kinit()
{
  for (int i = 0; i < NCPU; i++)
  {
    initlock(&kmem[i].lock, "kmem");
  }
  freerange(end, (void *)PHYSTOP);
}

// new kfree function, which the cpuid is set
void kfreeID(void *pa, int cpuID)
{
  struct run *r;

  if (((uint64)pa % PGSIZE) != 0 || (char *)pa < end || (uint64)pa >= PHYSTOP)
    panic("kfree");

  // Fill with junk to catch dangling refs.
  memset(pa, 1, PGSIZE);

  r = (struct run *)pa;

  acquire(&kmem[cpuID].lock);
  r->next = kmem[cpuID].freelist;
  kmem[cpuID].freelist = r;
  release(&kmem[cpuID].lock);
}

void freerange(void *pa_start, void *pa_end)
{
  char *p;
  char *start;
  start = p = (char *)PGROUNDUP((uint64)pa_start);
  // printf("%x %x\n", p, (char *)pa_end);
  int step = ((char *)pa_end - p) / NCPU;
  // printf("%d\n", step);
  for (int i = 0; i < NCPU; i++)
  {
    for (; p + PGSIZE <= start + (i + 1) * step; p += PGSIZE)
      kfreeID(p, i);
  }
}

// Free the page of physical memory pointed at by pa,
// which normally should have been returned by a
// call to kalloc().  (The exception is when
// initializing the allocator; see kinit above.)
void kfree(void *pa)
{
  push_off();
  int cpuID = cpuid();
  pop_off();
  struct run *r;

  if (((uint64)pa % PGSIZE) != 0 || (char *)pa < end || (uint64)pa >= PHYSTOP)
    panic("kfree");

  // Fill with junk to catch dangling refs.
  memset(pa, 1, PGSIZE);

  r = (struct run *)pa;

  acquire(&kmem[cpuID].lock);
  r->next = kmem[cpuID].freelist;
  kmem[cpuID].freelist = r;
  release(&kmem[cpuID].lock);
}

// Allocate one 4096-byte page of physical memory.
// Returns a pointer that the kernel can use.
// Returns 0 if the memory cannot be allocated.
void *
kalloc(void)
{
  push_off();
  int cpuID = cpuid();
  pop_off();
  struct run *r;

  acquire(&kmem[cpuID].lock);
  r = kmem[cpuID].freelist;
  if (r)
    kmem[cpuID].freelist = r->next;
  else
  {
    for (int i = 0; i < NCPU; i++)
    {
      if (i != cpuID)
      {
        acquire(&kmem[i].lock);
        if (kmem[i].freelist)
        {
          r = kmem[i].freelist;
          kmem[i].freelist = kmem[i].freelist->next;
          release(&kmem[i].lock);
          break;
        }
        release(&kmem[i].lock);
      }
    }
  }
  release(&kmem[cpuID].lock);

  if (r)
    memset((char *)r, 5, PGSIZE); // fill with junk
  return (void *)r;
}
