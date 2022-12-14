#ifndef __MESH_MEMORY_POOL_H
#define __MESH_MEMORY_POOL_H


typedef void * mesh_memory_pool_t;

// initialize memory pool with with given storage, block size and count
void   mesh_memory_pool_create(mesh_memory_pool_t *pool, void * storage, int count, int block_size);

// get free block from pool, @return NULL or pointer to block
void * mesh_memory_pool_get(mesh_memory_pool_t *pool);

// return previously reserved block to memory pool
void   mesh_memory_pool_free(mesh_memory_pool_t *pool, void * block);


#endif // __MESH_MEMORY_POOL_H
