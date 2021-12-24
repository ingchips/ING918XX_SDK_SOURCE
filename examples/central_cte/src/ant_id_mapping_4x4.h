#ifndef _ant_id_mapping_4x4_h
#define _ant_id_mapping_4x4_h

#include <stdint.h>

#define     ANT_ID_0_IN_GROUP       0
#define     ANT_ID_1_IN_GROUP       2
#define     ANT_ID_2_IN_GROUP       1
#define     ANT_ID_3_IN_GROUP       3

#define     ANT_GROUP_0             (0 << 2)
#define     ANT_GROUP_1             (2 << 2)
#define     ANT_GROUP_2             (1 << 2)
#define     ANT_GROUP_3             (3 << 2)

#define     _MAKE_A_GROUP(G)    ((G) | ANT_ID_0_IN_GROUP),((G) | ANT_ID_1_IN_GROUP),((G) | ANT_ID_2_IN_GROUP),((G) | ANT_ID_3_IN_GROUP)

#define     ANT_ID_MAPPING      _MAKE_A_GROUP(ANT_GROUP_0), _MAKE_A_GROUP(ANT_GROUP_1), _MAKE_A_GROUP(ANT_GROUP_2), _MAKE_A_GROUP(ANT_GROUP_3)

static const uint8_t ant_id_mapping_table[] = { ANT_ID_MAPPING };

static const uint8_t *switch_pattern_mapping(int len, const uint8_t *pattern)
{
    return pattern;
    static uint8_t mapped[100];
    int i;
    for (i = 0; i < len; i++)
        mapped[i] = ant_id_mapping_table[pattern[i]];
    return mapped;
}

#endif
