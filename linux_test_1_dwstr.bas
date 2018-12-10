'compile with console to view the information

' linux_test_1_dwstr.bas : to test under Linux the DWSTR (dynamic Wstring) class

'########################################################################
'this test code assume you are using a utf8 charset
'
'########################################################################


'================================
' Options
'================================

'#define _DWSTR_DEBUG_ 1 'change here to display debugg info ; 0 or commented : not , 1 print

'================================

#ifndef _DWSTR_DEBUG_

   #Define N_STEPS 10000                         '00 'change steps only here > to verify the speed from 1 to 1000000 or more ...

#Else
   #Define N_STEPS 1                             'do not change here, to be sure only 1 step is done when debugg mode
#ENDIF

'================================

#INCLUDE ONCE "DWSTR.inc"                        'DWSTR class

#Include Once "crt/time.bi"                      'just to measure the speed

declare sub show_info( byref dw1 as DWSTR , byref dw2 as DWSTR , byval i as long = 0)


print : print "testing  last  DWSTR.inc  "
print : print " SetLocale info = " & _LOCAL_VERIF_1234567890 : print : print

scope                                            'interesting to check the destructor action on debugg mode

   dim n as long = N_STEPS
   print "=======================================================" : print : print


   'dim mydw111 as dwstr = dw_string(60, 0)

	dim mydw111 as dwstr  = "test"
	print "after"
	print *mydw111
   print : print


   Show_Info("mydw111 = " & *mydw111 & wstr( "   Len ") & wstr(mydw111.m_BufferLen) , "capacity " & wstr(mydw111.capacity))
   
   'mydw111 = 4

   'mydw111 = dw_str( "test")
	

   print : print
   Show_Info(*mydw111 & wstr( "   Len ") & wstr(mydw111.m_BufferLen) , "capacity " & wstr(mydw111.capacity))
   dim messs as string = "abcdef"
   dim messs1 as string = mid(messs , 2)
   print messs1
   Mid(messs , 3 , 3) = "456"
   print messs & "    strptr " & strptr(messs) : print : print

   dim dw777 as dwstr = " ""VarPtr is correct !"" "
   print wstr(*dw777)
	print "dw777.get" & dw777.get : print

	dim as dwstr ptr pds1 = @dw777
	print "Good Varptr = " & @dw777 & " and  Strptr = " & strptr(dw777)
	print "pds1 = " & pds1 & " *pds1->m_pBuffer " & *pds1 -> m_pBuffer & "   " & @dw777
	print " Correct : different PTRs !" : print : print

   print "strptr : " & strptr(dw777) & " " & -dw777 ' to see the 2 possible forms
   'check conversion from non utf8 charset
   dw777 = Dw_Wstr(!"\&hCA\&hE0\&hEA\&hE8\&hE5-\&hF2\&hEE \&hEA\&hF0\&hE0\&hEA\&hEE\&hE7\&hFF\&hE1\&hF0\&hFB" , "ru_RU.iso-8859-5")

   print : print
   Show_Info(dw777 + wstr( "   Len ") + wstr(dw777.m_BufferLen) , "capacity " + wstr(dw777.capacity))
	dim as wstring ptr ws0 = strptr(dw777)
print : print
	print *ws0 : print mid(*ws0 , 4)
print : print
   dim dw778 as dwstr = mid(*dw777 , 4)
   Show_Info(dw778 & wstr( "   Len ") + wstr(dw778.m_BufferLen) , "capacity " + wstr(dw778.capacity))
   dim dw111 as dwstr = dw_string(248)
   Show_Info(dw111 & wstr( "   Len ") & wstr(dw111.m_BufferLen) , "capacity " & wstr(dw111.capacity))

   dim dw1 as dwstr = dw_string(23 , &h1D11E)    ' test code units > &h10000
   Show_Info(dw1 & wstr( "   Len ") & wstr(dw1.m_BufferLen) , "capacity " & wstr(dw1.capacity))
   dw1.replace( " it's a test of replacing text " , 20)
   Show_Info(dw1 & wstr( "   Len ") & wstr(dw1.m_BufferLen) , "capacity " & wstr(dw1.capacity))
   dim dw2 as dwstr = !"ABCD\u20ACFGH"           ' test unit code escape sequence \u20AC for euro symbol
   Show_Info(dw2 & wstr( "   Len ") & wstr(dw2.m_BufferLen) , "capacity " & wstr(dw2.capacity))
   dw2 = dw_asc(dw2 , 8364 , 3)                  ' test value code for euro
   Show_Info(dw2 & wstr( "   Len ") & wstr(dw2.m_BufferLen) , "capacity " & wstr(dw2.capacity))
   'dw2.char(3) = 8364
   'print "usc(dw2,3)= " & dw_asc(dw2,3)
   print "usc(*dw2,3)= " & asc(*dw2 , 3)
   Show_Info(dw2 & wstr( "   Len ") & wstr(dw2.m_BufferLen) , "capacity " & wstr(dw2.capacity))
   print "dw2[4]= " & dw2[4]
   Show_Info(dw2 & wstr( "   Len ") & wstr(dw2.m_BufferLen) , "capacity " & wstr(dw2.capacity))
   dim as string smess = dw_str(*dw2)
   Show_Info(smess , "test  dw_str")
   print smess & "  " & *strptr(smess)
   print "instr(2,dw2,!""\u20AC"") = " & instr(2 , *dw2 , !"\u20AC")
   dim as dwstr smess2 = !"coucou \u20AC"
   Show_Info(smess2 , "test  ")
   Show_Info(Dw_Str(smess2) , "Show_Info,  test  dw_Str")
   print smess2
   smess2 = ucase(*smess2)
   Show_Info(smess2 , "test  ucase   len=" & len(smess2) )
   print smess2


   ' change to your desired chars to test the conversion with bytes > 128 from your keyboard
   dw2 = dw_wstr("èçàé€","fr_FR.UTF-8" ) 'accent chars and euro
   Show_Info(dw2 , "test dw_wstr   len=" & len(dw2))
   print dw2
   dw2 = ucase(*dw2)
   Show_Info(dw2 , "test ucase")
   dw2 = lcase(*dw2)
   Show_Info(dw2 , "test lcase")
   print dw2
   dw2 = dw_chr(&h1D11E)                         ' test input code > &h10000
   ' how is it see on the console? it is possible to show with the console font?
   Show_Info(dw2 , "test dw_chr")
                                    
   dw2 = "abcdefgh"
   dw2.insert( "ABC" , 4)
   Show_Info(dw2 , "test dw2.insert")
   dw2.DelChars(5 , 20)
   Show_Info(dw2 , "test dw2.DelChars")


   print "initial string : " : print "Дми́трий Дми́триевич" 'uft8 coded string
   DIM bs2 AS dwstr = "Дми́трий Дми́триевич"
   Show_Info (bs2 , "test input UTF8")

   dim as string my_ss2 = dw_str(bs2)
   print "back to CP_UTF8 "
   print my_ss2 : print

   dw2.insert(my_ss2 , 4)
   Show_Info(dw2 , "test .insert utf8")
   dw2.add(my_ss2)
   Show_Info(dw2 , "test .add utf8")
   dim rval as double = 414.456789
   dw2 &= rval
   Show_Info(dw2 , "test & double")

   dw2 = str(rval)
   Show_Info(str(val(dw2)) , "test val")



   dim z1                as double
   dim z2                as double

   PRINT : PRINT
   print : print "Press key to continue !"
   sleep

   dim         as string st1
   dim as string sText = "verif : "
   dim as string sText2 = " -stop"


   dim         as DWSTR uws
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
   print : print "STRING reference +" : print st1 + "   time = " + str(z2 - z1) + " ms" : print

   z1 = clock()
   for x = 1 to n
      st1 = "Line " & n & ", Column " & n & ": " & sText '& sText2
   NEXT
   z2 = clock()
   print : print "STRING reference &" : print st1 + "   time = " + str(z2 - z1) + " ms" : print

   print "==========================================" : print
   z1 = clock()
   for x = 1 to n
      uws = "Line " + WSTR(n) + ", Column " + WSTR(n) + ": " + *uwsText '+ *uwsText2
   NEXT
   z2 = clock()
   print : print "DWSTR + de-referenced" : print uws + "   time = " + str(z2 - z1) + " ms" : print


   z1 = clock()
   for x = 1 to n
      uws = "Line " + WSTR(n) + ", Column " + WSTR(n) + ": " + uwsText '+ uwsText2
   NEXT
   z2 = clock()
   print : print "DWSTR + " : print uws + "   time = " + str(z2 - z1) + " ms" : print


   z1 = clock()
   for x = 1 to n
      uws = "Line " + WSTR(n) + ", Column " + WSTR(n) + ": " & *uwsText '& uwsText2
   NEXT
   z2 = clock()
   print : print "DWSTR &  new operator "
   print uws + "   time = " + str(z2 - z1) + " ms" : print : print

   print : print "Press key to continue !"
   sleep


   dim as string mys44 = "123456789abcdefghijklmnopqrstuvwyxzABCDEFGHIJKLMNOPQRSTUVWXYZ"
   dim         as string mysret

   print : print


   print "=========================================="
   print "   Comparaison  DWSTR Solutions : right left"
   print "==========================================" : print


   dim as DWSTR uws44 = "123456789abcdefghijklmnopqrstuvwyxzABCDEFGHIJKLMNOPQRSTUVWXYZ"
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
      uwsret = left(*uws44 , 32)
   NEXT
   z2 = clock()
   print : print "DWSTR left *" : print " uwsret  = >" + *uwsret + "<     time " + str(z2 - z1) + "  ms" : print


   z1 = clock()
   for x = 1 to n
      uwsret = left(uws44 , 32)
   NEXT
   z2 = clock()
   print : print "DWSTR new left : same or faster than de-referenced solution"
   print " uwsret  = >" + *uwsret + "<     time " + str(z2 - z1) + "  ms" : print


   z1 = clock()
   for x = 1 to n
      uwsret = right(*uws44 , 32)
   NEXT
   z2 = clock()
   print : print "DWSTR  right *    de-referenced solution"
   print " uwsret  = >" + *uwsret + "<     time " + str(z2 - z1) + "  ms" : print


   z1 = clock()
   for x = 1 to n
      uwsret = right(uws44 , 32)
   NEXT
   z2 = clock()
   print : print "DWSTR  new right : same or faster than de-referenced solution"
   print " uwsret  = >" + *uwsret + "<     time " + str(z2 - z1) + "  ms" : print : print

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
      uwsret = mid(*uws44 , 32 , 18)
   NEXT
   z2 = clock()
   print : print "DWSTR Mid *" : print " uwsret  = >" + *uwsret + "<     time " + str(z2 - z1) + "  ms" : print


   z1 = clock()
   for x = 1 to n
      uwsret = dW_mid(uws44 , 32 , 18)
   NEXT
   z2 = clock()
   print : print "DWSTR Dw_Mid : "
   print " uwsret  = >" + *uwsret + "<     time " + str(z2 - z1) + "  ms" : print


end scope

print : print "Press key to finish !"
sleep

sub mywait()

	Do
		Sleep 1, 1
	Loop Until Inkey <> ""
end sub


sub show_info(byref dw1 as DWSTR , byref dw2 as DWSTR , byval i2 as long )
   print "____________________________________________________________" : print "              " & *dw2
   print *dw1
   if i2 = 0 then print "______________________________________________  > key please" : print : mywait()
	if i2 <> 0 then print "____________________________________________________________" : print
END sub






