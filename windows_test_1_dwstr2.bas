'compile with console to view the information

' windows_test_1_dwstr.bas : to test under windows the DWSTR (dynamic Wstring) class

'########################################################################
'this test code assume you are using a system codepage : 1252
' the literal inputs are dependant of that codepage,
' except utf8 inputs which are codepage independant
'########################################################################
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


scope                                            'interesting to check the destructor action on debugg mode

   print : print "testing  last  DWSTR.inc  "

   dim n as long = N_STEPS
   print "=======================================================" : print : print


	dim wrien as wstring*5 = wstr("rien")

   dim mydw111 as dwstr = dw_string(20 , 65)
   print : print
	mydw111 &= wrien
	print : print
   messagebox(0 , mydw111 & wstr( "  dw.Len ") & wstr(Dw_Len(mydw111)) , "capacity " & wstr(mydw111.capacity) , 0)
   print
   mydw111 = 5
	messagebox(0 , mydw111 & wstr( "   dw.Len ") & wstr(Dw_Len(mydw111)) , "capacity " & wstr(mydw111.capacity) , 0)

   mydw111 = dw_str( "test")
   print : print
   messagebox(0 , mydw111 & wstr( "   dw.Len") & wstr(Dw_Len(mydw111)) , "capacity " & wstr(mydw111.capacity) , 0)


	dim messs as string = "abcdef"
   dim messs1 as string = mid(messs , 2)
   print messs1
   Mid(messs , 3 , 3) = "456"
   print messs & "    strptr " & strptr(messs) : print : print

   dim dw777 as dwstr = " ""VarPtr is correct !"" "
   print wstr(dw777)
	print "dw777.get" & dw777.get : print
	dim as dwstr ptr pds1 = @dw777
	print "Good Varptr = " & @dw777 & " and  Strptr = " & strptr(dw777)
	print "pds1 = " & pds1 & " *pds1->m_pBuffer " & *pds1 -> m_pBuffer & "   " & @dw777
	print " Correct : different PTRs !" : print : print

   print "strptr : " & strptr(dw777) & " " & -dw777 ' to see the 2 possible forms

   dw777 = Dw_Wstr( "Êàêèå-òî êðàêîçÿáðû" , 1251)
   print : print
   messagebox(0 , dw777 + wstr( "   dw.Len ") + wstr(Dw_Len(dw777)) , "capacity " + wstr(dw777.capacity) , 0)
   messagebox(0 , dw777 , "capacity " + wstr(dw777.capacity) , 0)
   dim dw778 as dwstr = mid(dw777 , 4)
   dim dw778_ as dwstr = mid(dw777 , 4)
   messagebox(0 , dw778 & wstr( "   Len ") + wstr(dw778.m_BufferLen) , "capacity " + wstr(dw778.capacity) , 0)
   messagebox(0 , dw778_ & wstr( "   Len ") + wstr(dw778_.m_BufferLen) , "capacity " + wstr(dw778_.capacity) , 0)
   dim dw111 as dwstr = dw_string(248 , 0)
   messagebox(0 , dw111 & wstr( "   Len ") & wstr(dw111.m_BufferLen) , "capacity " & wstr(dw111.capacity) , 0)
   dim dw1 as dwstr = dw_string(23 , &h1D11E)
   messagebox(0 , dw1 & wstr( "   dw_Len ") & wstr(Dw_Len(dw1)) , "test capacity " & wstr(dw1.capacity) , 0)
	#IFDEF ___WORK___SURROGATE___PAIRS___
		messagebox(0 , "m_SurPair = " & dw1.m_SurPair & "  nb of surrogate pair " , "len =" & len(dw1) & "  dw_len =" & Dw_Len(dw1), 0)
	#Endif
	messagebox(0 , dw1 & wstr( "   SurPair ") & wstr(dw1.Sur_Count) , "test capacity " & wstr(dw1.capacity) , 0)
   dw1.replace( "_it's a test of replacing text " , 21)
   messagebox(0 , dw1 & wstr( "   dw_Len ") & wstr(Dw_Len(dw1)) , "capacity " & wstr(dw1.capacity) , 0)
	messagebox(0 , "Sur_Count = " & dw1.Sur_Count & "  nb of surrogate pair " , "len =" & len(dw1) & "  dw_len =" & Dw_Len(dw1), 0)
	#IFDEF ___WORK___SURROGATE___PAIRS___
		messagebox(0 , "m_SurPair = " & dw1.m_SurPair & "  nb of surrogate pair " , "len =" & len(dw1) & "  dw_len =" & Dw_Len(dw1), 0)
	#Endif

	dim dw4 as dwstr = mid(dw1, 9, 4)
	messagebox(0 , ">" & dw4  & "<    len = " & len(dw4) & "    dw_len = " & dw_len(dw4), "test mid as wstring only" , 0)
	dw4 = dw_mid(dw1, 9 , 4)
	#IFDEF ___USE___UCS2___ONLY___
		messagebox(0 , ">" & dw4  & "<    len = " & len(dw4) & "    dw_len = " & dw_len(dw4), "test dw_mid      under ucs2" , 0)
	#ELSE
		messagebox(0 , ">" & dw4  & "<    len = " & len(dw4) & "    dw_len = " & dw_len(dw4) , "test dw_mid      under true utf16" , 0)
	#ENDIF
	dw4 = Utf8ToWstr(WstrToUtf8(dw4))
	messagebox(0 , ">" & dw4  & "<    len = " & len(dw4) & "    dw_len = " & dw_len(dw4), "test convert to-from utf8" , 0)
	dw4 = Dw_Repeat(dw4, 3)
	messagebox(0 , ">" & dw4  & "<    len = " & len(dw4) & "    dw_len = " & dw_len(dw4), "test Dw_repeat" , 0)
	print  Dw_Asc(dw4,5) : print	 Dw_Asc(dw4,4)
	'dw4 = Dw_Asc(dw4 , 65, 2) ': dw4 = Dw_Asc(dw4 , &h1D11E, 3): dw4 = Dw_Asc(dw4 , &h1D11E, 7): dw4 = Dw_Asc(dw4 , 65, 1) 'asc
	dw4 = Dw_Asc(dw4 , &h1D11E, 12)

	messagebox(0 , ">" & dw4  & "<    len = " & len(dw4) & "    dw_len = " & dw_len(dw4), "after Dw_repeat" , 0)
	dim l1 as long
	for l1 = 0 to len(dw4)
		print l1 & "  " & dw4[l1]
   NEXT
	print
	messagebox(0 , ">" & dw4  & "<    len = " & len(dw4) & "    dw_len = " & dw_len(dw4), "test Dw_asc" , 0)
	dw4 = Dw_Right(dw4 , 9)
	for l1 = 0 to len(dw4)
		print l1 & "  " & dw4[l1]
   NEXT
	print
	messagebox(0 , ">" & dw4  & "<    len = " & len(dw4) & "    dw_len = " & dw_len(dw4), "test Dw_Right" , 0)

	dw4 = Dw_left(dw4 , 6)
	for l1 = 0 to len(dw4)
		print l1 & "  " & dw4[l1]
   NEXT
	print
	messagebox(0 , ">" & *dw4  & "<    len = " & len(dw4) & "    dw_len = " & dw_len(dw4), "test Dw_left" , 0)


	messagebox(0 , ">" & dw4  & "<  " & dw_chr(&h1D11E) & "  position = " &  Dw_INSTR(5, dw4 ,dw_chr(&h1D11E)), "test Dw_instr" , 0)
	messagebox(0 , ">" & dw4  & "<  " & dw_chr(&h1D11E) & "  position = " &  Dw_INSTR(-2, dw4 ,dw_chr(&h1D11E)), "test Dw_instr" , 0)
	dw4 = Dw_Asc(dw4 , &hd801, 1)
	for l1 = 0 to len(dw4)
		print l1 & "  " & dw4[l1]
   NEXT
	messagebox(0 , ">" & dw4  & "<    len = " & len(dw4) & "    dw_len = " & dw_len(dw4), "test bad sur" , 0)
	dw_ValidUnit (dw4)
	messagebox(0 , ">" & dw4  & "<    len = " & len(dw4) & "    dw_len = " & dw_len(dw4), "test dw_ValidUnit" , 0)
	dw_CleanSur (dw4)
	messagebox(0 , ">" & dw4  & "<    len = " & len(dw4) & "    dw_len = " & dw_len(dw4), "test dw_CleanSur" , 0)
   dim dw2 as dwstr = !"ABCD\u20ACFGH" ' test unit code escape sequence \u20AC for euro symbol
   messagebox(0 , dw2 & wstr( "   Len ") & wstr(dw2.m_BufferLen) , "capacity " & wstr(dw2.capacity) , 0)
   dw2 = dw_asc(dw2 , 8364 , 3)
   messagebox(0 , dw2 & wstr( "   Len ") & wstr(dw2.m_BufferLen) , "capacity " & wstr(dw2.capacity) , 0)
   'dw2.char(3) = 8364
   'print "usc(dw2,3)= " & dw_asc(dw2,3)
   print "usc(*dw2,3)= " & asc(dw2 , 3)
   messagebox(0 , dw2 & wstr( "   Len ") & wstr(dw2.m_BufferLen) , "capacity " & wstr(dw2.capacity) , 0)
   print "dw2[4]= " & dw2[4]
   messagebox(0 , dw2 & wstr( "   Len ") & wstr(dw2.m_BufferLen) , "capacity " & wstr(dw2.capacity) , 0)
   dim as string smess = dw_str(*dw2)
   messagebox(0 , smess , "test  dw_str" , 0)
   print smess & "  " & *strptr(smess)
   print "instr(2,dw2,""€"") = " & instr(2 , dw2 , "€")
   dim as dwstr smess2 = "coucou €"
   messagebox(0 , smess2 , "test  " , 0)
   messagebox(0 , Dw_Str(smess2) , "messagebox,  test  dw_Str" , 0)
   print smess2
   smess2 = ucase(smess2)
   messagebox(0 , smess2 , "test  ucase" , 0)
   print smess2

   ' here if you are on codepage different than 1252, the resulting output is depending of the interpretation of you system codepage
   ' change to your desired chars to test the conversion with bytes > 128 from your keyboard
   dw2 = dw_wstr( "é a àçè € ")                  'accent chars from the windows 1252 codepage
   messagebox(0 , dw2 , "test dw_wstr" , 0)
   print dw2
   dw2 = ucase(dw2)
   messagebox(0 , dw2 , "test ucase" , 0)
   dw2 = lcase(dw2)
   messagebox(0 , dw2 , "test lcase" , 0)
   print dw2
   dw2 = dw_chr(&h1D11E)                         ' test input code > &h10000 to check surrogate pair creation
   ' on windows you will see a square box , because that unit code is not possible to show with normal font
   messagebox(0 , dw2 , "test dw_chr" , 0)
   print dw2                                     ' you will see ?? thats shows the pair is really done (surrogate pair) for that unit code
   messagebox(0 , "len = " & len(dw2) & "  even it is only 1 unit code " , "len is equal 2, because surrogate pair" , 0)
	#IFDEF ___WORK___SURROGATE___PAIRS___
		messagebox(0 , "m_SurPair = " & dw2.m_SurPair & "  nb of surrogate pair " , "test surrogate pair" , 0)
	#Endif
	messagebox(0 , "m_SurPair = " & dw2.sur_Count & "  nb of surrogate pair " , "len is equal 2, because surrogate pair" , 0)
   dw2 = "abcdefgh"
   dw2.insert( "ABC" , 4)
   messagebox(0 , dw2 , "test dw2.insert" , 0)
   dw2.DelChars(5 , 20)
   messagebox(0 , dw2 , "test dw2.DelChars" , 0)


   print "Ð”Ð¼Ð¸ÌÑ‚Ñ€Ð¸Ð¹ Ð”Ð¼Ð¸ÌÑ‚Ñ€Ð¸ÐµÐ²Ð¸Ñ‡" 'uft8 coded string
   DIM bs2 AS dwstr = dw_wstr( "Ð”Ð¼Ð¸ÌÑ‚Ñ€Ð¸Ð¹ Ð”Ð¼Ð¸ÌÑ‚Ñ€Ð¸ÐµÐ²Ð¸Ñ‡" , CP_UTF8)
   messagebox 0 , bs2 , "test dw_wstr CP_UTF8" , MB_OK

   dim as string my_ss2 = dw_str(bs2 , CP_UTF8)
   messagebox 0 , my_ss2 , "test dw_str CP_UTF8" , MB_OK
   print my_ss2

   dw2.insert(dw_wstr(my_ss2 , CP_UTF8) , 4)
   messagebox(0 , dw2 , "test .insert utf8" , 0)
   dw2.add(dw_wstr(my_ss2 , CP_UTF8))
   messagebox(0 , dw2 , "test .add utf8" , 0)
   dim rval as double = 414.456789
   dw2 &= rval
   messagebox(0 , dw2 , "test & double" , 0)

   dw2 = str(rval)
   messagebox(0 , str(val(dw2)) , "test val" , 0)


   dim z1                as double
   dim z2                as double

   PRINT : PRINT
   print : print "Press key to continue !"
   sleep

   dim         as string st1
   dim as string sText = "verif : "
   dim as string sText2 = " -stop"


   dim         as DWSTR uws=""
   dim         as DWSTR uws3
   dim as DWSTR uwsText = "verif : "
   dim as DWSTR uwsText2 = " -stop"
   dim x                 as long


   print : print
   print "=========================================="
   print "   Comparaison DWSTR Solutions : concatenation"
   print "==========================================" : print

   z1 = clock()
   for x = 1 to n
      st1 = "Line " + STR(n) + ", Column " + STR(n) + ": " + sText '+ sText2
   NEXT
   z2 = clock()
   print : print "STRING whith +" : print st1 + "   time = " + str(z2 - z1) + " ms" : print

   z1 = clock()
   for x = 1 to n
      st1 = "Line " & n & ", Column " & n & ": " & sText '& sText2
   NEXT
   z2 = clock()
   print : print "STRING whith &" : print st1 + "   time = " + str(z2 - z1) + " ms" : print

   print "==========================================" : print
   z1 = clock()
   for x = 1 to n
      uws = "Line " + WSTR(n) + ", Column " + WSTR(n) + ": " + *uwsText '+ *uwsText2
   NEXT
   z2 = clock()
   print : print "DWSTR + de-referenced" : print uws + "   time = " + str(z2 - z1) + " ms" : print

	uws=""
   z1 = clock()
   for x = 1 to n
      uws = "Line " + WSTR(n) + ", Column " + WSTR(n) + ": " + uwsText '+ uwsText2
   NEXT
   z2 = clock()
   print : print "DWSTR + not de-referenced" : print uws + "   time = " + str(z2 - z1) + " ms" : print

	uws=""
   z1 = clock()
   for x = 1 to n
      uws = "Line " + WSTR(n) + ", Column " + WSTR(n) + ": " & *uwsText '& uwsText2
   NEXT
   z2 = clock()
   print : print "DWSTR de-referenced  &  new operator "
   print uws + "   time = " + str(z2 - z1) + " ms" : print : print


   uws=""
   z1 = clock()
   for x = 1 to n
      uws = "Line " + WSTR(n) + ", Column " + WSTR(n) + ": " & uwsText '& uwsText2
   NEXT
   z2 = clock()
   print : print "DWSTR not de-referenced  &  new operator "
   print uws + "   time = " + str(z2 - z1) + " ms" : print : print

   uws=""
   z1 = clock()
   for x = 1 to n
      uws = "Line " + WSTR(n) + ", Column " + WSTR(n) + ": " & wstr(uwsText) '& uwsText2
   NEXT
   z2 = clock()
   print : print "DWSTR not de-referenced  &  new operator with wstr"
   print uws + "   time = " + str(z2 - z1) + " ms" : print : print

   print : print "Press key to continue !"
   sleep


   dim as string mys44 = "123456789abcdefghijklmnopqrstuvwyxz€ABCDEFGHIJKLMNOPQRSTUVWXYZ"
   dim         as string mysret

   print : print


   print "=========================================="
   print "   Comparaison  DWSTR Solutions : right left"
   print "==========================================" : print


   dim as DWSTR uws44 = "123456789abcdefghijklmnopqrstuvwyxz€ABCDEFGHIJKLMNOPQRSTUVWXYZ"
   dim         as DWSTR uwsret

   z1 = clock()
   for x = 1 to n
      mysret = right(mys44 , 32)
   NEXT
   z2 = clock()
   print : print "right  STRING ref " : print " mysret  = >" + mysret + "<     time " + str(z2 - z1) + "  ms" : print

   z1 = clock()
   for x = 1 to n
      mysret = left(mys44 , 32)
   NEXT
   z2 = clock()
   print : print "left  STRING ref " : print " mysret  = >" + mysret + "<     time " + str(z2 - z1) + "  ms" : print

   print "==========================================" : print
   z1 = clock()
   for x = 1 to n
      uwsret = left(uws44 , 32)
   NEXT
   z2 = clock()
   print : print "DWSTR left " : print " uwsret  = >" + uwsret + "<     time " + str(z2 - z1) + "  ms" : print


	z1 = clock()
   for x = 1 to n
      uwsret = dw_left(uws44 , 32)
   NEXT
   z2 = clock()
   print : print "DWSTR dw_left: "
   print " uwsret  = >" + uwsret + "<     time " + str(z2 - z1) + "  ms" : print

   z1 = clock()
   for x = 1 to n
      uwsret = right(uws44 , 32)
   NEXT
   z2 = clock()
   print : print "DWSTR  right "
   print " uwsret  = >" + uwsret + "<     time " + str(z2 - z1) + "  ms" : print



	z1 = clock()
   for x = 1 to n
      uwsret = dw_right(uws44 , 32)
   NEXT
   z2 = clock()
   print : print "DWSTR  dw_right : "
   print " uwsret  = >" + uwsret + "<     time " + str(z2 - z1) + "  ms" : print : print

   print : print "Press key to continue !"
   sleep

   print "=========================================="
   print "   Comparaison  DWSTR Solutions : mid "
   print "==========================================" : print

   z1 = clock()
   for x = 1 to n
      mysret = mid(mys44 , 32 , 18)
   NEXT
   z2 = clock()
   print : print "Mid  STRING ref " : print " mysret  = >" + mysret + "<     time " + str(z2 - z1) + "  ms" : print


   print "==========================================" : print
   z1 = clock()
   for x = 1 to n
      uwsret = mid(uws44 , 32 , 18)
   NEXT
   z2 = clock()
   print : print "DWSTR Mid " : print " uwsret  = >" + uwsret + "<     time " + str(z2 - z1) + "  ms" : print

	print "==========================================" : print


   z1 = clock()
   for x = 1 to n
      uwsret = dW_mid(uws44 , 32 , 18)
   NEXT
   z2 = clock()
   print : print "DWSTR Dw_Mid : " : print " uwsret  = >" + *uwsret + "<     time " + str(z2 - z1) + "  ms" : print

	print : print "Press key to continue !"
   sleep
   print "=========================================="
   print "   Comparaison  DWSTR Solutions : instr "
   print "==========================================" : print

	dim as long nret1
	z1 = clock()
   for x = 1 to n
      nret1 = instr(4, mys44 , "w" )
   NEXT
   z2 = clock()
   print : print "instr  STRING ref " : print " nret1  = >" & nret1 & "<     time " + str(z2 - z1) + "  ms" : print


   print "==========================================" : print

	z1 = clock()
   for x = 1 to n
      nret1 = instr(4, uws44 , "w" )
   NEXT
   z2 = clock()
   print : print "instr  DWSTR  " : print " nret1  = >" & nret1 & "<     time " + str(z2 - z1) + "  ms" : print




	z1 = clock()
   for x = 1 to n
      nret1 = dw_instr(4, uws44 , "w" )
   NEXT
   z2 = clock()
   print : print "instr  DWSTR dw_instr " : print " nret1  = >" & nret1 & "<     time " + str(z2 - z1) + "  ms" : print

end scope


print : print "Press key to finish !"
sleep







