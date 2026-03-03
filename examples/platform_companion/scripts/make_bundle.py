import sys
import re
import os
import os.path
import json
import shutil
import argparse

OUTPUT_DIR = '../generated'
PROJ_NAME = 'platform_companion'

STACK_BIN = f'{OUTPUT_DIR}/platform.bin'
map_fn = f'../listing/{PROJ_NAME}.map'

added_api = ['platform_rom_hotfix', 'platform_rom_hotfix_using_fpb',
            "adv_patch_set_interval",
            "adv_patch_set_addr",
            "adv_patch_enable",
            "adv_patch_set_adv_data",
            "adv_patch_set_rsp_data",
            "is_adv_set_advertising",
            "adv_get_last_terminated_adv_set",
]

PAGE = 1024 * 4 # size of a sector
BIN_INFO_OFFSET = 0x000000CC
patch_ram_usage = None
custom_app_addr = -1

def parse_args():
    global custom_app_addr
    parser = argparse.ArgumentParser()

    parser.add_argument(
        '--app_addr',
        type=lambda x: int(x, 0),
        default=-1,
        help='customize app address')

    FLAGS, unparsed = parser.parse_known_args()
    custom_app_addr = FLAGS.app_addr
    if custom_app_addr > 0:
        assert (custom_app_addr & (PAGE - 1)) == 0

parse_args()

if not os.path.exists(OUTPUT_DIR):
    os.mkdir(OUTPUT_DIR)

if os.path.exists(STACK_BIN):
    os.remove(STACK_BIN)

shutil.copyfile(f'../{PROJ_NAME}.bin', STACK_BIN)

symbols_addr = {}
symbols_size = {}
SYMS_TO_READ = set(['__Vectors', '__PATCH_ADD', '__PLATFORM_VER', '__ALL_END', '__APP_LOAD_ADD', 'platform_patches'])

def get_meta_from_map(fn):
    r = {}
    global PAGE, patch_ram_usage, custom_app_addr
    with open(fn, 'r') as f:
        for line in f:
            m = re.match(r"^\s+([a-zA-Z_][a-zA-Z0-9_]*)\s+(0x[0-9a-fA-F]{8})\s+Data\s+([0-9]+)", line)
            if (m is not None) and (m.group(1) in SYMS_TO_READ):
                symbols_addr[m.group(1)] = int(m.group(2), 0)
                symbols_size[m.group(1)] = int(m.group(3))

            m = re.match(r".*LR_IROM1 \(Base: 0x([0-9a-fA-F]{8}), .+COMPRESSED\[0x([0-9a-fA-F]{8})\]", line)
            if m is not None:
                r['rom'] = {"base" : int(m.group(1), 16), "size" : int(m.group(2), 16)}
                continue

            m = re.match(r".*LR_IROM1 \(Base: 0x([0-9a-fA-F]{8}), Size: 0x([0-9a-fA-F]{8})", line)
            if m is not None:
                r['rom'] = {"base" : int(m.group(1), 16), "size" : int(m.group(2), 16)}
                continue

            x = False
            m = re.match(r".*RW_IRAM2 \(Base: 0x([0-9a-fA-F]{8}),.*Size: 0x([0-9a-fA-F]{8})", line)
            if m is None:
                m = re.match(r".*RW_IRAM2 \(Exec base: 0x([0-9a-fA-F]{8}),.*Size: 0x([0-9a-fA-F]{8})", line)
            if m is not None:
                size =  int(m.group(2), 16)
                size = (size + 3) // 4 * 4
                patch_ram_usage = {"base" : int(m.group(1), 16), "size" : size}

    app_base = ((r['rom']['size'] + PAGE - 1) // PAGE) * PAGE + r['rom']['base']
    if custom_app_addr > 0: app_base = custom_app_addr
    r['app'] = {"base": app_base}
    return r

def parse_map(fn):
    r = {}
    with open(fn, 'r') as f:
        for line in f:
            m = re.match(r"^    ([a-zA-Z_][a-zA-Z0-9_]*)\s+(0x[0-9a-fA-F]{8})", line)
            if m == None:
                continue
            r[m.group(1)] = m.group(2)
    return r

def merge_api(fmapping):
    global added_api
    with open('apis.json') as f:
        rom_apis = json.load(f)
    r = {}
    for m in rom_apis:
        r[m] = rom_apis[m]
        if (m in fmapping) and (fmapping[m] != r[m]):
            print(f"overriding API: {m}")
            r[m] = fmapping[m]

    for m in added_api:
        print(f"    adding API: {m}")
        r[m] = fmapping[m]
    return r

fmapping = parse_map(map_fn)
meta = get_meta_from_map(map_fn)
meta['var'] = {}

with open('meta.json') as f:
    meta0 = json.load(f)
    meta['ram'] = meta0['ram']
    if patch_ram_usage is not None:
        size = patch_ram_usage['size'] + patch_ram_usage['base'] - meta['ram']['base']
        size = ((size + 3) // 4) * 4
        print(f"{size - meta['ram']['size']} bytes of RAM reserved")
        meta['ram']['size'] = size

    vars = meta0['var']
    for k in vars.keys():
        v: dict = vars[k]
        v.pop('addr', None)
        meta['var'][k] = v

import struct

bin_base = symbols_addr['__Vectors']
assert symbols_size['platform_patches'] <= 32 * 4

app_base = meta['app']['base']
print(f'app  base set to     : 0x{app_base:x} (app entry address)')
print(f"patches moved to     : 0x{symbols_addr['__ALL_END']:x}")
with open(STACK_BIN, 'r+b') as f:
    f.seek(symbols_addr['__PLATFORM_VER'] - bin_base)
    (major, minor, patch) = struct.unpack('<HBB', f.read(4))
    meta['version'] = [major, minor, patch]

    f.seek(symbols_addr['platform_patches'] - bin_base)
    patches = f.read(symbols_size['platform_patches'])


    f.seek(symbols_addr['__ALL_END'] - bin_base)
    f.write(patches)
    app_base_for_platform = f.tell() + bin_base
    prog    = struct.pack("<IIHHHHI", 0x0, app_base_for_platform + 9, 0x4801, 0x6800, 0x4700, 0x0000, app_base + 4)
    f.write(prog)
    f.seek(symbols_addr['__PATCH_ADD'] - bin_base)
    f.write(struct.pack('<I', symbols_addr['__ALL_END']))

    f.seek(symbols_addr['__APP_LOAD_ADD'] - bin_base)
    f.write(struct.pack('<I', app_base_for_platform))

print(f'app addr for platform: 0x{app_base_for_platform:x}')
print(f"version is {meta['version']}")

with open(f'{OUTPUT_DIR}/apis.json', 'w') as f:
    json.dump(merge_api(fmapping), f, indent=4, sort_keys=True)

with open(f'{OUTPUT_DIR}/meta.json', 'w') as f:
    json.dump(meta, f, indent=4, sort_keys=True)


