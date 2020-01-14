import platform_api

{.push stack_trace: off, profiler:off.}

proc rawoutput(s: string) =
  when nimvm: discard
  else: platform_printf(s)

proc panic(s: string) {.noreturn.} =
  rawoutput("PANIC ")
  rawoutput(s)
  while true: discard

{.pop.}
