'   Short test program demonstrating the wrong results of the
'   len() command in FreeBasic
'   -----------------------------------------------------------
'
#DEFINE unicode


'#define _DWSTR_DEBUG_ 1 'change here to display debugg info ; 0 or commented : not , 1 print
#define ___USE___SURROGATE___PAIRS___ 1 'change here to use surrogate var ; 0 or commented : not ; 1 use of surrogate var

#INCLUDE ONCE "DWSTR.inc"                        'DWSTR class
dim d1 as dwstr
'                                 ....+....1....+...
   dim germanstring  as string = "M‰rchenkˆnigsgr¸ﬂe" 'length=18
'                                 ....+....1....+..
   dim danishstring  as string = "R¯dgr¯d med fl¯de"  'length=17
'                                 ....+....1....+
   dim turkishstring as string = "Bir Áay l¸tfen!"    'length=15
'                                 ....+....1....+...
   dim frenchstring  as string = "AcadÈmie FranÁaise" 'length=18
   dim as integer germanlen, danishlen, turkishlen, frenchlen

    print "Test of international strings with FreeBasic"
    print "--------------------------------------------"
    germanlen  = len(germanstring)
    print "Output of German string:  " + germanstring
   print "Length of German string:  "; germanlen
	d1 = germanstring
	print: print " dwstr germansstring : " & d1 & "   len = " & len(d1) : print
   danishlen  = len(danishstring)
    print "Output of Danish string:  " + danishstring
   print "Length of Danish string:  "; danishlen
	d1 = danishstring
	print: print " dwstr danishstring : " & d1 & "   len = " & len(d1) : print
   turkishlen = len(turkishstring)
   print "Output of Turkish string: " + turkishstring
    print "Length of Turkish string: "; turkishlen
	d1 = turkishstring
	print: print " dwstr turkishstring : " & d1 & "   len = " & len(d1) : print
   frenchlen  = len(frenchstring)
    print "Output of French string:  " + frenchstring
    print "Length of French string:  "; frenchlen
	d1 = frenchstring
	print: print " dwstr frenchstring : " & d1 & "   len = " & len(d1) : print
	print: print len(d1) &  d1: print

	print "utf8 coded = " & strtoutf8(frenchstring)
	print "local 850 = " & utf8tostr(strtoutf8(frenchstring),850)
	print "utf16 coded = " & strtoutf16(frenchstring)

	dim as dwstr d2 = strtoutf16(frenchstring)
	dim as string s850 = utf16tostr(d2, 850)
	print "s850 = " & s850

	dim as dwstr dw2= dw_chr(&h1D11E)                         ' test input code > &h10000 to check surrogate pair creation
	s850 = utf16tostr(dw2, 850)
	print "s850 = " & s850
	print "utf8 = " & dw_str(dw2, CP_UTF8)
	print "str 1252 to 850 = " & StrToStr(frenchstring,1252,850)

	dim as dwstr d8 = Utf8ToWstr("–ë–Æ–Ø–§–ôê®ÄêÄÜê†ì") & wstr(!"Ä\u20AC") & wchr(&h20AC) & dw2
	messagebox 0, d8, "test utf8", 0
	messagebox 0, WstrToUtf8(d8), "back to utf8", 0
	dw_write_utf8_file("file_utf8.txt", d8)

	messagebox 0, dw_read_utf8_file("file_utf8.txt"), "read utf8 len = " & len(dw_read_utf8_file("file_utf8.txt")), 0
	'dim as dwstr dw8 = read_utf8_file("file_utf8.txt")

	if d2 = dw_read_utf8_file("file_utf8.txt") THEN
		print "equal"
	elseif d2 <> dw_read_utf8_file("file_utf8.txt") THEN
		print "different"
   END IF
print : print "Press key to finish !"
sleep


