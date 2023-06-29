import strutils, complex, algorithm, sugar
import os, json, tables, std/jsonutils, std/strformat

type
    SymDict = Table[string, string]
    SymList = seq[(string, string)]

proc gen_mdk(j: SymList; f: File) =
    f.writeLine("#<SYMDEFS># ARM Linker, 5.03 [Build 76]: Last Updated: Thu Oct 17 00:00:00 2019")
    for k in j:
        writeLine(f, fmt"{k[1]} T {k[0]}")
    return

proc gen_gcc(j: SymList; f: File) =
    for k in j:
        writeLine(f, fmt"{k[0]} = {k[1]};")
    return

proc gen_iar(j: SymList; f: File) =
    for k in j:
        f.writeLine(fmt"--define_symbol {k[0]}={k[1]}")
    return

proc gen_segger(j: SymList; f: File) =
    for k in j:
        f.writeLine(fmt"--defsym:{k[0]}={k[1]}")
    return

proc gen_symdefs(s: string) =
    var p: string
    p = splitPath(s)[0]
    var j = parseFile(s)
    var t = j.jsonTo(SymDict)
    var ks: seq[string] = @[]
    for k in t.keys:
        ks.add(k)
    ks.sort()

    var ll: SymList = @[]
    for k in ks:
        ll.add((k, t[k]))

    let all = @[joinPath(p, "symdefs.g"),
                joinPath(p, "symdefs.i"),
                joinPath(p, "symdefs.m"),
                joinPath(p, "symdefs.s")]
    let ap = @[gen_gcc, gen_iar, gen_mdk, gen_segger]
    for i in 0 ..< all.len:
        var f = open(all[i], fmWrite)
        defer: f.close()
        ap[i](ll, f)

const
    HEX_EXT_ADDR = 0x04u8
    HEX_DATA = 0x00u8
    HEX_EOF = 0x01u8

proc fmt_hex_line(loc: uint32, t: uint8, b: seq[byte]): string =
    result = newStringOfCap(100)
    result.add ":"
    var all: seq[byte] = @[cast[byte](len(b)),
                   cast[byte](loc shr 8),
                   cast[byte](loc and 0xffu8),
                   t]
    all.add(b)
    var sum: uint8 = 0
    for i in 0 ..< all.len:
        result.add fmt"{all[i]:02X}"
        sum += all[i]
    sum = not(sum) + 1
    result.add fmt"{sum:02X}"

proc fmt_hex_ext_addr(loc: uint32): string =
    var u1 = cast[uint8](loc shr 8)
    var u2 = cast[uint8](loc and 0xffu8)
    return fmt_hex_line(0x0u32, HEX_EXT_ADDR, @[u1, u2])

func partition[T](s: seq[T]; sublen: int): seq[seq[T]] =
    let cnt = (s.len + sublen - 1) div sublen
    result = newSeq[seq[T]](cnt)
    var first = 0
    for i in 0 ..< cnt:
      var last = first + sublen
      if last > s.len:
        last = s.len
      result[i] = newSeqOfCap[T](last - first)
      for g in first ..< last:
        result[i].add(s[g])
      first = last

proc dump_hex(f: File, b: seq[byte]; location: uint32) =
    var last_ext_addr: uint32 = 0xffffffffu32
    var loc = location
    var l = b.partition(16)
    for i in 0 ..< l.len:
        var t = l[i]
        var ext = loc shr 16
        if ext != last_ext_addr:
            f.writeLine(fmt_hex_ext_addr(ext))
            last_ext_addr = ext
        f.writeLine(fmt_hex_line(loc and 0xffffu32, HEX_DATA, t))
        loc += + 16

proc loadBin(f: string): seq[byte] =
    var s = readFile(f)
    result = @[]
    for c in s: result.add(cast[byte](c))

proc hex_xxxx(p, target: string, address: uint32, more: proc (f: File) {.closure.}) =
    var f = open(joinPath(p, target), fmWrite)
    defer: f.close()
    var bin = loadBin(joinPath(p, "platform.bin"))
    dump_hex(f, bin, address)
    more(f)
    f.writeLine(fmt_hex_line(0x00u32, HEX_EOF, @[]))

proc hex_918(p, target: string, more: proc (f: File) {.closure.}) =
    hex_xxxx(p, target, 0x4000u32, more)

proc hex_916(p, target: string, more: proc (f: File) {.closure.}) =
    hex_xxxx(p, target, 0x02002000u32, more)

proc hex_916(p: string) =
    hex_916(p, "platform.hex", (f: File) => (discard))

    hex_916(p, "platform_lock.hex", (f: File) => dump_hex(f, @[0xFFu8, 0xEF, 0xFF, 0xFF], 0x02000000u32))

proc hex_918(p: string) =
    hex_918(p, "platform.hex", (f: File) => (discard))

    let enty = @[0xFFu8, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
                 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x40, 0x00, 0x00]
    hex_918(p, "platform_entry.hex",
        (f: File) => dump_hex(f, enty, 0x087fec))

    let entylock = @[0xFFu8, 0xFF, 0xFF, 0xFF, 0x5a, 0x5a, 0x5a, 0x5a,
                 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x40, 0x00, 0x00]
    hex_918(p, "platform_entry_lock.hex",
        (f: File) => dump_hex(f, entylock, 0x087fec))

if paramCount() == 0:
    echo "usage: gen_files /path/to/sdk/bundles"
    quit(0)

var sdk = paramStr(1)
echo "searching files..."
var all_binaries: seq[string] = @[]
for x in walkDirRec(sdk):
    if x.endsWith("apis.json"):
        all_binaries.add(x)

echo "generating symdefs..."
for x in all_binaries:
    gen_symdefs x

echo "generating hex files..."
for x in all_binaries:
    var p = splitPath(x)[0]
    var series = toLower splitPath(p)[1]
    if series.startsWith("ing918"):
        hex_918(p)
    elif series.startsWith("ing916"):
        hex_916(p)
    else:
        debugEcho "unknown: ", series

