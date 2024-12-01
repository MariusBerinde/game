let SessionLoad = 1
let s:so_save = &g:so | let s:siso_save = &g:siso | setg so=0 siso=0 | setl so=-1 siso=-1
let v:this_session=expand("<sfile>:p")
silent only
silent tabonly
cd /mnt/c/Users/Utente/Desktop/test_cpp/game/parallelo
if expand('%') == '' && !&modified && line('$') <= 1 && getline(1) == ''
  let s:wipebuf = bufnr('%')
endif
let s:shortmess_save = &shortmess
if &shortmess =~ 'A'
  set shortmess=aoOA
else
  set shortmess=aoO
endif
badd +232 test/send.cpp
badd +335 src/simulation.cpp
badd +806 src/main.cpp
badd +1 include/simulation.h
badd +27 Makefile
badd +1 ../README.md
argglobal
%argdel
$argadd ./
edit test/send.cpp
let s:save_splitbelow = &splitbelow
let s:save_splitright = &splitright
set splitbelow splitright
wincmd _ | wincmd |
split
1wincmd k
wincmd w
let &splitbelow = s:save_splitbelow
let &splitright = s:save_splitright
wincmd t
let s:save_winminheight = &winminheight
let s:save_winminwidth = &winminwidth
set winminheight=0
set winheight=1
set winminwidth=0
set winwidth=1
exe '1resize ' . ((&lines * 33 + 26) / 52)
exe '2resize ' . ((&lines * 15 + 26) / 52)
argglobal
balt Makefile
setlocal foldmethod=syntax
setlocal foldexpr=0
setlocal foldmarker={{{,}}}
setlocal foldignore=#
setlocal foldlevel=0
setlocal foldminlines=1
setlocal foldnestmax=20
setlocal foldenable
7
normal! zo
19
normal! zo
65
normal! zo
70
normal! zo
76
normal! zo
97
normal! zo
154
normal! zo
154
normal! zc
212
normal! zo
223
normal! zo
228
normal! zo
let s:l = 232 - ((16 * winheight(0) + 16) / 33)
if s:l < 1 | let s:l = 1 | endif
keepjumps exe s:l
normal! zt
keepjumps 232
normal! 0
wincmd w
argglobal
if bufexists(fnamemodify("term:///mnt/c/Users/Utente/Desktop/test_cpp/game/parallelo//71:/bin/bash", ":p")) | buffer term:///mnt/c/Users/Utente/Desktop/test_cpp/game/parallelo//71:/bin/bash | else | edit term:///mnt/c/Users/Utente/Desktop/test_cpp/game/parallelo//71:/bin/bash | endif
if &buftype ==# 'terminal'
  silent file term:///mnt/c/Users/Utente/Desktop/test_cpp/game/parallelo//71:/bin/bash
endif
balt test/send.cpp
setlocal foldmethod=manual
setlocal foldexpr=0
setlocal foldmarker={{{,}}}
setlocal foldignore=#
setlocal foldlevel=0
setlocal foldminlines=1
setlocal foldnestmax=20
setlocal foldenable
let s:l = 39 - ((0 * winheight(0) + 7) / 15)
if s:l < 1 | let s:l = 1 | endif
keepjumps exe s:l
normal! zt
keepjumps 39
normal! 064|
wincmd w
2wincmd w
exe '1resize ' . ((&lines * 33 + 26) / 52)
exe '2resize ' . ((&lines * 15 + 26) / 52)
tabnext 1
if exists('s:wipebuf') && len(win_findbuf(s:wipebuf)) == 0 && getbufvar(s:wipebuf, '&buftype') isnot# 'terminal'
  silent exe 'bwipe ' . s:wipebuf
endif
unlet! s:wipebuf
set winheight=1 winwidth=20
let &shortmess = s:shortmess_save
let &winminheight = s:save_winminheight
let &winminwidth = s:save_winminwidth
let s:sx = expand("<sfile>:p:r")."x.vim"
if filereadable(s:sx)
  exe "source " . fnameescape(s:sx)
endif
let &g:so = s:so_save | let &g:siso = s:siso_save
set hlsearch
nohlsearch
doautoall SessionLoadPost
unlet SessionLoad
" vim: set ft=vim :
