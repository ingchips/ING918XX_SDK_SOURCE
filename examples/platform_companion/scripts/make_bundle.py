import sys
import re
import os
import os.path
import json
import shutil

OUTPUT_DIR = '../generated'
PROJ_NAME = 'platform_companion'

STACK_BIN = f'{OUTPUT_DIR}/platform.bin'
map_fn = f'../listing/{PROJ_NAME}.map'

added_api = ['platform_rom_hotfix']

PAGE = 1024 * 4 # size of a sector
BIN_INFO_OFFSET = 0x000000CC
patch_ram_usage = None

if not os.path.exists(OUTPUT_DIR):
    os.mkdir(OUTPUT_DIR)

if os.path.exists(STACK_BIN):
    os.remove(STACK_BIN)

shutil.copyfile(f'../{PROJ_NAME}.bin', STACK_BIN)

def get_meta_from_map(fn):
    r = {}
    global BIN_INFO_OFFSET
    global PAGE, patch_ram_usage
    with open(fn, 'r') as f:
        for line in f:
            m = re.match(r".*LR_IROM1 \(Base: 0x([0-9a-fA-F]{8}), .+COMPRESSED\[0x([0-9a-fA-F]{8})\]", line)
            if not (m is None):
                r['rom'] = {"base" : int(m.group(1), 16), "size" : int(m.group(2), 16)}
                continue

            m = re.match(r".*LR_IROM1 \(Base: 0x([0-9a-fA-F]{8}), Size: 0x([0-9a-fA-F]{8})", line)
            if not (m is None):
                r['rom'] = {"base" : int(m.group(1), 16), "size" : int(m.group(2), 16)}
                continue

            x = False
            m = re.match(r".*RW_IRAM2 \(Base: 0x([0-9a-fA-F]{8}),.*Size: 0x([0-9a-fA-F]{8})", line)
            if m is None:
                m = re.match(r".*RW_IRAM2 \(Exec base: 0x([0-9a-fA-F]{8}),.*Size: 0x([0-9a-fA-F]{8})", line)
            if not (m is None):
                size =  int(m.group(2), 16)
                size = (size + 3) // 4 * 4
                patch_ram_usage = {"base" : int(m.group(1), 16), "size" : size}

    app_base = ((r['rom']['size'] + PAGE - 1) // PAGE) * PAGE + r['rom']['base']
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

app_base = meta['app']['base']
print(f'app base set to: 0x{app_base:x}')
with open(STACK_BIN, 'r+b') as f:
    f.seek(BIN_INFO_OFFSET + 4)
    f.write(struct.pack('I', app_base))
    f.seek(BIN_INFO_OFFSET)
    (major, minor, patch) = struct.unpack('<HBB', f.read(4))
    meta['version'] = [major, minor, patch]

print(f"version is {meta['version']}")

with open(f'{OUTPUT_DIR}/apis.json', 'w') as f:
    json.dump(merge_api(fmapping), f, indent=4, sort_keys=True)

with open(f'{OUTPUT_DIR}/meta.json', 'w') as f:
    json.dump(meta, f, indent=4, sort_keys=True)


