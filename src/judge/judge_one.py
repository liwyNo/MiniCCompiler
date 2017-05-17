#!/usr/bin/env python3

from os import system, path

std_parser = 'parser' # change this
std_regalloc = 'regalloc' # change this
std_torisc = 'torisc' # change this
std_testdir = '.'

def input_line(std):
    try:
        ans = input()
        return ans if len(ans) != 0 else std
    except EOFError:
        return std

parser = input_line(std_parser)
regalloc = input_line(std_regalloc)
torisc = input_line(std_torisc)
testdir = input_line(std_testdir)

linker = 'riscv-gcc' # change this
emulator = 'spike' # change this
judge_folder = 'judge' # change this

datalist = path.join(testdir, 'list.txt') # change this
with open(datalist, 'r') as flist:
    num = int(flist.readline())
    score = int(flist.readline())
    timeout = int(flist.readline())
    src = flist.readline().strip()

def run(cmd):
    global res, timeout
    if res != 0: return
    res = system('timeout %d %s' % (timeout, cmd))

system('mkdir -p %s' % judge_folder)

res = 0
for t in range(num):
    data_in = path.join(testdir, 'data%d.in' % t) # change this
    data_ans = path.join(testdir, 'data%d.ans' % t) # change this
    data_out = path.join(judge_folder, 'data%d.out' % t)
    tads = path.join(judge_folder, 'tads.txt')
    regmid = path.join(judge_folder, 'regmid.txt')
    risc = path.join(judge_folder, 'risc.txt')
    logfile = path.join(judge_folder, 'log%d.txt' % t)
    run('%s < %s > %s 2>/dev/null' % (parser, src, tads))
    run('%s < %s > %s 2>/dev/null' % (regalloc, tads, regmid))
    run('%s < %s > %s 2>/dev/null' % (torisc, regmid, risc))
    run('%s %s -o %s &>>%s' % (linker, risc, exe, logfile))
    run('%s %s < %s > %s 2>>%s' % (emulator, exe, data_in, data_out, logfile))
    run('diff %s %s &>/dev/null' % (data_out, data_ans))
    if res != 0:
        break

print(score if res != 0 else 0)
