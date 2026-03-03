/**
 * @brief A demo of customized backend for kv storage
 *
 * Features:
 *
 * 1. Flash written occurs on the level of a k-v pair
 * 1. Dual Flash block are used for secure operation
 *    Note: block size = `EFLASH_ERASABLE_SIZE`
 * 1. value length must be > 0.
*/

#include <string.h>
#include <errno.h>
#include "eflash.h"
#include "kv_storage.h"
#include "platform_api.h"
#include "port_gen_os_driver.h"

#if (INGCHIPS_FAMILY == INGCHIPS_FAMILY_918)
#define DB_FLASH_ADDRESS  0x42000
#elif (INGCHIPS_FAMILY == INGCHIPS_FAMILY_916)
#define DB_FLASH_ADDRESS  0x203C000
#elif (INGCHIPS_FAMILY == INGCHIPS_FAMILY_20)
#define DB_FLASH_ADDRESS  0x203C000
#else
#error unknown or unsupported chip family
#endif

#define BLOCK_SIZE              (EFLASH_ERASABLE_SIZE)

#define DB_FLASH_ADDR_END		(DB_FLASH_ADDRESS + EFLASH_ERASABLE_SIZE)

#define DB_FLASH_ADDRESS_BACKUP	(DB_FLASH_ADDRESS + EFLASH_ERASABLE_SIZE)

// maximum number k-v can be stored
#ifndef KV_CACHE_SIZE
#define KV_CACHE_SIZE		50
#endif

#define GEN_OS          ((const gen_os_driver_t *)platform_get_gen_os_driver())

#pragma pack (push, 1)
struct kv_item
{
    uint8_t len;		// length of data (<= KV_VALUE_MAX_LEN)
    kvkey_t key;
    uint8_t data[0];
};
#pragma pack (pop)

struct kv_cache_item
{
    kvkey_t         key;
    uint8_t         len;
    void           *data;
};

struct kv_cache_item  kv_cache[KV_CACHE_SIZE] = {0};

static uint32_t kv_storage_tail = 0;

static uint32_t next_item(const struct kv_item *item)
{
    uint32_t s = (sizeof(struct kv_item) + item->len + 3) & ~0x3;

    return (uint32_t)item + s;
}

static int kv_flash_repair(uint32_t start, uint32_t end)
{
    struct kv_item *item = (struct kv_item *)start;
    while ((start < end) && (item->len < 255))
    {
        uint32_t next = next_item(item);
        if (next >= end)
            break;
        start = next;
        item = (struct kv_item *)start;
    }

    kv_storage_tail = start;

    while (start < end)
    {
        uint32_t *p = (uint32_t *)start;
        if (*p != 0xffffffff)
            return 1;
        start += 4;
    }
    return 0;
}

static void kv_do_backup(uint32_t from, uint32_t to)
{
#if (INGCHIPS_FAMILY == INGCHIPS_FAMILY_918)
        program_flash(to, (uint8_t *)from, BLOCK_SIZE);
#elif (INGCHIPS_FAMILY == INGCHIPS_FAMILY_916)
        int i;
        uint32_t buf[16];
        erase_flash_sector(to);

        for (i = 0; i < BLOCK_SIZE / sizeof(buf); i++)
        {
            memcpy(buf, (void *)from, sizeof(buf));
            write_flash(to, (uint8_t *)buf, sizeof(buf));
            from += sizeof(buf);
            to += sizeof(buf);
        }
#elif (INGCHIPS_FAMILY == INGCHIPS_FAMILY_20)
        int i;
        uint32_t buf[16];
        erase_flash_sector(to);

        for (i = 0; i < BLOCK_SIZE / sizeof(buf); i++)
        {
            memcpy(buf, (void *)from, sizeof(buf));
            write_flash(to, (uint8_t *)buf, sizeof(buf));
            from += sizeof(buf);
            to += sizeof(buf);
        }
#endif
}

static void kv_reset(void)
{
#if (INGCHIPS_FAMILY == INGCHIPS_FAMILY_918)
    erase_flash_page(DB_FLASH_ADDRESS);
#elif (INGCHIPS_FAMILY == INGCHIPS_FAMILY_916)
    erase_flash_sector(DB_FLASH_ADDRESS);
#elif (INGCHIPS_FAMILY == INGCHIPS_FAMILY_20)
    erase_flash_sector(DB_FLASH_ADDRESS);
#endif
    kv_storage_tail = DB_FLASH_ADDRESS;
}

static void impl_kv_remove_all(void)
{
    int i;
    for (i = 0; i < KV_CACHE_SIZE; i++)
    {
        if (kv_cache[i].data)
        {
            kv_cache[i].key = (kvkey_t)-1;
            GEN_OS->free(kv_cache[i].data);
            kv_cache[i].data = NULL;
        }
    }
    kv_reset();
}

static struct kv_item *kv_search_flash(const kvkey_t key, uint32_t start, uint32_t end)
{
    struct kv_item *found = NULL;
    struct kv_item *item = (struct kv_item *)start;
    while (item->len < 255)
    {
        uint32_t next = next_item(item);
        if (next > end)
            break;

        if (item->key == key)
        {
            found = item;
        }

        start = next;
        item = (struct kv_item *)start;
    }

    if (found)
        return found->len > 0 ? found : NULL;
    else
        return NULL;
}

static struct kv_cache_item *kv_search_cache(const kvkey_t key)
{
    int i;
    for (i = 0; i < KV_CACHE_SIZE; i++)
    {
        if ((kv_cache[i].key == key) && (kv_cache[i].data))
        {
            return kv_cache + i;
        }
    }
    return NULL;
}

static void impl_do_kv_visit(f_kv_visitor visitor, void *user_data, uint32_t start, uint32_t end)
{
    int k;
    for (k = 0; k <= KV_USER_KEY_END; k++)
    {
        struct kv_cache_item * r = kv_search_cache(k);
        if (r)
        {
            if (visitor(r->key, r->data, r->len, user_data) != KV_OK)
                break;
        }
        else
        {
            struct kv_item *item = kv_search_flash(k, start, end);
            if (item)
                if (visitor(item->key, item->data, item->len, user_data) != KV_OK)
                    break;
        }
    }
}

static void impl_kv_visit(f_kv_visitor visitor, void *user_data)
{
    impl_do_kv_visit(visitor, user_data, DB_FLASH_ADDRESS, DB_FLASH_ADDR_END);
}

static struct kv_cache_item *kv_search(const kvkey_t key)
{
    int i;
    struct kv_item *item;
    struct kv_cache_item * r = kv_search_cache(key);
    if (r) return r;

    item = kv_search_flash(key, DB_FLASH_ADDRESS, DB_FLASH_ADDR_END);

    if (item && item->len > 0)
    {
        for (i = 0; i < KV_CACHE_SIZE; i++)
        {
            if (kv_cache[i].data == NULL)
                break;
        }
        if (i >= KV_CACHE_SIZE)
        {
            // cache can't store so many items
            return NULL;
        }

        kv_cache[i].data = GEN_OS->malloc(item->len);
        kv_cache[i].key = key;
        kv_cache[i].len = item->len;
        memcpy(kv_cache[i].data, item->data, item->len);

        return kv_cache + i;
    }

    return NULL;
}

static int kv_do_append_key(kvkey_t key, const void *data, int len);

static int kv_visitor_dump(const kvkey_t key, const uint8_t *data, const int16_t len, void *user_data)
{
    kvkey_t k = (kvkey_t)(uintptr_t)user_data;
    if ((k != key) && (len > 0))
       kv_do_append_key(key, data, len);
    return KV_OK;
}

static void kv_do_gc(kvkey_t except_key)
{
    kv_do_backup(DB_FLASH_ADDRESS, DB_FLASH_ADDRESS_BACKUP);
    kv_reset();
    impl_do_kv_visit(kv_visitor_dump, (void *)(uintptr_t)except_key, DB_FLASH_ADDRESS_BACKUP, DB_FLASH_ADDRESS_BACKUP + BLOCK_SIZE);
}

static int kv_do_append_key(kvkey_t key, const void *data, int len)
{
    int aligned = (2 + len + 3) & ~0x3ul;
    const uint8_t *d = (const uint8_t *)data;
    uint8_t t[4] = {len, key, 0, 0};

    if(d != NULL && len > 0)
        memcpy(t + 2, d, (len > 2 ? 2 : len));

    if (kv_storage_tail + aligned > DB_FLASH_ADDR_END)
        kv_do_gc(key);
    if (kv_storage_tail + aligned > DB_FLASH_ADDR_END)
        return KV_ERR_OUT_OF_MEM;

    write_flash(kv_storage_tail, t, sizeof(t));
    kv_storage_tail += sizeof(t);

    aligned -= 4;
    if (aligned > 0)
    {
        write_flash(kv_storage_tail, d + 2, aligned);
        kv_storage_tail += aligned;
    }
    return KV_OK;
}

static void kv_do_remove_key(const kvkey_t key)
{
    kv_do_append_key(key, NULL, 0);
}

static void impl_kv_remove(const kvkey_t key)
{
    struct kv_cache_item * r = kv_search_cache(key);
    if (r)
    {
        GEN_OS->free(r->data);
        r->data = 0;
        r->key = (kvkey_t)-1;
        kv_do_remove_key(key);
        return;
    }

    struct kv_item *item = kv_search_flash(key, DB_FLASH_ADDRESS, DB_FLASH_ADDR_END);
    if (item)
        kv_do_remove_key(key);
}

static int impl_kv_put(const kvkey_t key, const uint8_t *data, int16_t len)
{
    struct kv_cache_item *item = kv_search(key);
    if (item)
    {
        GEN_OS->free(item->data);
        item->data = NULL;
        item->key = (kvkey_t)-1;
    }

    return kv_do_append_key(key, data, len);
}

static uint8_t *impl_kv_get(const kvkey_t key, int16_t *len)
{
    struct kv_cache_item *item = kv_search(key);
    if (item)
    {
        if (len) *len = item->len;
        return item->data;
    }
    return NULL;
}

static void impl_kv_value_modified_of_key(kvkey_t key)
{
    struct kv_cache_item * r = kv_search_cache(key);
    if (r)
        kv_do_append_key(key, r->data, r->len);
}

static const kv_backend_t kv_backend =
{
    .kv_remove_all  = impl_kv_remove_all,
    .kv_remove      = impl_kv_remove,
    .kv_get         = impl_kv_get,
    .kv_put         = impl_kv_put,
    .kv_visit       = impl_kv_visit,
    .kv_value_modified_of_key  = impl_kv_value_modified_of_key,
};

void kv_impl_init(void)
{
    int r = kv_flash_repair(DB_FLASH_ADDRESS, DB_FLASH_ADDR_END);
    if (r)
    {
        r = kv_flash_repair(DB_FLASH_ADDRESS_BACKUP, DB_FLASH_ADDRESS_BACKUP + BLOCK_SIZE);
        if (r == 0)
        {
            kv_do_backup(DB_FLASH_ADDRESS_BACKUP, DB_FLASH_ADDRESS);
            kv_flash_repair(DB_FLASH_ADDRESS, DB_FLASH_ADDR_END);
        }
        else
            kv_reset();
    }

    kv_init_backend(&kv_backend);
}
