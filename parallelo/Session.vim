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
badd +231 test/send.cpp
badd +335 src/simulation.cpp
badd +806 src/main.cpp
badd +1 ./
badd +1 include/simulation.h
badd +1 send
badd +27 Makefile
badd +291 term:///mnt/c/Users/Utente/Desktop/test_cpp/game/parallelo//474:/bin/bash
argglobal
%argdel
$argadd ./
edit test/send.cpp
argglobal
balt src/main.cpp
setlocal foldmethod=syntax
setlocal foldexpr=0
setlocal foldmarker={{{,}}}
setlocal foldignore=#
setlocal foldlevel=0
setlocal foldminlines=1
setlocal foldnestmax=20
setlocal foldenable
9
normal! zo
19
normal! zo
67
normal! zo
71
normal! zo
77
normal! zo
98
normal! zo
154
normal! zo
157
normal! zo
154
normal! zc
210
normal! zo
221
normal! zo
226
normal! zo
246
normal! zo
252
normal! zo
260
normal! zo
let s:l = 231 - ((80 * winheight(0) + 24) / 49)
if s:l < 1 | let s:l = 1 | endif
keepjumps exe s:l
normal! zt
keepjumps 231
normal! 029|
tabnext 1
if exists('s:wipebuf') && len(win_findbuf(s:wipebuf)) == 0 && getbufvar(s:wipebuf, '&buftype') isnot# 'terminal'
  silent exe 'bwipe ' . s:wipebuf
endif
unlet! s:wipebuf
set winheight=1 winwidth=20
let &shortmess = s:shortmess_save
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
