#PRAGMA DWS
#DEFINE UNICODE                                  ' needed to messagebox only : to use wstring not string

'================================
' Options
'================================
'following option tu use ucs2 not true utf16 (limited unit codes <= &hFFFF) but normally faster with dw_ string functions
'#define ___USE___UCS2___ONLY___    		'change here to use UCS2___ONLY not true utf16 for windows ; commented : not ; uncommented valid option

'following option tu use always count surrogates pairs into the buffer , dw_ string functions will be normally faster with, when many surrogates
'#define ___STORE___SURROGATE___PAIRS___ 	'change here to use surrogate var ;  commented : not ; uncommented valid option

'#define _DWSTR_DEBUG_ 1 						'change here to display debugg info ; 0 or commented : not , 1 print
'================================

#ifndef _DWSTR_DEBUG_

   #Define N_STEPS 1000000                     'change steps only here > to verify the speed from 1 to 1000000 or more ...

#Else
   #Define N_STEPS 1                             'do not change here, to be sure only 1 step is done when debugg mode
#ENDIF

'================================


#INCLUDE ONCE "DWSTR.inc"                        'DWSTR class

#Include Once "crt/time.bi"                      'just to measure the speed


scope
dim dw1 as dwstr = dw_wstr("è_ç", CP_UTF8)
messagebox(0 , ">" & dw1  & "<    len = " & len(dw1) & "    dw_len = " & dw_len(dw1), "dw1" , 0)
print asc(dw1,1)
dim dw4 as dwstr = dw_string(23 , &h1D11E)
dim dw5 as dwstr
	messagebox(0 , ">" & dw4  & "<    len = " & len(dw4) & "    dw_len = " & dw_len(dw4), "dw4" , 0)
	'dw4 = Dw_Repeat(dw4, 3)
	'messagebox(0 , ">" & dw4  & "<    len = " & len(dw4) & "    dw_len = " & dw_len(dw4), "test Dw_repeat" , 0)
	'print  Dw_Asc(dw4,5) : print	 Dw_Asc(dw4,4)
	'dw4 = Dw_Asc(dw4 , 65, 2) : dw4 = Dw_Asc(dw4 , &h1D11E, 3): dw4 = Dw_Asc(dw4 , &h1D11E, 7): dw4 = Dw_Asc(dw4 , 65, 1) 'asc
	'dw5 = Dw_Asc(dw4 , &h1D11E, 12)
	dw4 = Dw_Asc(dw4 , 65, 5)
	messagebox(0 , ">" & dw4  & "<    len = " & len(dw4) & "    dw_len = " & dw_len(dw4), "after Dw_Asc" , 0)

end scope


print : print "Press key to finish !"
sleep	