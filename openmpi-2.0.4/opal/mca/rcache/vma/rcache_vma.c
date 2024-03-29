/* -*- Mode: C; c-basic-offset:4 ; indent-tabs-mode:nil -*- */
/*
 * Copyright (c) 2004-2005 The Trustees of Indiana University and Indiana
 *                         University Research and Technology
 *                         Corporation.  All rights reserved.
 * Copyright (c) 2004-2007 The University of Tennessee and The University
 *                         of Tennessee Research Foundation.  All rights
 *                         reserved.
 * Copyright (c) 2004-2005 High Performance Computing Center Stuttgart,
 *                         University of Stuttgart.  All rights reserved.
 * Copyright (c) 2004-2005 The Regents of the University of California.
 *                         All rights reserved.
 * Copyright (c) 2006      Voltaire. All rights reserved.
 * Copyright (c) 2009-2013 Cisco Systems, Inc.  All rights reserved.
 * Copyright (c) 2009      IBM Corporation.  All rights reserved.
 * Copyright (c) 2013      NVIDIA Corporation.  All rights reserved.
 * Copyright (c) 2015-2017 Los Alamos National Security, LLC. All rights
 *                         reserved.
 *
 * $COPYRIGHT$
 *
 * Additional copyrights may follow
 *
 * $HEADER$
 */

#include "opal_config.h"

#include MCA_memory_IMPLEMENTATION_HEADER
#include "opal/mca/memory/memory.h"
#include "opal/mca/rcache/rcache.h"
#include "rcache_vma.h"
#include "rcache_vma_tree.h"
#include "opal/mca/mpool/base/base.h"

/**
 * Initialize the rcache
 */

void mca_rcache_vma_module_init( mca_rcache_vma_module_t* rcache ) {
    if (!mca_rcache_vma_tree_items_inited) {
        opal_free_list_init (&mca_rcache_vma_tree_items, sizeof (mca_rcache_vma_t),
                             8, OBJ_CLASS(mca_rcache_vma_t), 0, 8,
                             mca_rcache_vma_tree_items_min, mca_rcache_vma_tree_items_max,
                             mca_rcache_vma_tree_items_inc, NULL, 0, NULL, NULL, NULL);
        mca_rcache_vma_tree_items_inited = true;
    }

    rcache->base.rcache_find = mca_rcache_vma_find;
    rcache->base.rcache_find_all = mca_rcache_vma_find_all;
    rcache->base.rcache_insert = mca_rcache_vma_insert;
    rcache->base.rcache_delete = mca_rcache_vma_delete;
    rcache->base.rcache_finalize = mca_rcache_vma_finalize;
    rcache->base.rcache_dump_range = mca_rcache_vma_dump_range;
    rcache->base.rcache_iterate = mca_rcache_vma_iterate;
    OBJ_CONSTRUCT(&rcache->base.lock, opal_recursive_mutex_t);
    mca_rcache_vma_tree_init(rcache);
}

int mca_rcache_vma_find(struct mca_rcache_base_module_t* rcache,
        void* addr, size_t size, mca_mpool_base_registration_t **reg)
{
    int rc;
    unsigned char* bound_addr;

    if(size == 0) {
        return OPAL_ERROR;
    }

    bound_addr = ((unsigned char *)addr) + size - 1;

    /* Check to ensure that the cache is valid */
    if (OPAL_UNLIKELY(opal_memory_changed() &&
                      NULL != opal_memory->memoryc_process &&
                      OPAL_SUCCESS != (rc = opal_memory->memoryc_process()))) {
        return rc;
    }

    *reg = mca_rcache_vma_tree_find((mca_rcache_vma_module_t*)rcache, (unsigned char*)addr,
            bound_addr);

    return OPAL_SUCCESS;
}

int mca_rcache_vma_find_all(struct mca_rcache_base_module_t* rcache,
        void* addr, size_t size, mca_mpool_base_registration_t **regs,
        int reg_cnt)
{
    int rc;
    unsigned char *bound_addr;

    if(size == 0) {
        return OPAL_ERROR;
    }

    bound_addr = ((unsigned char *)addr) + size - 1;

    /* Check to ensure that the cache is valid */
    if (OPAL_UNLIKELY(opal_memory_changed() &&
                      NULL != opal_memory->memoryc_process &&
                      OPAL_SUCCESS != (rc = opal_memory->memoryc_process()))) {
        return rc;
    }

    return mca_rcache_vma_tree_find_all((mca_rcache_vma_module_t*)rcache,
            (unsigned char*)addr, bound_addr, regs,
            reg_cnt);
}

int mca_rcache_vma_insert(struct mca_rcache_base_module_t* rcache,
        mca_mpool_base_registration_t* reg, size_t limit)
{
    int rc;
    size_t reg_size = reg->bound - reg->base + 1;
    mca_rcache_vma_module_t *vma_rcache = (mca_rcache_vma_module_t*)rcache;

    if(limit != 0 && reg_size > limit) {
        /* return out of resources if request is bigger than cache size
         * return temp out of resources otherwise */
        return OPAL_ERR_OUT_OF_RESOURCE;
    }

    /* Check to ensure that the cache is valid */
    if (OPAL_UNLIKELY(opal_memory_changed() &&
                      NULL != opal_memory->memoryc_process &&
                      OPAL_SUCCESS != (rc = opal_memory->memoryc_process()))) {
        return rc;
    }

    rc = mca_rcache_vma_tree_insert(vma_rcache, reg, limit);
    if (OPAL_LIKELY(OPAL_SUCCESS == rc)) {
        /* If we successfully registered, then tell the memory manager
           to start monitoring this region */
        opal_memory->memoryc_register(reg->base,
                                      (uint64_t) reg_size, (uint64_t) (uintptr_t) reg);
    }

    return rc;
}

int mca_rcache_vma_delete(struct mca_rcache_base_module_t* rcache,
        mca_mpool_base_registration_t* reg)
{
    mca_rcache_vma_module_t *vma_rcache = (mca_rcache_vma_module_t*)rcache;
    /* Tell the memory manager that we no longer care about this
       region */
    opal_memory->memoryc_deregister(reg->base,
                                    (uint64_t) (reg->bound - reg->base),
                                    (uint64_t) (uintptr_t) reg);
    return mca_rcache_vma_tree_delete(vma_rcache, reg);
}

int mca_rcache_vma_iterate (struct mca_rcache_base_module_t* rcache,
                            unsigned char *base, size_t size,
                            int (*callback_fn) (mca_mpool_base_registration_t *, void *),
                            void *ctx)
{
    mca_rcache_vma_module_t *vma_rcache = (mca_rcache_vma_module_t*)rcache;
    return mca_rcache_vma_tree_iterate (vma_rcache, base, size, callback_fn, ctx);
}

/**
  * finalize
  */
void mca_rcache_vma_finalize(struct mca_rcache_base_module_t* rcache)
{
    OBJ_DESTRUCT(&rcache->lock);
    mca_rcache_vma_tree_finalize((mca_rcache_vma_module_t *)rcache);
    free(rcache);
}

void mca_rcache_vma_dump_range(struct mca_rcache_base_module_t* rcache,
                               unsigned char *base, size_t size, char *msg)
{
    mca_rcache_vma_module_t *vma_rcache = (struct mca_rcache_vma_module_t*) rcache;
    mca_rcache_vma_tree_dump_range(vma_rcache, base, size, msg);
}
