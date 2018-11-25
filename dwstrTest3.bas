
'#define unicode
'#include Once "windows.bi"
'#DEFINE _DWSTR_DEBUG_ 1
#include once "dwstr.inc"                      ' for dwstr type
#include once "Afx/cwstr.inc"                      ' for dwstr type

#Include Once "crt/time.bi"                      ' just to measure the time needed

using Afx

scope
'***********************************************************************************
dim n as long = 1000000                           ' number of iterations, change here
'***********************************************************************************

dim i                 as long
dim z1                as double
dim z2                as double

? "test speed concat for  " & n & " steps" : ?

dim         as String sret1
dim as String tst = "Paul Squires"
z1 = clock()
for i = 1 to n
   sret1 &= tst & i & "a"
NEXT
z2 = clock()

? "done as string concat" : ?
? " sret1 (last 50)  = " & mid(sret1 , len(sret1) - 50)
? "len = " & len(sret1) & " chars "
? "execution time : " & z2 - z1 & "ms" : ? : ? : ?



dim as dwstr uret1
dim as dwstr utst
utst = "Paul Squires"
uret1=""
z1 = clock()
for i = 1 to n
   uret1 &= *utst & i & "a"
NEXT
z2 = clock()

? "done as dwstr concat" : ?
? " uret1 (last 50)  = " & mid(*uret1 , len(uret1) - 50)
? "len = " & len(uret1) & " wchars "
? "execution time : " & z2 - z1 & "ms" : ? : ? : ?


uret1 = ""
utst = "Paul Squires"
z1 = clock()
for i = 1 to n
   uret1 &= *utst & wstr(i) & wstr("a")
NEXT
z2 = clock()

? "done as dwstr concat" : ?
? " uret1 (last 50)  = " & mid(*uret1 , len(uret1) - 50)
? "len = " & len(uret1) & " wchars "
? "execution time : " & z2 - z1 & "ms" : ? : ? : ?

dim as cwstr cret1
dim as cwstr ctst

cret1 = ""
ctst = "Paul Squires"
z1 = clock()
for i = 1 to n
   cret1 &= **ctst & wstr(i) & wstr("a")
NEXT
z2 = clock()

? "done as cwstr concat" : ?
? " cret1 (last 50)  = " & mid(**cret1 , len(cret1) - 50)
? "len = " & len(cret1) & " wchars "
? "execution time : " & z2 - z1 & "ms" : ? : ? : ?

? : ? : ?


end scope


? "press any key to finish ..."

sleep

