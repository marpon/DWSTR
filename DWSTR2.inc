/'   DWSTR2.inc  release 1.4.3 : 05 dec 2018  by Marc Pons

 Topics - Behavior explanations

 VarPtr(dwstr)	: gives the ptr of dwstr   (as dwstr ptr)  	'as normal
 @dwstr			:  same as above but simplified way				'as normal

 -dwstr  		: gives the pointer of the internal buffer 		( as wstring ptr  ) use the '-' overloaded operator line 1138
 strptr (dwstr): same as above, to be similar as with string

 *dwstr			: dereferences the internal pointer to wstring	( byref as wstring)
						' to ease the typing dwstr.get or even dwstr.m_pBuffer

 & operators : to concatenate directly dwstr with wstring (both sides) or other dwstr or double

 wstring functions : Must use *cwstr form !!!!  to insure the native wstring functions work always correctly
		e.g.   Trim(*DWSTR1)     or  Ltrim (*DWSTR1, " ,;")    or  InStr(4, *DWSTR1, Any *DWSTR2)

		for simplicity some functions have been overloaded :  RIGHT, LEFT, VAL

 but all the native functions using wstring will work , you only have to put * before the DWSTR var
		remark , sometime it is not needed because the implicit cast can work correctly (some not)
		but it is a good habit to always use the *dwstr form when using native wstring functions with dwstr!!!

more specific to windows: because of surrogate pair needed to count positions in unit code not wchar, not really needed in Linux!
	specific functions : 		Dw_Asc , Dw_Chr , Dw_String  (counterpart to Asc, Chr, String )
										Dw_Right , Dw_Left , Dw_Mid , Dw_Instr , Dw_Len (evident counterpart functions )
	even more specific : 		Dw_CleanSur , Dw_LenSur , Dw_ValidUnit
because of the possible surrogate pair, these functions, make extra job to verify, position correctly in unit codes,
so the speed is affected ( in average 2 times slower), but if you have surrogate and want to work with correct	functions
no choice, you have to use them !

An alternative to speed up the treatment, if very few case with surrogate pairs, you can avoid them completely,
all unit code > &hFFFF will be replaced by '~'. Use the optionnal #define ___USE___UCS2___ONLY___

An alternative to speed up a little , if you have very frequently surrogate, you can use another option
#define ___STORE___SURROGATE___PAIRS___ , the surrogate will be counted an stored, so it will accelerate a little the treatment.
Choose that option only if you have a lot of surrogate , in average it is a bit faster , but if you have only few,
better to let without these options, in any case these options have no effect at all on Linux, the function will work
as their conterparts function (wstring ones) without dw_ prefix.

conversion from / to    wstring / string
	Str or Wstr  will work  e.g.    Str(*cwstr)   ;  wstr(*dwstr)  ! again notice with *

	but 2 new extended functions , with optionnal codepage param to set the codepage/charset  (see list: charset.txt)
		Dw_Str( dwstr1, codepage)  converts dwstr to string;    string will be coded according the defined codepage
		Dw_Wstr( str1, codepage)	converts string to dwstr;    string supposed coded according the defined codepage

## in Windows mode, codepage is a numeric var ( e.g 1252 for cp-1252  , CP_UTF8  when utf8 )
	it is assumed the default charset is not utf8
		Dw_Wstr extended Wstr function to convert to DWSTR, can use a codepage or CP_UTF8 to convert the string input
		e.g.   Dw_Wstr (" ‡ÍËÂ-ÚÓ Í‡ÍÓÁˇ·˚", 1251)   or    dw_wstr("–î–º–∏ÃÅ—Ç—Ä–∏–π –î–º–∏ÃÅ—Ç—Ä–∏–µ–≤–∏—á", CP_UTF8)

		Dw_Str  extended Str function to convert to string, can use a codepage or CP_UTF8 to convert the DWSTR input
		e.g.   Dw_Str (DWSTR1, 1251)     or  Dw_Str (DWSTR1, CP_UTF8)

## in Linux mode,  codepage is a string var  ( e.g  "de_DE.UTF-8"  or for Locale "fr_FR.iso-8859-1")
	it is assumed the default charset is always utf8 coded , if not the program will stop!
		Dw_Wstr extended Wstr function to convert to DWSTR, can use "codepage" to convert the string input
		e.g.   Dw_Wstr (" ‡ÍËÂ-ÚÓ Í‡ÍÓÁˇ·˚", " xxx.yyyy " )   or    dw_wstr("–î–º–∏ÃÅ—Ç—Ä–∏–π –î–º–∏ÃÅ—Ç—Ä–∏–µ–≤–∏—á", "xxx.UTF-8")

		Dw_Str  extended Str function to convert to string, can use a "codepage" to convert the DWSTR input
		e.g.   Dw_Str (DWSTR1, " xxx.yyyy ")     or  Dw_Str (DWSTR1, "xxx.UTF-8")
=========================================================================================
'/


' DWSTR class : Dynamic Wstring class

' ########################################################################################
' Can be used with Microsoft Windows or Linux
' Implements a dynamic data type null terminated WSTRING, to simplify the use of unicode,
' intended to give the same behavior as dynamic STRING gives for ansi.
' Compiler: Free Basic 32 & 64 bit
' This code is an evolution of the original CWSTR class from JosÈ Roca , the structure is similar,
' is has been simplified is some aspects and extended in others features to "mimic" as much
' as possible the STRING type (dynamic).
' Regarding the CWSTR class, they are not interchageable! But can be easily "casted".
' Copyright (c) 2016 Paul Squires & JosÈ Roca & Marc Pons. Freeware. Use at your own risk.
' THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, EITHER
' EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
' MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
' ########################################################################################

#pragma ONCE

#INCLUDE ONCE "crt/string.bi"                    ' for memcopy
#INCLUDE ONCE "utf_conv.bi"                      ' for utf conversions : utf8 string to DWSTR and reverse


#IF (SizeOf(WSTRING) = 4)
	#IFNDEF __AS__LINUX__
		#DEFINE __AS__LINUX__
	#ENDIF
   #INCLUDE ONCE "crt/stdlib.bi"
   #DEFINE _MY_SIZE_WSTRING_ 4                   ' sizeof(wstring)
   TYPE _MY_W_CHAR_ AS ULONG                     ' equivalent type
   #INCLUDE ONCE "crt/locale.bi"                 ' is it already included in LINUX ? not sure: if yes can be deleted
   Dim SHARED as string _LOCAL_VERIF_1234567890 = *SetLocale(LC_CTYPE , 0) ' it is needed, probably SetLocale is already done?
   if left(UCASE(_LOCAL_VERIF_1234567890), 10) = "LC_COLLATE" THEN _LOCAL_VERIF_1234567890 = *SetLocale(LC_CTYPE , "") 'init local if not done before
	#IFDEF _DWSTR_DEBUG_
      print : print " SetLocale info = " & _LOCAL_VERIF_1234567890 : print : print
   #ENDIF
	if instr(UCASE(_LOCAL_VERIF_1234567890), "UTF-8") < 1 THEN 'to insure the locale default is utf8
		Print : Print "Information : from " & __FILE__ & "  line " & __LINE__
		Print : Print "   Sorry, the program will stop! The Locale setting must be changed before executing again. "
		Print :"   Please set the default Locale to any UTF-8 charset. If not set, the program will not be able to execute."
		End
   END IF
#ELSE
   #IFDEF __FB_DOS__
      TYPE DWSTR AS STRING
		#Print ======>
		#Print ======> Warning, Compiled under DOS : DWSTR type is only an Alias for String!
		#Print ======>
   #ELSE
      #INCLUDE ONCE "windows.bi"                 ' needed if not included before
      #DEFINE _MY_SIZE_WSTRING_ 2                ' sizeof(wstring)
      TYPE _MY_W_CHAR_ AS USHORT                 ' equivalent type
      #define IS_HIGH_UTF16_SUR(wch) (((wch) >= &hD800) andalso ((wch) <= &hDBFF))
      #define IS_LOW_UTF16_SUR(wch) (((wch) >= &hDC00) andalso ((wch) <= &hDFFF))
   #ENDIF
	#IFDEF __AS__LINUX__
		#UNDEF __AS__LINUX__
	#ENDIF
#ENDIF


#IFNDEF __FB_DOS__
	' ========================================================================================
	' Macro for debug
	' To allow debugging, define _DWSTR_DEBUG_ 1 or more, in your application before including this file.
	' ========================================================================================
	#IFNDEF _DWSTR_DEBUG_
		#DEFINE _DWSTR_DEBUG_ 0
	#ENDIF
	#IFNDEF _DWSTR_DP_
		#DEFINE _DWSTR_DP_ 1
		#MACRO DWSTR_DP(st)
			#IF(_DWSTR_DEBUG_ > 0)
				print(st)
			#ENDIF
		#ENDMACRO
	#ENDIF
	#DEFINE IS_FULL_VALID_RANGE(wch) (((wch) <> &hFFFF) and ((wch) <> &hFFFE) and ((wch) < &h110000) and (((wch) < &hD800) or ((wch) > &hDFFF)))

	#DEFINE IS_LIMITED_VALID_RANGE(wch) (((wch) <> &hFFFF) and ((wch) <> &hFFFE) and ((wch) < &h10000) and (((wch) < &hD800) or ((wch) > &hDFFF)))

	#DEFINE _MY_TRAP_NUMBER_ 222 'to insure good input via ubyte constructor, better not change or always < 255
	' ========================================================================================
	' In windows mode to use UCS2 ONLY not full unicode utf16 , define ___USE___UCS2___ONLY___ ,
	' in your application before including this file.
	' Warning : you have to be sure you will never use unit code >FFFF
	' ========================================================================================

	' ========================================================================================
	' In windows mode, to use SURROGATE PAIRS process a bit faster , define ___STORE___SURROGATE___PAIRS___ ,
	' in your application before including this file.
	' Warning : it will slow down some processes, use that option only if you have very numerous unit code >h&FFFF
	' ========================================================================================
	#IFNDEF __AS__LINUX__
		#IFDEF ___USE___UCS2___ONLY___                       'more priority to that option
			#IFDEF ___STORE___SURROGATE___PAIRS___
				#UNDEF ___STORE___SURROGATE___PAIRS___				'this one
			#ENDIF
		#ENDIF
	#ELSE
		#IFDEF ___USE___UCS2___ONLY___
			#UNDEF ___USE___UCS2___ONLY___
		#ENDIF
		#IFDEF ___STORE___SURROGATE___PAIRS___
			#UNDEF ___STORE___SURROGATE___PAIRS___
		#ENDIF
	#ENDIF



	' ########################################################################################
	' *** DWSTR CLASS ***
	' ########################################################################################
	TYPE DWSTR
		Public:
			m_pBuffer     AS wstring PTR = NULL ' Pointer to the buffer
			m_BufferLen   AS LONG = 0   			' Length in Wchars of the current string in the buffer
			m_SurPair     AS LONG = 0   			' nb of surrogates in wstring chain
			DECLARE CONSTRUCTOR
			DECLARE CONSTRUCTOR(BYVAL nCapacity AS Ulong) ' can create specific "fixed" sized buffer (can be lower than the initial 260)
			DECLARE CONSTRUCTOR(BYREF ansiStr AS STRING)
			DECLARE CONSTRUCTOR(BYVAL pwszStr AS WSTRING PTR)
			DECLARE CONSTRUCTOR(BYREF ust AS DWSTR)
			DECLARE CONSTRUCTOR(BYREF pub AS UBYTE PTR , BYVAL len1 AS LONG , BYVAL size1 AS LONG = 0, BYVAL nSur AS LONG = 0) 'special interresting for speed
			DECLARE DESTRUCTOR
			DECLARE FUNCTION Sur_Count(BYVAL nIndex AS LONG = 1, BYVAL nEnd AS LONG = &h7FFFFFFE) AS LONG
			DECLARE FUNCTION Sur_CountRev(BYVAL nIndex AS LONG = 1, BYVAL nEnd AS LONG = &h7FFFFFFE) AS LONG
			DECLARE SUB ResizeBuffer(BYVAL nValue AS LONG)
			DECLARE PROPERTY Capacity() AS LONG
			DECLARE PROPERTY Capacity(BYVAL nValue AS LONG)
			DECLARE SUB Clear
			DECLARE SUB Add(BYREF ust AS DWSTR)
			DECLARE SUB Add(BYVAL pwszStr AS WSTRING PTR , BYVAL nLen AS LONG = -1, BYVAL nNumSur AS LONG = -1)
			DECLARE SUB Add(BYREF ansiStr AS STRING)
			DECLARE PROPERTY Char(BYVAL nIndex AS LONG) AS _MY_W_CHAR_
			DECLARE PROPERTY Char(BYVAL nIndex AS LONG , BYVAL nValue AS _MY_W_CHAR_)
			DECLARE OPERATOR[](BYVAL nIndex AS LONG) AS _MY_W_CHAR_
			DECLARE FUNCTION DelChars(BYVAL nIndex AS LONG , BYVAL nCount AS LONG) AS LONG
			DECLARE FUNCTION Insert(BYREF ansiStr AS STRING , BYVAL nIndex AS LONG) AS LONG
			DECLARE FUNCTION Insert(BYVAL pwszStr AS WSTRING PTR , BYVAL nIndex AS LONG) AS LONG
			DECLARE FUNCTION Insert(BYREF ust AS DWSTR , BYVAL nIndex AS LONG) AS LONG
			DECLARE FUNCTION Replace(BYREF ansiStr AS STRING , BYVAL nIndex AS LONG) AS LONG
			DECLARE FUNCTION Replace(BYVAL pwszStr AS WSTRING PTR , BYVAL nIndex AS LONG) AS LONG
			DECLARE FUNCTION Replace(BYREF ust AS DWSTR , BYVAL nIndex AS LONG) AS LONG
			DECLARE FUNCTION Get() BYREF AS WSTRING
			DECLARE OPERATOR CAST() BYREF AS WSTRING
			DECLARE OPERATOR CAST() AS ANY PTR
			DECLARE OPERATOR LET(BYREF ansiStr AS STRING)
			DECLARE OPERATOR LET(BYREF wszStr AS CONST WSTRING)
			DECLARE OPERATOR LET(BYREF pwszStr AS WSTRING PTR)
			DECLARE OPERATOR LET(BYREF ust AS DWSTR)
			DECLARE OPERATOR += (BYREF wszStr AS WSTRING)
			DECLARE OPERATOR += (BYREF ust AS DWSTR)
			DECLARE OPERATOR += (BYREF ansiStr AS STRING)
			DECLARE OPERATOR &= (BYREF wszStr AS WSTRING)
			DECLARE OPERATOR &= (BYREF ust AS DWSTR)
			DECLARE OPERATOR &= (BYREF ansiStr AS STRING)
			DECLARE OPERATOR &= (BYVAL num AS DOUBLE)
		Private:
			m_Capacity   AS LONG = 0   ' The total size of the buffer
			m_GrowSize   AS LONG = 260 ' How much to grow the buffer by when required ! can be reduced but better to let as it is for speed
			'*****  set to 1 only via constructor ubyte ptr ... when size < 0 and check = _MY_TRAP_NUMBER_
			m_Flag       AS LONG = 0   ' **** flag to avoid multiple alloc/copy of the same pointer and insure not deallocated prematurely

			DECLARE SUB WriteBuffer(BYVAL addrMemory AS WSTRING PTR, BYVAL nNumWchars AS LONG, BYVAL nNumSur AS LONG = -1)
			DECLARE SUB AppendBuffer(BYVAL addrMemory AS WSTRING PTR, BYVAL nNumWchars AS LONG, BYVAL nNumSur AS LONG = -1)
			DECLARE SUB InsertBuffer(BYVAL addrMemory AS WSTRING PTR, BYVAL nIndex AS LONG, BYVAL nNumWchars AS LONG, BYVAL nNumSur AS LONG = -1)
			DECLARE SUB ReplaceBuffer(BYVAL addrMemory AS WSTRING PTR, BYVAL nIndex AS LONG, BYVAL nNumWchars AS LONG)
	END TYPE
	' ########################################################################################

	' ========================================================================================
	DECLARE FUNCTION Dw_LenSur(BYVAL pwszStr AS WSTRING PTR, BYREF Sur as LONG) AS LONG

	#IFDEF __AS__LINUX__
		DECLARE FUNCTION Dwstr_To_Str2(BYREF DWsrc AS DWSTR , BYREF codepage AS STRING) AS STRING
		DECLARE FUNCTION MbsToWcs_2(BYREF code AS STRING , BYVAL dest AS WSTRING PTR , BYVAL src AS ZSTRING PTR , BYVAL nb AS LONG) AS LONG
	#ENDIF
	' ========================================================================================
	' DWSTR constructors
	' ========================================================================================
	PRIVATE CONSTRUCTOR DWSTR
		DWSTR_DP( "+++BEGIN- DWSTR CONSTRUCTOR Default")
		this.ResizeBuffer(m_GrowSize)              ' Create the initial buffer
		DWSTR_DP( "END DWSTR CONSTRUCTOR Default - " & WSTR(m_pBuffer))
	END CONSTRUCTOR
	' ========================================================================================
	' ========================================================================================
	PRIVATE CONSTRUCTOR DWSTR(BYVAL nCapacity AS Ulong)
		DWSTR_DP( "+++BEGIN- DWSTR CONSTRUCTOR Capacity: " & WSTR(nCapacity))
		if nCapacity = 0 then nCapacity = 32          ' to insure a minimal buffer
		this.ResizeBuffer(nCapacity)                  ' Create the nCapacity buffer
		m_Flag = 1                                    ' to not be double allocated/deallocated
		DWSTR_DP( "-END- DWSTR CONSTRUCTOR Capacity - " & WSTR(m_pBuffer))
	END CONSTRUCTOR
	' ========================================================================================
	' ========================================================================================
	PRIVATE CONSTRUCTOR DWSTR(BYVAL pwszStr AS WSTRING PTR)
		DWSTR_DP( "+++BEGIN- DWSTR CONSTRUCTOR WSTRING PTR - " & WSTR(pwszStr))
		if pwszStr <> NULL then
			this.Add(pwszStr)                             ' Add the passed WSTRING
			DWSTR_DP( "-END- DWSTR CONSTRUCTOR WSTRING PTR - " & WSTR(m_pBuffer))
		else
			this.ResizeBuffer(m_GrowSize)                 ' Create the initial buffer
			DWSTR_DP( "-END- DWSTR CONSTRUCTOR WSTRING PTR, Default: " & WSTR(m_pBuffer))
		end if
	END CONSTRUCTOR
	' ========================================================================================
	' ========================================================================================
	PRIVATE CONSTRUCTOR DWSTR(BYREF ansiStr AS STRING)
		DWSTR_DP( "+++BEGIN- DWSTR CONSTRUCTOR STRING - " & WSTR(STRPTR(ansiStr)))
		this.Add(ansiStr)                                ' Add the passed ansi string
		DWSTR_DP( "-END- DWSTR CONSTRUCTOR STRING - " & WSTR(m_pBuffer))
	END CONSTRUCTOR
	' ========================================================================================

	' ========================================================================================
	PRIVATE CONSTRUCTOR DWSTR(BYREF ust AS DWSTR)
		DWSTR_DP( "+++BEGIN- DWSTR CONSTRUCTOR DWSTR - " & WSTR(ust.m_pBuffer))
		if ust.m_Flag = 0 then
			this.Add(ust.m_pBuffer , ust.m_BufferLen)     ' Add the passed DWSTR
		else
			this.m_BufferLen = ust.m_BufferLen
			this.m_pBuffer = ust.m_pBuffer
			this.m_Capacity = ust.m_Capacity
			#IFDEF ___STORE___SURROGATE___PAIRS___
				this.m_SurPair = ust.m_SurPair
			#ENDIF
		end if
		DWSTR_DP( "-END- DWSTR CONSTRUCTOR DWSTR - " & WSTR(m_pBuffer))
	END CONSTRUCTOR
	' ========================================================================================
	' ========================================================================================
	PRIVATE CONSTRUCTOR DWSTR(BYREF pub AS UBYTE PTR , BYVAL len1 AS LONG , BYVAL size1 AS LONG = 0, BYVAL nSur AS LONG = 0) '****
		DWSTR_DP( "+++BEGIN- DWSTR CONSTRUCTOR UBYTE - " & WSTR(pub))
		IF pub = 0 or len1 = 0 THEN
			this.ResizeBuffer(m_GrowSize)
			DWSTR_DP( "    - DWSTR CONSTRUCTOR UBYTE - INIT ONLY : " & WSTR(this.m_pBuffer))
		elseif size1 = -1  and pub[(len1 + 1) *_MY_SIZE_WSTRING_] = _MY_TRAP_NUMBER_ then '**** trap if bad option
			this.m_BufferLen = len1
			this.m_pBuffer = cptr(wstring ptr , pub)
			this.m_Capacity = len1
			#IFDEF ___STORE___SURROGATE___PAIRS___
				if nSur = -1 then
					this.m_SurPair = Sur_Count()
				else
					this.m_SurPair = nSur
				end if
			#ENDIF
			this.m_Flag = 1                               'to avoid deallocating the buffer on destructor and avoid double copy too in LET step
			DWSTR_DP( "    - DWSTR CONSTRUCTOR UBYTE - ASSIGN ONLY  to " & WSTR(this.m_pBuffer))
		ELSE
			if size1 = 0 THEN
				size1 = len1 + 1
			else
				if len1 + 1 > size1 THEN size1 = len1 + 1
			END IF
			m_BufferLen = len1
			ResizeBuffer(size1)
			memcpy(m_pBuffer , cptr(wstring ptr , pub) , m_BufferLen * _MY_SIZE_WSTRING_)
			' Mark the end of the string with a null
			m_pBuffer[m_BufferLen] = 0
			#IFDEF ___STORE___SURROGATE___PAIRS___
				this.m_SurPair = this.Sur_Count()
			#ENDIF
			DWSTR_DP( "    - DWSTR CONSTRUCTOR UBYTE - COPY BUFFER  to " & WSTR(this.m_pBuffer))
		END IF
		DWSTR_DP( "-END- DWSTR CONSTRUCTOR UBYTE - " & WSTR(this.m_pBuffer))
	END CONSTRUCTOR
	' ========================================================================================

	' ========================================================================================
	' Destructor
	' ========================================================================================
	PRIVATE DESTRUCTOR DWSTR
		DWSTR_DP( "***DWSTR DESTRUCTOR - buffer: " & WSTR(m_pBuffer))
		IF m_pBuffer <> 0 and m_Flag = 0 THEN            '**** if mFlag = 1 do not deallocate, because not allocated here
			Deallocate(m_pBuffer)
			m_pBuffer = NULL
		else
			DWSTR_DP( "   DWSTR DESTRUCTOR - not deallocated: " & WSTR(m_pBuffer))
		end if
	END DESTRUCTOR
	' ========================================================================================

	' ========================================================================================
	' Count the nb of surrogate pairs in DWSTR wstring chain, needed for windows unicode utf16
	' ========================================================================================
	PRIVATE FUNCTION DWSTR.Sur_Count(BYVAL nIndex AS LONG = 1, BYVAL nEnd AS LONG = &h7FFFFFFE) AS LONG
		#IFDEF __AS__LINUX__
			return 0
		#ELSE
			#IFDEF ___USE___UCS2___ONLY___
				return 0
			#ELSE
				dim  as long y
				if nIndex < 1 or nIndex > m_BufferLen or nEnd < 1 THEN return 0
				nIndex -= 1
				if nEnd > m_BufferLen then nEnd = m_BufferLen
				Do While nIndex < nEnd
					if IS_HIGH_UTF16_SUR(m_pBuffer[nIndex]) then
						if nIndex = nEnd - 1 THEN exit do
						nIndex += 1
						if IS_LOW_UTF16_SUR(m_pBuffer[nIndex]) then y += 1
					END IF
					nIndex += 1
				LOOP
				return y
			#ENDIF
		#ENDIF
	END FUNCTION
	' ========================================================================================

	' ========================================================================================
	' Reverse Count the nb of surrogate pairs in DWSTR wstring chain, needed for windows unicode utf16
	' ========================================================================================
	PRIVATE FUNCTION DWSTR.Sur_CountRev(BYVAL nIndex AS LONG = 1, BYVAL nEnd AS LONG = &h7FFFFFFE) AS LONG
		#IFDEF __AS__LINUX__
			return 0
		#ELSE
			#IFDEF ___USE___UCS2___ONLY___
				return 0
			#ELSE
				dim  as long y
				if nIndex < 1 or nIndex > m_BufferLen or nEnd < 1 THEN return 0
				if nEnd > m_BufferLen then nEnd = m_BufferLen
				Do While nIndex < nEnd + 1
					if IS_LOW_UTF16_SUR(m_pBuffer[m_BufferLen - nIndex])then
						if nIndex = nEnd  THEN exit do
						nIndex += 1
						if IS_HIGH_UTF16_SUR(m_pBuffer[m_BufferLen - nIndex])then y += 1
					END IF
					nIndex += 1
				LOOP
				return y
			#ENDIF
		#ENDIF
	END FUNCTION
	' ========================================================================================

	' ========================================================================================
	' dereferencing DWSTR same as *zstring or as *wstring.
	' in fact dereferencing the wstring ptr buffer, just a short way to get the value as byref as wstring
	' ========================================================================================
	PRIVATE OPERATOR * (BYREF ust AS DWSTR) BYREF AS WSTRING
		DWSTR_DP( "DWSTR OPERATOR * buffer: " & WSTR(ust.m_pBuffer))
		RETURN * ust.m_pBuffer
	END OPERATOR
	' ========================================================================================
	' ========================================================================================
	' Returns the length of the DWSTR.
	' ========================================================================================
	PRIVATE OPERATOR LEN(BYREF ust AS DWSTR) AS LONG
		DWSTR_DP( "DWSTR OPERATOR LEN -   m_BufferLen: " & WSTR(ust.m_BufferLen))
		#IFDEF ___STORE___SURROGATE___PAIRS___
			DWSTR_DP("DWSTR OPERATOR LEN -   m_SurPair  : " & WSTR(ust.m_SurPair ))
		#ENDIF
		RETURN(ust.m_BufferLen)
	END OPERATOR
	' ========================================================================================

	' ========================================================================================
	' Cast implicitly DWSTR to different types.
	' ========================================================================================
	PRIVATE OPERATOR DWSTR.CAST() BYREF AS WSTRING
		DWSTR_DP( "DWSTR CAST BYREF AS WSTRING - buffer: " & WSTR(m_pBuffer))
		RETURN * m_pBuffer
	END OPERATOR
	' ========================================================================================
	' ========================================================================================
	PRIVATE OPERATOR DWSTR.CAST() AS ANY PTR
		DWSTR_DP( "DWSTR CAST ANY PTR - buffer: " & WSTR(m_pBuffer))
		RETURN cast(ANY PTR , m_pBuffer)
	END OPERATOR
	' ========================================================================================

	' ========================================================================================
	' Assigns new text to the DWSTR.
	' ========================================================================================
	PRIVATE OPERATOR DWSTR.Let(BYREF wszStr AS CONST WSTRING)
		DWSTR_DP( "DWSTR LET WSTRING")
		this.Clear
		this.Add(wszStr)
	END OPERATOR
	' ========================================================================================
	' ========================================================================================
	PRIVATE OPERATOR DWSTR.Let(BYREF ansiStr AS STRING)
		DWSTR_DP( "DWSTR LET STRING")
		this.Clear
		this.Add(ansiStr)
	END OPERATOR
	' ========================================================================================

	' ========================================================================================
	PRIVATE OPERATOR DWSTR.Let(BYREF pwszStr AS WSTRING PTR)
		DWSTR_DP( "DWSTR LET WSTRING PTR")
		IF pwszStr = NULL THEN
			pwszStr = @wstr( "")
		end if
		this.Clear
		this.Add(pwszStr)
	END OPERATOR
	' ========================================================================================
	' ========================================================================================
	PRIVATE OPERATOR DWSTR.Let(BYREF ust AS DWSTR)
		DWSTR_DP( "DWSTR LET DWSTR")
		IF m_pBuffer = ust.m_pBuffer THEN EXIT OPERATOR  ' // Ignore ust = ust
		if ust.m_Flag = 1 THEN
			if m_pBuffer then
				DWSTR_DP( "   DWSTR LET DWSTR : - deallocate buffer: " & WSTR(m_pBuffer))
				deallocate m_pBuffer
			end if
			m_pBuffer = ust.m_pBuffer
			m_BufferLen = ust.m_BufferLen
			m_Capacity = ust.m_capacity
			#IFDEF ___STORE___SURROGATE___PAIRS___
				m_SurPair = ust.m_SurPair
			#ENDIF
			DWSTR_DP( "   DWSTR LET DWSTR : - assign buffer: " & WSTR(m_pBuffer))
		else
			DWSTR_DP( "   DWSTR LET DWSTR : - copy buffer: " & WSTR(m_pBuffer))
			this.Clear
			this.Add(ust.m_pBuffer , ust.m_BufferLen, ust.m_SurPair)
		end if
	END OPERATOR
	' ========================================================================================

	' ========================================================================================
	' Get the internal buffer 'content' as wstring
	' ========================================================================================
	PRIVATE FUNCTION DWSTR.Get() BYREF AS WSTRING
		DWSTR_DP( "DWSTR Get content - buffer: " & WSTR(m_pBuffer))
		RETURN * m_pBuffer
	END FUNCTION
	' ========================================================================================

	' ========================================================================================
	' ResizeBuffer
	' Increase the size of the internal buffer capacity
	' ========================================================================================
	PRIVATE SUB DWSTR.ResizeBuffer(BYVAL nValue AS LONG)
		DWSTR_DP( "DWSTR ResizeBuffer - Value = " & WSTR(nValue))
		' Allocate or reallocate buffer to the right size preversing data when exiting
		DWSTR_DP( "DWSTR ResizeBuffer ;   old buffer = " &(WSTR(m_pBuffer)))
		IF m_pBuffer THEN
			m_pBuffer = reAllocate(m_pBuffer ,(nValue + 1) *_MY_SIZE_WSTRING_)
		else
			m_pBuffer = Allocate((nValue + 1) *_MY_SIZE_WSTRING_)
		END IF
		DWSTR_DP( "DWSTR ResizeBuffer ;   new buffer = " &(WSTR(m_pBuffer)))
		m_Capacity = nValue
		' Mark the end of the string with a null
		m_pBuffer[m_BufferLen] = 0
	END SUB
	' ========================================================================================

	' ========================================================================================
	' (Private) Write the number of Wchars from the specified memory address to the buffer.
	' ========================================================================================
	PRIVATE SUB DWSTR.WriteBuffer(BYVAL addrMemory AS WSTRING PTR , BYVAL nNumWchars AS LONG, BYVAL nNumSur AS LONG = -1)
		m_BufferLen = nNumWchars                      'assign m_BufferLen before modifying nNumWchars
		' the idea here is to have at least some buffer reserve to not have always to resize if append after
		if nNumWchars < m_GrowSize / 2 THEN
			nNumWchars = m_GrowSize
		else
			nNumWchars = nNumWchars * 2
		end if
		DWSTR_DP( "DWSTR WriteBuffer " & WSTR(nNumWchars))
		this.ResizeBuffer(nNumWchars)
		memcpy(m_pBuffer , addrMemory , m_BufferLen * _MY_SIZE_WSTRING_)
		' Mark the end of the string with a null
		m_pBuffer[m_BufferLen] = 0
		#IFDEF ___STORE___SURROGATE___PAIRS___
			if nNumSur <> -1 THEN
				m_SurPair += nNumSur
			else
				m_SurPair = this.Sur_Count()
         END IF
		#ENDIF
		DWSTR_DP( "--END - DWSTR WriteBuffer " & WSTR(m_BufferLen))
	END SUB
	' ========================================================================================

	' ========================================================================================
	' (Private) Append the number of Wchars from the specified memory address to the end of the buffer.
	' ========================================================================================
	PRIVATE SUB DWSTR.AppendBuffer(BYVAL addrMemory AS WSTRING PTR , BYVAL nNumWchars AS LONG, BYVAL nNumSur AS LONG = -1)
		DWSTR_DP( "DWSTR AppendBuffer " & WSTR(m_BufferLen) & " " & WSTR(nNumWchars))
		IF (m_BufferLen + nNumWchars) > m_Capacity THEN this.ResizeBuffer((m_BufferLen + nNumWchars) *2)
		memcpy(m_pBuffer + m_BufferLen , addrMemory , nNumWchars * _MY_SIZE_WSTRING_)
		m_BufferLen += nNumWchars
		' Mark the end of the string with a null
		m_pBuffer[m_BufferLen] = 0
		#IFDEF ___STORE___SURROGATE___PAIRS___
			if nNumSur <> -1 THEN
				m_SurPair += nNumSur
			else
				m_SurPair = this.Sur_Count()
         END IF
		#ENDIF
		DWSTR_DP( "--END - DWSTR AppendBuffer " & WSTR(m_BufferLen))
	END SUB
	' ========================================================================================

	' ========================================================================================
	' The string parameter is appended to the string held in the class. If the internal string
	' buffer overflows, the class will automatically extend it to an appropriate size.
	' ========================================================================================
	PRIVATE SUB DWSTR.Add(BYREF ust AS DWSTR)
		DWSTR_DP( "DWSTR Add DWSTR - LEN = " & WSTR(ust.m_BufferLen))
		' Incoming string is already in wide format, simply copy it to the buffer.
		IF ust.m_BufferLen = 0 THEN
			if m_pBuffer = NULL THEN
				if ust.m_Capacity = 0 THEN
					this.ResizeBuffer(m_GrowSize)        '**** check if capacity
				else
					this.ResizeBuffer(ust.m_Capacity)
				END IF
			END IF
			RETURN
		END IF
		' Copy the string into the buffer and update the length
		IF m_pBuffer = NULL THEN
			this.WriteBuffer(ust.m_pBuffer , ust.m_BufferLen, ust.m_SurPair)
		ELSE
			this.AppendBuffer(ust.m_pBuffer , ust.m_BufferLen, ust.m_SurPair)
		END IF
	END SUB
	' ========================================================================================
	' ========================================================================================
	PRIVATE SUB DWSTR.Add(BYVAL pwszStr AS WSTRING PTR, BYVAL nLen AS LONG = - 1, BYVAL nNumSur AS LONG = -1)
		DWSTR_DP( "DWSTR Add WSTRING")
		if pwszStr = NULL THEN
			nLen = 0
		end if
		' Incoming string is already in wide format
		DIM AS LONG nLenString
		if nLen > - 1 THEN
			nLenString = nLen
		else
			#IFDEF ___STORE___SURROGATE___PAIRS___
				DIM AS LONG nNum
				nLenString = Dw_LenSur(pwszStr, nNum)
				nNumSur = nNum
			#ELSE
				nLenString = LEN(*pwszStr)
			#ENDIF
		END IF
		IF nLenString = 0 THEN
			IF m_pBuffer = NULL THEN this.ResizeBuffer(m_GrowSize)
			RETURN
		END IF
		' Copy the string into the buffer and update the length
		IF m_pBuffer = NULL THEN                  '***
			this.WriteBuffer(pwszStr , nLenString, nNumSur)
		ELSE
			this.AppendBuffer(pwszStr , nLenString, nNumSur)
		END IF
	END SUB
	' ========================================================================================
	' ========================================================================================
	PRIVATE SUB DWSTR.Add(BYREF ansiStr AS STRING)
		DWSTR_DP( "DWSTR Add STRING ")
		DIM as ulong ulen = LEN(ansiStr)
		IF ulen = 0 THEN
			if m_pBuffer = NULL THEN this.ResizeBuffer(m_GrowSize)
			RETURN
		END IF
		DIM as ulong i1 = ulen + 1
		' Create the wide string from the incoming ansi string
		DIM s2 as string = string(i1 * _MY_SIZE_WSTRING_ , 0)
		DIM ps2 as wstring ptr = cptr(wstring ptr , strptr(s2))
		IF ps2 THEN
			#IFDEF __AS__LINUX__                        ' assumed it is always utf8 coded
				UTFToWChar(1 , strptr(ansiStr) , ps2 , cptr(integer ptr , @i1))
				ulen = i1
			#ELSE                                      ' assumed it is not utf8 coded
				MultiByteToWideChar(0 , MB_PRECOMPOSED , STRPTR(ansiStr) , ulen , ps2 , i1)
			#ENDIF
			' Copy the string into the buffer and update the length
			dim as long nsur
			#IFDEF ___STORE___SURROGATE___PAIRS___
				if ulen <> LEN(ansiStr) THEN ulen = Dw_LenSur(ps2, nsur)
			#ENDIF
				IF m_pBuffer = NULL THEN
					this.WriteBuffer(ps2, ulen, nsur)
				ELSE
					this.AppendBuffer(ps2, ulen, nsur)
				END IF
		ELSE
			RETURN
		END IF
	END SUB
	' ========================================================================================

	' ========================================================================================
	' Appends a wstring to the DWSTR
	' ========================================================================================
	PRIVATE OPERATOR DWSTR.+= (BYREF wszStr AS WSTRING)
		DWSTR_DP( "DWSTR OPERATOR += WSTRING")
		this.Add(wszStr)
	END OPERATOR
	' ========================================================================================

	' ========================================================================================
	' Appends a string to the DWSTR
	' ========================================================================================
	PRIVATE OPERATOR DWSTR.+= (BYREF ansiStr AS STRING)
		DWSTR_DP( "DWSTR OPERATOR += STRING")
		this.Add(ansiStr)
	END OPERATOR
	' ========================================================================================

	' ========================================================================================
	' Appends a DWSTR to the DWSTR
	' ========================================================================================
	PRIVATE OPERATOR DWSTR.+= (BYREF ust AS DWSTR)
		DWSTR_DP( "DWSTR OPERATOR += DWSTR")
		this.Add(ust.m_pBuffer , ust.m_BufferLen, ust.m_SurPair)
	END OPERATOR
	' ========================================================================================

	' ========================================================================================
	' Appends a WSTRING to the DWSTR
	' ========================================================================================
	PRIVATE OPERATOR DWSTR.&= (BYREF wszStr AS WSTRING)
		DWSTR_DP( "DWSTR OPERATOR &= WSTRING")
		this.Add(wszStr)
	END OPERATOR
	' ========================================================================================

	' ========================================================================================
	' Appends a string to the DWSTR
	' ========================================================================================
	PRIVATE OPERATOR DWSTR.&= (BYREF ansiStr AS STRING)
		DWSTR_DP( "DWSTR OPERATOR &= STRING")
		this.Add(ansiStr)
	END OPERATOR
	' ========================================================================================

	' ========================================================================================
	' Appends a DWSTR to the DWSTR
	' ========================================================================================
	PRIVATE OPERATOR DWSTR.&= (BYREF ust AS DWSTR)
		DWSTR_DP( "DWSTR OPERATOR &= DWSTR")
		this.Add(ust.m_pBuffer , ust.m_BufferLen, ust.m_SurPair)
	END OPERATOR
	' ========================================================================================

	' ========================================================================================
	' Appends a DOUBLE( or any numeric value) "string representation" to the DWSTR
	' can accept any kind of numeric type but better not use explicit float type because
	' the num value is implicitly converted to double and the fractionnal part is altered (if exists)
	' ========================================================================================
	PRIVATE OPERATOR DWSTR.&= (BYVAL num AS DOUBLE)
		DWSTR_DP( "DWSTR OPERATOR &= DOUBLE")
		this.Add(str(num))
	END OPERATOR
	' ========================================================================================

	' ========================================================================================
	' The size of the internal string buffer is retrieved and returned to the caller. The size
	' is the number of characters which can be stored without further expansion.
	' ========================================================================================
	PRIVATE PROPERTY DWSTR.Capacity() AS LONG
		DWSTR_DP( "DWSTR PROPERTY GET Capacity")
		PROPERTY = m_Capacity
	END PROPERTY
	' ========================================================================================
	' ========================================================================================
	' The internal string buffer is expanded to the specified number of characters. If the new
	' capacity is smaller or equal to the current capacity, no operation is performed.
	' ========================================================================================
	PRIVATE PROPERTY DWSTR.Capacity(BYVAL nValue AS LONG)
		DWSTR_DP( "DWSTR PROPERTY SET Capacity")
		IF nValue <= m_Capacity THEN EXIT PROPERTY
		this.ResizeBuffer(nValue)
	END PROPERTY
	' ========================================================================================

	' ========================================================================================
	' Returns the corresponding unicode integer representation of the character at the position
	' specified by the nIndex parameter (1 for the first character, 2 for the second, etc.).
	' If nIndex is beyond the current length of the string, a 0 is returned.
	' ========================================================================================
	PRIVATE PROPERTY DWSTR.Char(BYVAL nIndex AS LONG) AS _MY_W_CHAR_ ' index counted in wstrings not in bytes,
		DWSTR_DP( "DWSTR PROPERTY GET Char")
		IF (nIndex < 1) Or (nIndex > m_BufferLen) THEN RETURN 0
		' Get the numeric character code at position nIndex
		nIndex -= 1
		PROPERTY = PEEK(_MY_W_CHAR_ , m_pBuffer + nIndex)
	END PROPERTY
	' ========================================================================================
	' ========================================================================================
	' Changes the corresponding unicode integer representation of the character at the position
	' specified by the nIndex parameter (1 for the first character, 2 for the second, etc.).
	' If nIndex is beyond the current length of the string, nothing is changed.
	' ========================================================================================
	PRIVATE PROPERTY DWSTR.Char(BYVAL nIndex AS LONG , BYVAL nValue AS _MY_W_CHAR_) 'index as position in the wstring
		DWSTR_DP( "DWSTR PROPERTY SET Char")
		IF (nIndex < 1) Or (nIndex > m_BufferLen) THEN EXIT PROPERTY
		if not(IS_FULL_VALID_RANGE(nValue)) then nValue = 126
		#IFNDEF __AS__LINUX__
			if nValue > &hFFFF THEN nValue = 126
		#ENDIF
		' Set the numeric character code at position nIndex (zero based)
		nIndex -= 1
		POKE _MY_W_CHAR_ , m_pBuffer + nIndex , nValue
		#IFDEF ___STORE___SURROGATE___PAIRS___
			m_SurPair = Sur_Count()
		#ENDIF
	END PROPERTY
	' ========================================================================================
	' ========================================================================================
	' Returns the corresponding ASCII or Unicode integer representation of the character at
	' the position specified by the nIndex parameter. Like the get Char property, but allowing
	' to use the [] syntax, e.g. value = cws[1]. Can't be used to change a value.
	' ========================================================================================
	PRIVATE OPERATOR DWSTR.[](BYVAL nIndex AS LONG) AS _MY_W_CHAR_
		DWSTR_DP( "DWSTR [] GET code")
		IF (nIndex < 0) Or (nIndex > m_BufferLen) THEN RETURN 0
		RETURN This.m_pBuffer[nIndex ]
	END OPERATOR
	' ========================================================================================

	' ========================================================================================
	' All data in the class object is erased. Actually, we only set the buffer length to zero,
	' indicating no string in the buffer. The allocated memory for the buffer is deallocated
	' when the class is destroyed.
	' ========================================================================================
	PRIVATE SUB DWSTR.Clear
		DWSTR_DP( "DWSTR Clear")
		m_BufferLen = 0
		' Mark the end of the string with a null
		m_pBuffer[m_BufferLen] = 0
		m_SurPair = 0
	END SUB
	' ========================================================================================

	' ========================================================================================
	' nCount characters are removed starting at the position given by nIndex.
	' nIndex = 1 for the first character, 2 for the second, etc.
	' ========================================================================================
	PRIVATE FUNCTION DWSTR.DelChars(BYVAL nIndex AS LONG , BYVAL nCount AS LONG) AS LONG
		DWSTR_DP( "DWSTR DelChars")
		IF (nIndex < 1) Or (nIndex > m_BufferLen) or (nCount < 1) THEN RETURN - 1
		IF nCount > m_BufferLen - nIndex + 1 THEN nCount = m_BufferLen - nIndex + 1
		memcpy(m_pBuffer + nIndex - 1 , m_pBuffer + (nIndex + nCount) - 1 ,(m_BufferLen - nCount) *_MY_SIZE_WSTRING_)
		m_BufferLen -= (nCount)
		' Mark the end of the string with a null
		m_pBuffer[m_BufferLen] = 0
		#IFDEF ___STORE___SURROGATE___PAIRS___
			m_SurPair = Sur_Count()
		#ENDIF
	END FUNCTION
	' ========================================================================================

	' ========================================================================================
	' (Private) Insert the number of Wchars from the specified memory address into the buffer.
	' ========================================================================================
	PRIVATE SUB DWSTR.InsertBuffer(BYVAL addrMemory AS WSTRING PTR, BYVAL nIndex AS LONG, BYVAL nNumWchars AS LONG, BYVAL nNumSur AS LONG = -1)
		DWSTR_DP( "DWSTR InsertBuffer")
		nIndex -= 1
		' Determine the size of the new buffer
		IF m_BufferLen + nNumWchars > m_Capacity THEN m_Capacity = (m_BufferLen + nNumWchars) *2
		DIM pNewBuffer AS WSTRING PTR = Allocate((m_Capacity + 1) *_MY_SIZE_WSTRING_)
		IF pNewBuffer THEN
			' Copy the existing data into the new buffer
			memcpy(pNewBuffer , m_pBuffer ,(nIndex * _MY_SIZE_WSTRING_))
			memcpy(pNewBuffer + nIndex , addrMemory , nNumWchars * _MY_SIZE_WSTRING_)
			memcpy(pNewBuffer + nIndex + nNumWchars , m_pBuffer + nIndex ,(m_BufferLen - nIndex) *_MY_SIZE_WSTRING_)
			Deallocate m_pBuffer
		END IF
		m_pBuffer = pNewBuffer
		m_BufferLen += nNumWchars
		' Mark the end of the string with a null
		m_pBuffer[m_BufferLen] = 0
		#IFDEF ___STORE___SURROGATE___PAIRS___
			if nNumSur = -1 THEN
				m_SurPair = Sur_Count()
			else
				m_SurPair += nNumSur
         END IF
		#ENDIF
	END SUB
	' ========================================================================================

	' ========================================================================================
	' The incoming string parameter is inserted in the string starting at the position of wstring
	' given by nIndex. nIndex = 1 for the first character, 2 For the second, etc.
	' If nIndex is beyond the current length of the string +1, no operation is performed.
	' ========================================================================================
	PRIVATE FUNCTION DWSTR.Insert(BYVAL pwszStr AS WSTRING PTR , BYVAL nIndex AS LONG) AS LONG
		DWSTR_DP( "DWSTR Insert WSTRING")
		IF (pwszStr = NULL) or (nIndex < 1) Or (nIndex > m_BufferLen) THEN RETURN - 1
		Dim as long nsur
		#IFDEF ___STORE___SURROGATE___PAIRS___
			DIM AS LONG nLenString = Dw_LenSur(pwszStr, nsur)
		#ELSE
			DIM AS LONG nLenString = LEN(*pwszStr)
		#ENDIF
		IF nLenString = 0 THEN RETURN 0
		this.InsertBuffer(pwszStr , nIndex , nLenString, nsur)
		RETURN 0
	END FUNCTION
	' ========================================================================================
	' ========================================================================================
	PRIVATE FUNCTION DWSTR.Insert(BYREF ust1 AS DWSTR , BYVAL nIndex AS LONG) AS LONG
		DWSTR_DP( "DWSTR Insert DWSTR")
		IF (nIndex < 1) Or (nIndex > m_BufferLen) THEN RETURN - 1
		IF ust1.m_BufferLen = 0 THEN RETURN 0
		this.InsertBuffer(ust1.m_pBuffer , nIndex , ust1.m_BufferLen, ust1.m_SurPair)
		RETURN 0
	END FUNCTION
	' ========================================================================================

	' ========================================================================================
	PRIVATE FUNCTION DWSTR.Insert(BYREF ansiStr AS STRING , BYVAL nIndex AS LONG) AS LONG
		DWSTR_DP( "DWSTR Insert STRING")
		IF (nIndex < 1) Or (nIndex > m_BufferLen) THEN RETURN - 1
		DIM AS LONG nLenString = LEN(ansiStr)
		IF nLenString = 0 THEN RETURN 0
		DIM as ulong i1 = nLenString + 1
		' Create the wide string from the incoming ansi string
		DIM s2 as string = string(i1 * _MY_SIZE_WSTRING_ , 0)
		DIM ps2 as wstring ptr = cptr(wstring ptr , strptr(s2))
		IF ps2 THEN
			#IFDEF __AS__LINUX__                        ' assumed it is always utf8 coded
				UTFToWChar(1 , strptr(ansiStr) , ps2 , cptr(integer ptr , @i1))
				nLenString = i1
			#ELSE                                      ' assumed it is not utf8 coded
				MultiByteToWideChar(0 , MB_PRECOMPOSED , STRPTR(ansiStr) , nLenString , ps2 , i1)
			#ENDIF
			dim as long nsur
			#IFDEF ___STORE___SURROGATE___PAIRS___
				if nLenString <> LEN(ansiStr) THEN nLenString = Dw_LenSur(ps2, nsur)
			#ENDIF
			' Copy the string into the buffer and update the length
			this.InsertBuffer(ps2, nIndex, nLenString, nsur)
		ELSE
			RETURN - 1
		END IF
		RETURN 0
	END FUNCTION
	' ========================================================================================


	' ========================================================================================
	' (Private) Replace the number of Wchars from the specified memory address into the buffer at specified nIndex
	' ========================================================================================
	PRIVATE SUB DWSTR.ReplaceBuffer(BYVAL addrMemory AS WSTRING PTR , BYVAL nIndex AS LONG , BYVAL nNumWchars AS LONG) '**** new
		DWSTR_DP( "DWSTR ReplaceBuffer")
		nIndex -= 1
		' Determine the size of the new buffer
		IF nIndex + nNumWchars > m_Capacity THEN
			m_Capacity = (nIndex + nNumWchars ) * 2
			m_pBuffer = reallocate(m_pBuffer ,(m_Capacity + 1) *_MY_SIZE_WSTRING_)
		END IF
		' Copy the replacing data into the buffer
		IF m_pBuffer THEN
			memcpy(m_pBuffer + nIndex , addrMemory , nNumWchars * _MY_SIZE_WSTRING_)
			if m_BufferLen < nIndex + nNumWchars  THEN
				m_BufferLen = nIndex + nNumWchars
				' Mark the end of the string with a null
				m_pBuffer[m_BufferLen] = 0
			end if
			#IFDEF ___STORE___SURROGATE___PAIRS___
				m_SurPair = Sur_Count()
			#ENDIF
		end if
		return
	END SUB
	' ========================================================================================

	' ========================================================================================
	' The incoming string parameter is replacing in the string starting at the position of wstring
	' given by nIndex. nIndex = 1 for the first character, 2 For the second, etc.
	' If nIndex is beyond the current length of the string +1, no operation is performed.
	' If the number of chars to replace exceeds the len of existing DWSRT it will be resized to accept the incoming text
	' ========================================================================================
	PRIVATE FUNCTION DWSTR.Replace(BYVAL pwszStr AS WSTRING PTR , BYVAL nIndex AS LONG) AS LONG '**** new
		DWSTR_DP( "DWSTR Replace WSTRING")
		IF (pwszStr = NULL) or (nIndex < 1) Or (nIndex > m_BufferLen) THEN RETURN - 1
		DIM AS LONG nLenString = LEN(*pwszStr)
		IF nLenString = 0 THEN RETURN 0
		this.ReplaceBuffer(pwszStr , nIndex , nLenString)
		RETURN 0
	END FUNCTION
	' ========================================================================================
	' ========================================================================================
	PRIVATE FUNCTION DWSTR.Replace(BYREF ust1 AS DWSTR , BYVAL nIndex AS LONG) AS LONG '**** new
		DWSTR_DP( "DWSTR Replace DWSTR")
		IF (nIndex < 1) Or (nIndex > m_BufferLen) THEN RETURN - 1
		IF ust1.m_BufferLen = 0 THEN RETURN 0
		this.ReplaceBuffer(ust1.m_pBuffer , nIndex , ust1.m_BufferLen)
		RETURN 0
	END FUNCTION
	' ========================================================================================
	' ========================================================================================
	PRIVATE FUNCTION DWSTR.Replace(BYREF ansiStr AS STRING , BYVAL nIndex AS LONG) AS LONG '**** new
		DWSTR_DP( "DWSTR Replace STRING")
		IF (nIndex < 1) Or (nIndex > m_BufferLen) THEN RETURN - 1
		DIM AS LONG nLenString = LEN(ansiStr)
		IF nLenString = 0 THEN RETURN 0
		DIM as ulong i1 = nLenString + 1
		' Create the wide string from the incoming ansi string
		DIM s2 as string = string(i1 * _MY_SIZE_WSTRING_ , 0)
		DIM ps2 as wstring ptr = cptr(wstring ptr , strptr(s2))
		IF ps2 THEN
			#IFDEF __AS__LINUX__                        ' assumed it is always utf8 coded
				UTFToWChar(1 , strptr(ansiStr) , ps2 , cptr(integer ptr , @i1))
				nLenString = i1
			#ELSE                                      ' assumed it is not utf8 coded
				MultiByteToWideChar(0 , MB_PRECOMPOSED , STRPTR(ansiStr) , nLenString , ps2 , i1)
			#ENDIF
			' Copy the string into the buffer and update the length
			this.ReplaceBuffer(ps2 , nIndex , nLenString)
		ELSE
			RETURN - 1
		END IF
		RETURN 0
	END FUNCTION
	' ========================================================================================

	' ========================================================================================
	' New & OPERATORS to play with Wstring and DWSTR
	' ========================================================================================
	PRIVATE OPERATOR &(BYREF ust1 AS DWSTR , byref ust2 AS DWSTR) AS DWSTR
		DWSTR_DP( "OPERATOR & DWSTR :  DWSTR & DWSTR")
		DIM pNewBuffer AS WSTRING PTR = allocate((ust1.m_BufferLen + ust2.m_BufferLen + 2) *_MY_SIZE_WSTRING_)
		IF pNewBuffer THEN
			memcpy(pNewBuffer , ust1.m_pBuffer , ust1.m_BufferLen * _MY_SIZE_WSTRING_)
			memcpy(pNewBuffer + ust1.m_BufferLen , ust2.m_pBuffer , ust2.m_BufferLen * _MY_SIZE_WSTRING_)
			pNewBuffer[ust1.m_BufferLen + ust2.m_BufferLen] = 0
			pNewBuffer[ust1.m_BufferLen + ust2.m_BufferLen + 1] = _MY_TRAP_NUMBER_ '****extra value to check validity
			RETURN DWSTR(cptr(ubyte ptr , pNewBuffer) , ust1.m_BufferLen + ust2.m_BufferLen , - 1, ust1.m_SurPair + ust2.m_SurPair)
		END IF
		RETURN ""
	END OPERATOR

	PRIVATE OPERATOR &(BYREF wst1 AS WSTRING , BYREF ust2 AS DWSTR) AS DWSTR
		DWSTR_DP( "OPERATOR & DWSTR :  WSTRING & DWSTR")
		DIM AS long nSur
		#IFDEF ___STORE___SURROGATE___PAIRS___
			DIM AS LONG l1 = len(wst1)
		#ELSE
			DIM AS LONG l1 = Dw_LenSur(@wst1, nsur)
		#ENDIF
		DIM pNewBuffer AS WSTRING PTR = allocate((l1 + ust2.m_BufferLen + 2) *_MY_SIZE_WSTRING_)
		IF pNewBuffer THEN
			memcpy(pNewBuffer , @wst1 , l1 * _MY_SIZE_WSTRING_)
			memcpy(pNewBuffer + l1 , ust2.m_pBuffer , ust2.m_BufferLen * _MY_SIZE_WSTRING_)
			pNewBuffer[l1 + ust2.m_BufferLen] = 0
			pNewBuffer[l1 + ust2.m_BufferLen + 1] = _MY_TRAP_NUMBER_ '****extra value to check validity
			RETURN DWSTR(cptr(ubyte ptr , pNewBuffer) , l1 + ust2.m_BufferLen , - 1, nsur + ust2.m_SurPair)
		END IF
		RETURN ""
	END OPERATOR

	PRIVATE OPERATOR &(BYREF ust1 AS DWSTR , byref wst2 AS WSTRING) AS DWSTR
		DWSTR_DP( "OPERATOR & DWSTR :  DWSTR & WSTRING")
		DIM AS long nSur
		#IFDEF ___STORE___SURROGATE___PAIRS___
			DIM AS LONG l2 = len(wst2)
		#ELSE
			DIM AS LONG l2 = Dw_LenSur(@wst2, nsur)
		#ENDIF
		DIM pNewBuffer AS WSTRING PTR =  allocate((ust1.m_BufferLen + l2 + 2) *_MY_SIZE_WSTRING_)
		IF pNewBuffer THEN
			memcpy(pNewBuffer , ust1.m_pBuffer , ust1.m_BufferLen * _MY_SIZE_WSTRING_)
			memcpy(pNewBuffer + ust1.m_BufferLen , @wst2 , l2 * _MY_SIZE_WSTRING_)
			pNewBuffer[ust1.m_BufferLen + l2] = 0
			pNewBuffer[ust1.m_BufferLen + l2 + 1] = _MY_TRAP_NUMBER_ '****extra value to check validity
			RETURN DWSTR(cptr(ubyte ptr , pNewBuffer) , ust1.m_BufferLen + l2 , - 1, ust1.m_SurPair + nsur)
		END IF
		RETURN ""
	END OPERATOR



	PRIVATE OPERATOR &(BYREF ust1 AS DWSTR , BYVAL d1 AS DOUBLE) AS DWSTR
		DWSTR_DP( "OPERATOR & DWSTR :  DWSTR & DOUBLE")
		DIM as wstring *64 ws2 = wstr(d1)               'large enough buffer
		DIM AS Long l2 = len(ws2)
		DIM pNewBuffer AS WSTRING PTR = allocate((ust1.m_BufferLen + l2 + 2) *_MY_SIZE_WSTRING_)
		IF pNewBuffer THEN
			memcpy(pNewBuffer , ust1.m_pBuffer , ust1.m_BufferLen * _MY_SIZE_WSTRING_)
			memcpy(pNewBuffer + ust1.m_BufferLen , @ws2 , l2 * _MY_SIZE_WSTRING_)
			pNewBuffer[ust1.m_BufferLen + l2] = 0
			pNewBuffer[ust1.m_BufferLen + l2 + 1] = _MY_TRAP_NUMBER_ '****extra value to check validity
			RETURN DWSTR(cptr(ubyte ptr , pNewBuffer) , ust1.m_BufferLen + l2 , - 1, ust1.m_SurPair)
		END IF
		RETURN ""
	END OPERATOR

	PRIVATE OPERATOR &(BYVAL d1 AS DOUBLE , BYREF ust1 AS DWSTR) AS DWSTR
		DWSTR_DP( "OPERATOR & DWSTR :  DWSTR & DOUBLE")
		DIM as wstring *64 ws2 = wstr(d1)               'large enough buffer
		DIM AS Long l2 = len(ws2)
		DIM pNewBuffer AS WSTRING PTR = allocate((ust1.m_BufferLen + l2 + 2) *_MY_SIZE_WSTRING_)
		IF pNewBuffer THEN
			memcpy(pNewBuffer , @ws2 , l2 * _MY_SIZE_WSTRING_)
			memcpy(pNewBuffer + l2 , ust1.m_pBuffer , ust1.m_BufferLen * _MY_SIZE_WSTRING_)
			pNewBuffer[ust1.m_BufferLen + l2] = 0
			pNewBuffer[ust1.m_BufferLen + l2 + 1] = _MY_TRAP_NUMBER_ '****extra value to check validity
			RETURN DWSTR(cptr(ubyte ptr , pNewBuffer) , ust1.m_BufferLen + l2 , - 1, ust1.m_SurPair)
		END IF
		RETURN ""
	END OPERATOR

	' ========================================================================================
	' REDEFINITION / REPLACEMENT of STRPTR to be able to use that keyword with DWSTR
	' ========================================================================================
	#IFNDEF MY_STRPTR_REDEFINITION
		'#PRINT
		'#PRINT ====> REDEFINITION OF STRPTR ADDING DWSTR SUPPORT <====
		'#PRINT
		#UNDEF STRPTR                                 ' here is the trick undefining the normal STRPTR keyword

		TYPE MY_STRING_FB_                            ' type to mimic the string struct and get access to each element
			DIM data1             AS ZSTRING PTR
			DIM len1              AS LONG
			DIM size1             AS LONG
		END TYPE

		'replacement functions and extension to be able to use strptr() even with DWSTR
		DECLARE FUNCTION STRPTR OVERLOAD(BYREF str1 AS CONST STRING) AS ZSTRING PTR 'same
		DECLARE FUNCTION STRPTR OVERLOAD(BYREF ws1 AS CONST WSTRING) AS ZSTRING PTR 'same, remark : gives same strange return
		DECLARE FUNCTION STRPTR OVERLOAD(BYREF DWSTR1 AS CONST DWSTR) AS WSTRING PTR 'new, gives same behaviour as others
		DECLARE FUNCTION STRPTR OVERLOAD(BYREF str1 AS STRING) AS ZSTRING PTR 'same
		DECLARE FUNCTION STRPTR OVERLOAD(BYREF ws1 AS WSTRING) AS ZSTRING PTR 'same, remark : gives same strange return
		DECLARE FUNCTION STRPTR OVERLOAD(BYREF DWSTR1 AS DWSTR) AS WSTRING PTR 'new, gives same behaviour as others

		PRIVATE FUNCTION STRPTR(BYREF str1 AS CONST STRING) AS ZSTRING PTR
			DWSTR_DP( "STRPTR CONST STRING")
			if @str1 = NULL THEN RETURN NULL
			DIM my_lhs AS MY_STRING_FB_ PTR = Cptr(MY_STRING_FB_ PTR , @str1)
			RETURN my_lhs -> data1
		END FUNCTION

		PRIVATE FUNCTION STRPTR(BYREF ws1 AS CONST WSTRING) AS ZSTRING PTR
			DWSTR_DP( "STRPTR CONST WSTRING")
			RETURN Cptr(ZSTRING PTR , @ws1)
		END FUNCTION

		PRIVATE FUNCTION STRPTR(BYREF DWSTR1 AS CONST DWSTR) AS WSTRING PTR
			DWSTR_DP( "STRPTR CONST DWSTR")
			RETURN Cptr(WSTRING PTR , DWSTR1.m_Pbuffer)
		END FUNCTION

		PRIVATE FUNCTION STRPTR(BYREF str1 AS STRING) AS ZSTRING PTR
			DWSTR_DP( "STRPTR STRING")
			if @str1 = NULL THEN RETURN NULL
			DIM my_lhs AS MY_STRING_FB_ PTR = Cptr(MY_STRING_FB_ PTR , @str1)
			RETURN my_lhs -> data1
		END FUNCTION

		PRIVATE FUNCTION STRPTR(BYREF ws1 AS WSTRING) AS ZSTRING PTR
			DWSTR_DP( "STRPTR WSTRING")
			RETURN Cptr(ZSTRING PTR , @ws1)
		END FUNCTION

		PRIVATE FUNCTION STRPTR(BYREF DWSTR1 AS DWSTR) AS WSTRING PTR
			DWSTR_DP( "STRPTR DWSTR")
			RETURN Cptr(WSTRING PTR , DWSTR1.m_Pbuffer)
		END FUNCTION

		#DEFINE MY_STRPTR_REDEFINITION
	#ENDIF

	' ========================================================================================
	' Returns the address of the DWSTR buffer. Use the overloaded '-' global operator.
	' That operator is interresting because it does not need the () to work, and it is 'free' for that class
	' ========================================================================================
	PRIVATE OPERATOR - (BYREF dwstr1 as DWSTR) AS WSTRING PTR 'strptr equivalent form for dwstr
		DWSTR_DP( "OPERATOR  '-' for strptr  DWSTR :  WSTRING PTR = " & WSTR(dwstr1.m_pBuffer))
		return dwstr1.m_pBuffer
	END OPERATOR

	' ========================================================================================
	' RIGHT overloaded function , for DWSTR as input and output
	' ========================================================================================
	PRIVATE FUNCTION RIGHT(BYREF Ust AS DWSTR , BYVAL n AS LONG) AS DWSTR
		DWSTR_DP( "DWSTR -RIGHT FUNCTION-")
		if Ust.m_BufferLen = 0 or n <= 0 THEN
			RETURN ""
		elseif n > Ust.m_BufferLen THEN
			RETURN Ust.m_pBuffer
		else
			RETURN Ust.m_pBuffer + Ust.m_BufferLen - n
		END IF
	END FUNCTION

	' ========================================================================================
	' LEFT overloaded function , for DWSTR as input and output
	' ========================================================================================
	PRIVATE FUNCTION LEFT(BYREF Ust AS DWSTR , BYVAL n AS LONG) AS DWSTR
		DWSTR_DP( "DWSTR -LEFT FUNCTION-")
		IF Ust.m_BufferLen = 0 or n <= 0 THEN
			FUNCTION = ""
		ELSEIF n > Ust.m_BufferLen THEN
			FUNCTION = Ust.m_pBuffer
		ELSE
			dim as _MY_W_CHAR_ u1
			u1 = Ust.m_pBuffer[n]
			Ust.m_pBuffer[n] = 0
			FUNCTION = Ust.m_pBuffer
			Ust.m_pBuffer[n] = u1
		END IF
	END FUNCTION


 /'
	'======================================================================================================
	' possible extensions to avoid *dwstr on these functions, but they have impact on string speed process!
	' just to show/remind how to play for redifining intrinsic functions
	'======================================================================================================

	 #IFNDEF MY_STRMID_REDEFINITION
	 '#PRINT ====> REDEFINITION OF MID ADDING DWSTR SUPPORT <====
		 #UNDEF FB_STRMID
		 DECLARE Function FB_STRMID overload ( BYREF str1 AS string , BYVAL start as long, BYVAL n as long = &h7FFFFFFE )AS string
		 PRIVATE Function FB_STRMID ( BYREF str1 AS string , BYVAL start as long, BYVAL n as long = &h7FFFFFFE )AS string
			 dim i1 as long = len(str1)
			 if start < 1 or start > i1 or n < 1 THEN function = "" : exit function
			 dim as ubyte nstop
			 dim as zstring ptr ps2 = strptr(str1) + start - 1
			 if n > i1 - start + 1 THEN
			 function = *ps2
			 else
			 nstop = ps2[n]
			 ps2[n] = 0
			 function = *ps2
			 ps2[n] = nstop
			 END IF
		 END function

		 PRIVATE Function FB_STRMID ( BYREF ust AS DWSTR, BYVAL start as long, BYVAL n as long = &h7FFFFFFE )AS DWSTR
			 if start < 1 or start > Ust.m_BufferLen or n < 1 THEN function = "" : exit function
			 dim as _MY_W_CHAR_ nstop
			 dim as wstring ptr pw1 = Ust.m_pBuffer + start - 1
			 if n > Ust.m_BufferLen - start + 1 THEN
			 function = pw1
			 else
			 nstop = Ust.m_pBuffer[start + n - 1]
			 Ust.m_pBuffer[start + n - 1] = 0
			 function = pw1
			 Ust.m_pBuffer[start + n - 1] = nstop
			 END IF
		 END function

		 #DEFINE MY_STRMID_REDEFINITION
	 #ENDIF

	 #IFNDEF MY_STRUCASE2_REDEFINITION
		 #UNDEF FB_STRUCASE2
		 PRIVATE Function FB_STRUCASE2 ( BYREF ust AS wstring , BYVAL icode as long = 0)AS DWSTR
			 return FB_WSTRUCASE2(Ust, icode)
		 END function
		 #DEFINE MY_STRUCASE2_REDEFINITION
	 #ENDIF

	 #IFNDEF MY_STRLCASE2_REDEFINITION
		 #UNDEF FB_STRLCASE2
		 PRIVATE Function FB_STRLCASE2 ( BYREF ust AS wstring , BYVAL icode as long = 0)AS DWSTR
			 return FB_WSTRLCASE2(Ust, icode)
		 END function
		 #DEFINE MY_STRLCASE2_REDEFINITION
	 #ENDIF
 '/

	'======================================================================================================
	' Interesting for windows : Mid  taking care of surrogates , 'start' and 'n' position/nb  are counted in unit codes not in wchar
	'======================================================================================================
	PRIVATE Function Dw_Mid(BYREF ust AS DWSTR , BYVAL start AS LONG , BYVAL n AS LONG = &h7FFFFFFE) AS DWSTR
		if start < 1 or start > Ust.m_BufferLen or n < 1 THEN function = "" : exit function
		#IFNDEF __AS__LINUX__
			#IFNDEF ___USE___UCS2___ONLY___
				dim nsur as long
				dim temp as long
				#IFDEF ___STORE___SURROGATE___PAIRS___
					nsur = ust.m_SurPair
				#ELSE
					nsur = ust.Sur_Count()
				#ENDIF
				if nsur > 0 THEN
					temp = start
					do
						nsur = ust.Sur_Count(1, temp)
						if temp - nsur  =  start THEN exit do
						temp = start + nsur
						if temp > ust.m_BufferLen THEN function = "" : exit function
					loop
					start += nsur
					if start > Ust.m_BufferLen THEN function = "" : exit function
					temp = start + n
					do
						nsur = ust.Sur_Count(start, temp)
						if temp - nsur - start =  n  then exit do
						temp = n + nsur + start
						if temp > ust.m_BufferLen THEN exit do
					loop
					n += nsur
				end if
			#ENDIF
		#ENDIF
		dim as wstring ptr pw1 = Ust.m_pBuffer + start - 1
		if n > Ust.m_BufferLen - start + 1 THEN
			function = pw1
		else
			dim  nstop as _MY_W_CHAR_ = Ust.m_pBuffer[start + n - 1]
			Ust.m_pBuffer[start + n - 1] = 0
			function = pw1
			Ust.m_pBuffer[start + n - 1] = nstop
		END IF
	END function

	' ========================================================================================
	' dw_RIGHT  function , for DWSTR as input and output, n counted in unit code not wchar
	' ========================================================================================
	PRIVATE FUNCTION dw_RIGHT(BYREF Ust AS DWSTR , BYVAL n AS LONG) AS DWSTR
		DWSTR_DP( "DWSTR -dw_RIGHT FUNCTION-")
		if Ust.m_BufferLen = 0 or n <= 0 THEN
			RETURN ""
		elseif n > Ust.m_BufferLen THEN
			RETURN Ust.m_pBuffer
		end if
		#IFNDEF __AS__LINUX__
			#IFNDEF ___USE___UCS2___ONLY___
				dim nsur as long
				dim temp as long
				#IFDEF ___STORE___SURROGATE___PAIRS___
					nsur = ust.m_SurPair
				#ELSE
					nsur = ust.Sur_CountRev()
				#ENDIF
				if nsur > 0 THEN
					temp = n
					do
						nsur = ust.Sur_CountRev(1, temp)
						if temp - nsur  =  n THEN exit do
						temp = n + nsur
						if temp > ust.m_BufferLen THEN RETURN Ust.m_pBuffer
					loop
					n += nsur
					if n > Ust.m_BufferLen THEN RETURN Ust.m_pBuffer
				END IF
			#ENDIF
		#ENDIF
		RETURN Ust.m_pBuffer + Ust.m_BufferLen - n
	END FUNCTION

	' ========================================================================================
	' dw_LEFT function , for DWSTR as input and output, n counted in unit code not wchar
	' ========================================================================================
	PRIVATE FUNCTION dw_LEFT(BYREF Ust AS DWSTR , BYVAL n AS LONG) AS DWSTR
		DWSTR_DP( "DWSTR -dw_LEFT FUNCTION-")
		IF Ust.m_BufferLen = 0 or n <= 0 THEN
			FUNCTION = ""
		ELSEIF n > Ust.m_BufferLen THEN
			FUNCTION = Ust.m_pBuffer
		Else
			#IFNDEF __AS__LINUX__
				#IFNDEF ___USE___UCS2___ONLY___
					dim nsur as long
					dim temp as long
					#IFDEF ___STORE___SURROGATE___PAIRS___
						nsur = ust.m_SurPair
					#ELSE
						nsur = ust.Sur_Count()
					#ENDIF
					if nsur > 0 THEN
						temp = n
						do
							nsur = ust.Sur_Count(1, temp)
							if temp - nsur  =  n THEN exit do
							temp = n + nsur
							if temp > ust.m_BufferLen THEN FUNCTION = Ust.m_pBuffer : exit function
						loop
						n += nsur
						if n > Ust.m_BufferLen THEN FUNCTION = Ust.m_pBuffer : exit function
						if IS_HIGH_UTF16_SUR(Ust.m_pBuffer[n-1]) then
							if n < Ust.m_BufferLen THEN
								if IS_LOW_UTF16_SUR(Ust.m_pBuffer[n]) then n += 1
                     END IF
						end if
					END IF
				#ENDIF
			#ENDIF
			dim as _MY_W_CHAR_ u1
			u1 = Ust.m_pBuffer[n ]
			Ust.m_pBuffer[n ] = 0
			FUNCTION = Ust.m_pBuffer
			Ust.m_pBuffer[n ] = u1
		END IF
	END FUNCTION

	' ========================================================================================
	' Val overloaded function , using DWSTR as input
	' ========================================================================================
	PRIVATE FUNCTION VAL(BYREF ust AS DWSTR) AS DOUBLE
		RETURN VAL(*(Ust.m_pBuffer))
	END FUNCTION

	' ========================================================================================
	' Translates ansi string to "unicode" DWSTR.
	' Parameters:
	' - ansiStr = An ansi or UTF8 encoded string.
	' - nCodePage =
	'## in windows mode, its a num var
	' The code page related to ansiStr, e.g. 1251 for Russian.
	' If you specify CP_UTF8, the ansiStr string has to be UTF8 encoded.
	' If you don't pass an unicode page, the function will use CP_ACP (0), which is your
	' system default Windows ANSI code page to make the conversion.
	'## in linux mode, its a string var
	' The code page related to ansiStr, what are the code on Linux ?
	' If you specify "UTF-8" somewhere in codepage , the ansiStr string has to be UTF8 encoded.
	' If you don't pass an unicode page, the function will assume the string is utf8 coded
	' if you specify a code page, it will be set by setlocale(LC_CTYPE, ncodepage ) to make the conversion,
	' and the previous codepage is restored after the convertion
	' Return value:
	' An "unicode" DWSTR.
	' ========================================================================================
	#IFDEF __AS__LINUX__
		PRIVATE FUNCTION Dw_Wstr(BYREF ansiStr AS STRING , BYREF nCodePage AS STRING = "") AS DWSTR 'BYREF AS WSTRING
			DIM as ulong i1 = len(ansiStr)
			if i1 = 0 THEN return wstr("")
			DIM ps2 as wstring ptr = allocate((i1 + 2) *_MY_SIZE_WSTRING_)
			if ps2 THEN
				IF nCodePage = "" or instr(ucase(nCodePage) , "UTF-8") > 0 THEN 'assume default utf8
					UTFToWChar(1 , strptr(ansiStr) , ps2 , Cptr(integer ptr , @i1))
				ELSE
					MbsToWcs_2(nCodePage , ps2 , strptr(ansiStr) , i1)
				END IF
				ps2[i1] = 0
				ps2[i1 + 1] = _MY_TRAP_NUMBER_          '**** extra value to check validity
				RETURN DWSTR(Cptr(ubyte ptr , ps2) , i1 , - 1, 0)
			END IF
			return wstr("")
		END FUNCTION
	#ELSE
		PRIVATE FUNCTION Dw_Wstr(BYREF ansiStr AS STRING , BYVAL nCodePage AS LONG = 0) AS DWSTR  'BYREF AS WSTRING
			DIM as ulong i1 = len(ansiStr)
			if i1 = 0 THEN return wstr("")
			DIM ps2 as wstring ptr = allocate((i1 + 2) *_MY_SIZE_WSTRING_)
			if ps2 THEN
				IF nCodePage = CP_UTF8 THEN
					UTFToWChar(1 , strptr(ansiStr) , ps2 , Cptr(integer ptr , @i1)) ' **** much faster than 2 times MultiByteToWideChar
				ELSE
					MultiByteToWideChar(nCodePage , MB_PRECOMPOSED , STRPTR(ansiStr) , i1 , ps2 , i1 + 2)
				END IF
				ps2[i1] = 0
				ps2[i1 + 1] = _MY_TRAP_NUMBER_          '**** extra value to check validity
				RETURN DWSTR(Cptr(ubyte ptr , ps2) , i1 , - 1, -1)
			END IF
			return wstr("")
		END FUNCTION
	#ENDIF
	' ========================================================================================

	' ========================================================================================
	' Translates "unicode" DWSTR to ansi string type.
	' Parameters:
	' - ust1 = The unicode string to convert
	' - nCodePage =
	'## in windows mode, its a num var
	' The code page used in the conversion, e.g. 1251 for Russian.
	' If you specify CP_UTF8, it is assumed that the resulting string will be UTF8 encoded.
	' If you don't pass an unicode page, the function will use CP_ACP (0), which is your
	' system default Windows ANSI code page to make the conversion.
	'## in linux mode, its a string var
	' The code page related to the outpout String, what are the code on Linux ?
	' If you specify "UTF-8" somewhere in codepage , the string will be UTF8 encoded.
	' If you don't pass an unicode page, the function will assume the string has to be UTF8 coded
	' if you specify a code page, it will be set by setlocale(LC_CTYPE, ncodepage ) to make the conversion,
	' and the previous codepage is restored after the convertion
	' Return value:
	' The converted string.
	' ========================================================================================
	#IFDEF __AS__LINUX__
		PRIVATE FUNCTION Dw_Str(BYREF ust1 AS DWSTR , BYREF nCodePage AS STRING = "") AS STRING
			IF ust1.m_BufferLen = 0 THEN RETURN ""
			IF nCodePage = "" or instr(ucase(nCodePage) , "UTF-8") > 0 THEN 'assume default utf8
				dim i1 as integer = ust1.m_BufferLen * 5 + 1 'if all unicode chars use 5 bytes in utf8
				DIM ansiStr AS STRING = string(i1 , 0)
				return * Cptr(zstring ptr , WCharToUTF(1 , ust1.m_pBuffer , ust1.m_BufferLen , strptr(ansiStr) , @i1))
			ELSE
				RETURN Dwstr_To_Str2(ust1 , nCodePage)
			END IF
		END FUNCTION
	#ELSE
		PRIVATE FUNCTION Dw_Str(BYREF ust1 AS DWSTR , BYVAL nCodePage AS LONG = 0) AS STRING
			IF ust1.m_BufferLen = 0 THEN RETURN ""
			DIM ansiStr           AS STRING
			IF nCodePage = CP_UTF8 THEN
				dim i1 as integer = ust1.m_BufferLen * 5 + 1 'if all unicode chars use 5 bytes in utf8
				ansiStr = string(i1 , 0)
				return * Cptr(zstring ptr , WCharToUTF(1 , ust1.m_pBuffer , ust1.m_BufferLen , strptr(ansiStr) , @i1)) '**** much faster than 2 times WideCharToMultiByte
			ELSE
				ansiStr = string(ust1.m_BufferLen , 0)
				WideCharToMultiByte(nCodePage , 0 , Cptr(wstring ptr , ust1.m_pBuffer) , ust1.m_BufferLen , STRPTR(ansiStr) , ust1.m_BufferLen + 1 , NULL , NULL)
				RETURN ansiStr
			END IF
		END FUNCTION
	#ENDIF
	' ========================================================================================
	' ========================================================================================
	' Equivalent to string() function , creates a DWSTR with ncar of specific icode : unit code "unicode char"
	' ========================================================================================
	PRIVATE FUNCTION Dw_String(BYVAL ncar AS LONG, BYVAL icode AS ULONG = 0) AS DWSTR  '****
		IF icode = 0 or ncar = 0 THEN RETURN DWSTR(ncar)           'use capacity constructor just to allocate mem
		if not(IS_FULL_VALID_RANGE(icode)) then icode = 126
		dim as wstring ptr pw1
		dim as long x
		dim as long nSur
		#IFNDEF __AS__LINUX__
			#IFDEF ___USE___UCS2___ONLY___
				if icode >= &H10000 then icode = 126
			#ENDIF
			if (icode >= &H10000 and icode <= &H10FFFF) then
				dim as ulong hi = ((icode - &H10000) / &H400) + &HD800
				dim as ulong lo = ((icode - &H10000) mod &H400) + &HDC00
				nSur = ncar
				ncar = ncar * 2
				pw1 = allocate((ncar + 2) *_MY_SIZE_WSTRING_) 'cast(wstring ptr, strptr(str1))
				FOR x = 0 to(ncar - 1) step 2
					pw1[x] = hi
					pw1[x + 1] = lo
				next
			else
				pw1 = allocate((ncar + 2) *_MY_SIZE_WSTRING_) 'cast(wstring ptr, strptr(str1))
				FOR x = 0 to ncar - 1
					pw1[x] = icode
				NEXT
			end if

		#ELSE
			pw1 = allocate((ncar + 2) *_MY_SIZE_WSTRING_) 'cast(wstring ptr, strptr(str1))
			FOR x = 0 to ncar - 1
				pw1[x] = icode
			NEXT
		#ENDIF
		pw1[ncar] = 0
		pw1[ncar + 1] = _MY_TRAP_NUMBER_                 '**** extra value to check validity
		RETURN DWSTR(Cptr(ubyte ptr , pw1) , ncar , - 1, nSur)
	END FUNCTION
	' ========================================================================================

	' ========================================================================================
	' creates a DWSTR with 1 unicode codepoint (also > FFFF, makes surrogate pair)
	' ========================================================================================
	PRIVATE FUNCTION Dw_Chr(BYVAL U1 AS ULONG) AS DWSTR '****
		if not(IS_FULL_VALID_RANGE(U1)) then return wchr(126)
		dim hi                as Ulong
		dim lo                as Ulong
		DIM AS wstring ptr pw1 = allocate(4 * _MY_SIZE_WSTRING_)
		if pw1 THEN
			#IFDEF __AS__LINUX__
				pw1[0] = U1
				pw1[1] = 0
				pw1[2] = _MY_TRAP_NUMBER_            '**** extra value to check validity
				RETURN DWSTR(Cptr(ubyte ptr , pw1) , 1 )
			#ELSE			' windows
				#IFDEF ___USE___UCS2___ONLY___
					if U1 >= &H10000 then U1 = 126
				#ENDIF
				if U1 >= &H10000 then
					hi = ((U1 - &H10000) / &H400) + &HD800
					lo = ((U1 - &H10000) mod &H400) + &HDC00
					pw1[0] = hi
					pw1[1] = lo
					pw1[2] = 0
					pw1[3] = _MY_TRAP_NUMBER_            '**** extra value to check validity
					RETURN DWSTR(Cptr(ubyte ptr , pw1) , 2 , - 1, 1)
				elseif U1 < &H10000 then
					pw1[0] = U1
					pw1[1] = 0
					pw1[2] = _MY_TRAP_NUMBER_            '**** extra value to check validity
					RETURN DWSTR(Cptr(ubyte ptr , pw1) , 1 )
				end if
			#ENDIF                                     '__AS__LINUX__
		end if
	END FUNCTION

	' ========================================================================================
	' creates a DWSTR using a DWSTR copy with a replaced ucode(unit code) at position counted in unit code not wchar
	' ========================================================================================
	DECLARE FUNCTION Dw_Asc OVERLOAD(BYREF dws1 AS DWSTR , BYVAL ucode AS ULONG , BYVAL position AS LONG ) AS DWSTR
	PRIVATE FUNCTION Dw_Asc(BYREF dws1 as DWSTR , BYVAL ucode AS ULONG , BYVAL position AS LONG ) AS DWSTR '****
		if position < 1 or position > dws1.m_BufferLen THEN return dws1
		if not(IS_FULL_VALID_RANGE(ucode)) then
			dws1.m_pBuffer[position - 1] = 126
			RETURN dws1.m_pBuffer
		end if
		#IFNDEF __AS__LINUX__
			#IFNDEF ___USE___UCS2___ONLY___
				#IFDEF ___STORE___SURROGATE___PAIRS___
					dim nsur as long = dws1.m_SurPair
				#ELSE
					dim nsur as long = dws1.Sur_Count()
				#ENDIF
				if nsur > 0 THEN return *dw_mid(dws1, 1, position - 1) & *Dw_Chr(ucode) & *dw_mid(dws1, position + 1) ' can be improved
			#ELSE
				if ucode >= &H10000 then ucode = 126
			#ENDIF
		#ENDIF
		dws1.m_pBuffer[position - 1] = ucode
		RETURN dws1.m_pBuffer
	END FUNCTION

	' ========================================================================================
	' gets "unicode" code char of a DWSTR at position counted in unit code not wchar
	' ========================================================================================
	PRIVATE FUNCTION Dw_Asc(BYREF dws1 AS DWSTR , BYVAL position AS LONG ) AS ULONG '****
		if position < 1 or position > dws1.m_BufferLen THEN return 0
		#IFNDEF __AS__LINUX__
			#IFNDEF ___USE___UCS2___ONLY___
				dim nsur as long
				dim temp as long
				#IFDEF ___STORE___SURROGATE___PAIRS___
					nsur = dws1.m_SurPair
				#ELSE
					nsur = dws1.Sur_Count()
				#ENDIF
				if nsur > 0 THEN
					temp = position
					do
						nsur = dws1.Sur_Count(1, temp)
						if temp - nsur  =  position THEN exit do
						temp = position + nsur
						if temp > dws1.m_BufferLen THEN RETURN 0
					loop
					position += nsur
					if position > dws1.m_BufferLen THEN RETURN 0
					Dim U1 as ULONG = dws1.m_pBuffer[position - 1]
					if IS_HIGH_UTF16_SUR(U1) then
						Dim U2 as ULONG = dws1.m_pBuffer[position]
						if  U2 = 0 THEN RETURN U1
						if IS_LOW_UTF16_SUR(U2) then
							RETURN ((U1 - &HD800) *&H400) + (U2 - &HDC00) + &H10000
						else
							RETURN U1
						end if
					END IF
				END IF
			#ENDIF
		#ENDIF
		RETURN dws1.m_pBuffer[position - 1]
	END FUNCTION



	' ========================================================================================
	' Get code units  len (take care of surrogate pairs on windows utf-16)
	' ========================================================================================
	PRIVATE FUNCTION Dw_Len(BYREF DWsrc AS DWSTR) AS LONG
		#IFDEF __AS__LINUX__
			Return DWsrc.m_BufferLen
		#ENDIF
		#IFDEF ___USE___UCS2___ONLY___
			Return DWsrc.m_BufferLen
		#ENDIF
		#IFDEF ___STORE___SURROGATE___PAIRS___
			Return(DWsrc.m_BufferLen - DWsrc.m_SurPair)
		#ELSE
			Return(DWsrc.m_BufferLen - DWsrc.Sur_Count())
		#ENDIF
	END FUNCTION

	#IFDEF __AS__LINUX__
		'function to convert DWSTR into string (codepage dependant), using setlocale to modify the conversion behavior
		PRIVATE FUNCTION Dwstr_To_Str2(BYREF DWsrc AS DWSTR , BYREF codepage AS STRING) AS STRING
			dim         as long iflag
			DIM loc0    as string = *setlocale(LC_CTYPE, 0) ' current codepage before , has to be restored if changed
			if instr(ucase(nCodePage) , "UTF-8") > 0 and instr(ucase(loc0) , "UTF-8") > 0
				'do nothing
			elseif ucase(loc0) <> ucase(codepage) THEN 'verify if requested codepage is different than the current one
				setlocale(LC_CTYPE , codepage)          'change to new codepage
				iflag = 1                               ' flag to remember to restore after
			END IF
			function = str(*DWsrc.m_pBuffer)           'make conversion
			if iflag = 1 then setlocale(LC_CTYPE , loc0) 'restore the previous codepage
		END FUNCTION


		'function to convert zstring ptr into wstring ptr (codepage dependant), using setlocale to modify the conversion behavior
		PRIVATE FUNCTION MbsToWcs_2(BYREF codepage AS STRING , BYVAL dest AS WSTRING PTR , BYVAL src AS ZSTRING PTR , BYVAL nb AS LONG) AS LONG
			if nb < 0 or dest = NULL or src = NULL THEN RETURN - 1
			if nb = 0 THEN return 0

			dim         as long iflag
			DIM loc0    as string = *setlocale(LC_CTYPE, 0) ' current codepage before , has to be restored if changed
			if instr(ucase(nCodePage) , "UTF-8") > 0 and instr(ucase(loc0) , "UTF-8") > 0
				'do nothing
			elseif ucase(loc0) <> ucase(codepage) THEN 'verify if requested codepage is different than the current one
				setlocale(LC_CTYPE , codepage)          'change to new codepage
				iflag = 1                               ' flag to remember to restore after
			end if
			MbsToWcs(dest , src , nb)                  'make conversion
			if iflag = 1 then setlocale(LC_CTYPE , loc0) 'restore the previous codepage
			RETURN nb
		END FUNCTION

		'function to convert string to utf8-string, with optional codepage for input string
		PRIVATE FUNCTION StrToUtf8(BYREF st1 AS STRING , BYREF codePage as STRING = "") AS STRING
			dim len1 as long = len(st1)
			if len1 = 0 then return ""

			if codePage = "" or instr(ucase(CodePage) , "UTF-8") > 0 THEN 'assuming default is utf8
				return st1
			else
				return Dw_Str(Dw_Wstr(st1 , codePage))
			END IF
		END FUNCTION

		'function to convert utf8-string to string, with optional codepage for output string
		PRIVATE FUNCTION Utf8ToStr(BYREF st1 AS STRING , BYREF codePage as STRING = "") AS STRING
			if st1 = "" or codepage = "" or instr(ucase(CodePage) , "UTF-8") > 0 THEN return st1
			return Dw_Str(dw_wstr(st1) , codePage)
		END FUNCTION

		'function to convert utf8-string to wstring
		PRIVATE FUNCTION Utf8ToWstr(BYREF st1 AS STRING) BYREF AS WSTRING
			return *dw_wstr(st1 , "")                   'convert to dwstr according UTF8,
		END FUNCTION

		'function to convert wstring to utf8-string
		PRIVATE FUNCTION WstrToUtf8(BYREF wst1 AS WSTRING) AS STRING
			return dw_str(wst1 , "")                   'convert to dwstr according UTF8,
		END FUNCTION

		'function to convert string to utf32-wstring, with optional codepage for input string ; utf32 little endian
		PRIVATE FUNCTION StrToUtf32(BYREF st1 AS STRING , BYREF codePage AS STRING = "") BYREF AS WSTRING
			if st1 = "" then return wstr( "")
			'convert to dwstr according the specified codepage and dereference pointer buffer
			return *Dw_Wstr(st1 , codePage)
		END FUNCTION

		'function to convert utf32-wstring to string, with optional codepage for output string ; utf32 little endian
		PRIVATE FUNCTION Utf32ToStr(BYREF wst1 AS WSTRING , BYVAL codePage AS STRING = "") AS STRING
			if wst1 = "" THEN return ""
			return Dw_Str(wst1 , codePage)             'convert to string using codepage
		END FUNCTION

		'function to convert string to String, with codepage for input string and optional codepage for output string
		PRIVATE FUNCTION StrToStr(BYREF st1 AS STRING , BYVAL codeIn AS STRING , BYVAL codeOut AS STRING = "") AS STRING
			if st1 = "" then return ""
			if ucase(codeIn) = ucase(codeOut) then return st1
			if instr(ucase(codeIn) , "UTF-8") > 0 and instr(ucase(codeOut) , "UTF-8") > 0 THEN return st1
			if instr(ucase(codeIn) , "UTF-8") > 0 and codeOut = "" THEN return st1
			if instr(ucase(codeOut) , "UTF-8") > 0 and codeIn = "" THEN return st1
			If ucase(mid(codeIn , instr(codeIn , "."))) = ucase(mid(codeOut , instr(codeOut , "."))) THEN return st1
			return Dw_Str(Dw_Wstr(st1 , codeIn) , codeOut) 'convert to dwstr using codeIn and convert back to string using codeOut
		END FUNCTION
	#ELSE

		'function to convert string to utf8-string, with optional codepage for input string
		PRIVATE FUNCTION StrToUtf8(BYREF st1 AS STRING , BYVAL codePage AS LONG = 0) AS STRING
			dim len1 as long = len(st1)
			if len1 = 0 then return ""

			if codePage = 0 THEN                       'use system codepage
				dim as string dest = string(len1 * 5 , 0)
				dim         as integer i1
				CharToUTF(1 , strptr(st1) , len1 , strptr(dest) , @i1)
				return dest
			elseif codePage = CP_UTF8 THEN             'already utf8 coded
				return st1
			else                                       'convert to dwstr according the specified codepage, convert back to string utf8 coded
				return Dw_Str(Dw_Wstr(st1 , codePage) , CP_UTF8)
			END IF
		END FUNCTION

		'function to convert utf8-string to string, with optional codepage for output string
		PRIVATE FUNCTION Utf8ToStr(BYREF st1 AS STRING , BYVAL codePage AS LONG = 0) AS STRING
			if st1 = "" or codepage = CP_UTF8 THEN return st1
			return Dw_Str(dw_wstr(st1 , CP_UTF8) , codePage) 'convert to dwstr according CP_UTF8, convert back to string using codepage
		END FUNCTION

		'function to convert utf8-string to wstring
		PRIVATE FUNCTION Utf8ToWstr(BYREF st1 AS STRING) BYREF AS WSTRING
			return *dw_wstr(st1 , CP_UTF8)              'convert to dwstr according CP_UTF8,
		END FUNCTION

		'function to convert wstring to utf8-string
		PRIVATE FUNCTION WstrToUtf8(BYREF wst1 AS WSTRING) AS STRING
			return dw_str(wst1 , CP_UTF8)              'convert to dwstr according CP_UTF8,
		END FUNCTION

		'function to convert string to utf16-wstring, with optional codepage for input string ; utf16 little endian
		PRIVATE FUNCTION StrToUtf16(BYREF st1 AS STRING , BYVAL codePage AS LONG = 0) BYREF AS WSTRING
			if st1 = "" then return wstr( "")
			'convert to dwstr according the specified codepage and dereference pointer buffer
			return *Dw_Wstr(st1 , codePage)
		END FUNCTION

		'function to convert utf16-wstring to string, with optional codepage for output string ; utf16 little endian
		PRIVATE FUNCTION Utf16ToStr(BYREF wst1 AS WSTRING , BYVAL codePage AS LONG = 0) AS STRING
			if wst1 = "" THEN return ""
			return Dw_Str(wst1 , codePage)             'convert to string using codepage
		END FUNCTION

		'function to convert string to String, with codepage for input string and optional codepage for output string
		PRIVATE FUNCTION StrToStr(BYREF st1 AS STRING , BYVAL codeIn AS LONG , BYVAL codeOut AS LONG = 0) AS STRING
			if st1 = "" then return ""
			if codeIn = codeOut then return st1
			return Dw_Str(Dw_Wstr(st1 , codeIn) , codeOut) 'convert to dwstr using codeIn and convert back to string using codeOut
		END FUNCTION

	#ENDIF                                           'IFDEF __AS__LINUX__

	''Write wstring to utf8 file with BOM ::::: ok for win and linux, if rewrite <>1 does not replace existing file
	PRIVATE FUNCTION Dw_Write_Utf8_File(BYREF file AS STRING , BYREF content AS WSTRING , BYVAL rewrite AS LONG = 1) AS LONG
		dim         as long x
		dim         as long y
		dim         as long z
		dim as long f1 = freefile
		dim         as ubyte by1
		dim as string spc1 = WstrToUtf8(content)
		x = len(spc1)
		If x > 0 Then
			y = open(file for binary Access Read as #f1)
			if y = 0 then
				z = LOF(f1)
				close #f1
			end if
			if rewrite = 1 and y = 0 then
				kill file
			elseif rewrite <> 1 and y = 0 and z > 0 then
				return - 3                              'file already exists and not replaced
			elseif y = 0 then
				kill file
			end if
			f1 = freefile
			if open(file for binary as #f1) = 0 then
				'put BOM
				by1 = &hEF                              '239
				Put #f1 , 1 , by1 , 1
				by1 = &hBB                              '187
				Put #f1 , 2 , by1 , 1
				by1 = &hBF                              '191
				Put #f1 , 3 , by1 , 1
				Put #f1 , 4 , spc1
				Close #f1
			else
				return - 2                              'cannot create file
			end if
		else
			return - 1                                 'empty string nothing done
		end if
		return 0                                      'file saved, replaced existing or new
	END FUNCTION

	'read utf8 file with or without BOM and convert to wstring ::::: ok for win and linux
	PRIVATE FUNCTION Dw_Read_Utf8_File(BYREF file AS STRING) BYREF AS WSTRING
		dim         as long x
		dim as long f1 = freefile
		dim         as string stemp
		if open(file for binary Access Read as #f1) = 0 then
			x = LOF(f1)
			If x > 0 Then
				stemp = String(x , 0)
				Get #f1 , , stemp
			else
				Close #f1
				return wstr("")
			End If
			Close #f1
		else
			return wstr("")
		end if
		'BOM: &hEF ; &hBB ; &hBF
		if asc(left(stemp , 1)) = 239 and asc(mid(stemp , 2 , 1)) = 187 and asc(mid(stemp , 3 , 1)) = 191 THEN _
				stemp = mid(stemp , 4)
		if len(stemp) > 0 THEN return Utf8ToWstr(stemp)
		return wstr("")
	END FUNCTION

	' ========================================================================================
	' clean surrogate pairs in  wstring chain, needed for windows unicode ucs2 option, replaced by '~'
	' ========================================================================================
	PRIVATE SUB dw_CleanSur(BYREF DWsrc AS DWSTR)
		#IFNDEF __AS__LINUX__
			if DWsrc.m_BufferLen = 0 THEN return
			dim as long x
			dim as long y
			DIM pNewBuffer AS WSTRING PTR = allocate((DWsrc.m_BufferLen + 1) *_MY_SIZE_WSTRING_)
			Do While DWsrc.m_BufferLen > x
				IF IS_HIGH_UTF16_SUR(DWsrc.m_pBuffer[x]) THEN
					pNewBuffer[y]= 126
					y += 1
					if x + 1 = DWsrc.m_BufferLen THEN exit do
					x += 1
					if IS_LOW_UTF16_SUR(DWsrc.m_pBuffer[x]) then x += 1
				ELSEIF IS_LOW_UTF16_SUR(DWsrc.m_pBuffer[x]) THEN
					pNewBuffer[y] = 126
					y += 1
					x += 1
				ELSE
					pNewBuffer[y] = DWsrc.m_pBuffer[x]
					y += 1
					x += 1
				END IF
			LOOP
			pNewBuffer[y]= 0
			deallocate DWsrc.m_pBuffer
			DWsrc.m_pBuffer = pNewBuffer
			DWsrc.m_BufferLen = y
			DWsrc.m_SurPair = 0
		#ENDIF
	END SUB
	' ========================================================================================

	' ========================================================================================
	' Correct false surrogate pairs in  wstring chain, needed for windows, if wrong surrogate replaced by '~'
	' correct bad unit code also for LINUX if out of range replaced by '~'
	' ========================================================================================
	PRIVATE SUB Dw_ValidUnit(BYREF DWsrc AS DWSTR)
		if DWsrc.m_BufferLen = 0 THEN return
		dim as long x
		#IFNDEF __AS__LINUX__
			dim as long y
			dim as long sur
			DIM pNewBuffer AS WSTRING PTR = allocate((DWsrc.m_BufferLen + 1) *_MY_SIZE_WSTRING_)
			Do While DWsrc.m_BufferLen > x
				if not IS_FULL_VALID_RANGE(DWsrc.m_pBuffer[x])THEN
					DWsrc.m_pBuffer[x] = 126
					x += 1
				ELSEIF IS_HIGH_UTF16_SUR(DWsrc.m_pBuffer[x]) THEN
					if x + 1 = DWsrc.m_BufferLen then
						pNewBuffer[y] = 126
						y += 1
						exit do
					end if
					if IS_LOW_UTF16_SUR(DWsrc.m_pBuffer[x + 1]) then
						pNewBuffer[y]= DWsrc.m_pBuffer[x]
						pNewBuffer[y + 1]= DWsrc.m_pBuffer[x + 1]
						y += 2
						x += 2
						sur +=1
					else
						pNewBuffer[y] = 126
						y += 1
						x += 1
					end if
				ELSEIF IS_LOW_UTF16_SUR(DWsrc.m_pBuffer[x]) THEN
					pNewBuffer[y] = 126
					y += 1
					x += 1
				ELSEIF not IS_LIMITED_VALID_RANGE(DWsrc.m_pBuffer[x])THEN
					DWsrc.m_pBuffer[x] = 126
					x += 1
				ELSE
					pNewBuffer[y] = DWsrc.m_pBuffer[x]
					y += 1
					x += 1
				END IF
			LOOP
			pNewBuffer[y]= 0
			deallocate DWsrc.m_pBuffer
			DWsrc.m_pBuffer = pNewBuffer
			DWsrc.m_BufferLen = y
			#IFDEF ___STORE___SURROGATE___PAIRS___
				DWsrc.m_SurPair = sur
			#ELSE
				DWsrc.m_SurPair = 0
			#ENDIF
		#ELSE
			Do While DWsrc.m_BufferLen > x
				if not IS_FULL_VALID_RANGE(DWsrc.m_pBuffer[x])THEN
					DWsrc.m_pBuffer[x] = 126
            END IF
				x += 1
			loop
		#ENDIF
	END SUB
	' ========================================================================================

	' ========================================================================================
	' len and Count  the nb of surrogate pairs in  wstring chain, needed for windows unicode utf16
	' ========================================================================================
	PRIVATE FUNCTION Dw_LenSur(BYVAL pwszStr AS WSTRING PTR, BYREF Sur as LONG) AS LONG
		Sur = 0
		if pwszStr = NULL THEN return 0
		dim as long x
		Do While pwszStr[x] > 0
			#IFNDEF __AS__LINUX__
				#IFNDEF ___USE___UCS2___ONLY___
					if IS_HIGH_UTF16_SUR(pwszStr[x]) then
						if pwszStr[x + 1] = 0 THEN
							x += 1
							exit do
						end if
						x += 1
						if IS_LOW_UTF16_SUR(pwszStr[x]) then Sur += 1
					END IF
				#ENDIF
			#ENDIF
			x += 1
		LOOP
		return x
	END FUNCTION
	' ========================================================================================

	' ========================================================================================
	' Creates a DWSTR with concatenation nMul times of specific wst WSTRING
	' ========================================================================================
	PRIVATE FUNCTION Dw_Repeat(BYREF wst as WSTRING, BYVAL nMul AS LONG = 1)AS DWSTR  '****
		IF wst = wstr("") or nMul < 1 THEN RETURN DWSTR(0)  'use capacity constructor just to allocate mem
		dim as long x
		dim as dwstr dw1 = wst
		if nMul > 2 then
			FOR x = 2 to nMul
				dw1 &= wst
			NEXT
      END IF
		RETURN dw1
	END FUNCTION
	' ========================================================================================

	' ========================================================================================
	' search first occurence position substring dwstr into dwstr , start is counted in unit code not wchar, return in code unit too
	' if start < 0 , it is counted from the end ; e.g.  dw_Instr(-1, dwstr, "a") give the position of 'a' , but -1 is saying search from the last
	' if make in fact similar function as InStrRev, the returned position in code units counted from the beginning.
	' ========================================================================================
	PRIVATE FUNCTION Dw_INSTR ( BYVAL start AS LONG, BYREF dwstr1 AS DWSTR, BYREF sub1 AS DWSTR ) AS LONG
		if dwstr1.m_BufferLen = 0 or sub1.m_BufferLen = 0 or start = 0 or start > dwstr1.m_BufferLen _
				or start < (dwstr1.m_BufferLen * -1) or dwstr1.m_BufferLen < sub1.m_BufferLen then return 0
		#IFNDEF __AS__LINUX__
			#IFNDEF ___USE___UCS2___ONLY___
				dim temp as long
				#IFDEF ___STORE___SURROGATE___PAIRS___
					dim nsur as long = dwstr1.m_SurPair
				#ELSE
					dim nsur as long = dwstr1.Sur_Count()
				#ENDIF
				if nsur > 0 THEN
					dim as long position
					if start > 0 THEN
						temp = start
						do
							nsur = dwstr1.Sur_Count(1, temp)
							if temp - nsur  =  start THEN exit do
							temp = start + nsur
							if temp > dwstr1.m_BufferLen THEN return 0
						loop
						start += nsur
						if start > dwstr1.m_BufferLen THEN return 0
						position = instr(start, *dwstr1, *sub1)
					else
						start *= -1
						temp = start
						do
							nsur = dwstr1.Sur_CountRev(1, temp)
							if temp - nsur  =  start THEN exit do
							temp = start + nsur
							if temp > dwstr1.m_BufferLen THEN return 0
						loop
						start += nsur
						if start > dwstr1.m_BufferLen THEN return 0
						position = InStrRev(*dwstr1, *sub1, len(dwstr1) - start + 1)
					END IF
					nsur = dwstr1.Sur_Count(1, position)
					position -= nsur
					return position
				End if
			#ENDIF
		#ENDIF
		if start > 0 THEN
			return instr(start, *dwstr1, *sub1)
		else
			return InStrRev(*dwstr1, *sub1, len(dwstr1) + start + 1)
      END IF

	END FUNCTION

'	PRIVATE OPERATOR &(BYREF ust1 AS DWSTR , BYREF st2 AS STRING) AS DWSTR
'		DWSTR_DP( "OPERATOR & DWSTR :  DWSTR & WSTRING")
'		DIM AS LONG l2 = len(st2)
'		DIM pNewBuffer AS WSTRING PTR =  allocate((ust1.m_BufferLen + l2 + 2) *_MY_SIZE_WSTRING_)
'		IF pNewBuffer THEN
'			memcpy(pNewBuffer , ust1.m_pBuffer , ust1.m_BufferLen * _MY_SIZE_WSTRING_)
'			memcpy(pNewBuffer + ust1.m_BufferLen , -Dw_Wstr(st2) , l2 * _MY_SIZE_WSTRING_)
'			pNewBuffer[ust1.m_BufferLen + l2] = 0
'			pNewBuffer[ust1.m_BufferLen + l2 + 1] = _MY_TRAP_NUMBER_ '****extra value to check validity
'			RETURN DWSTR(cptr(ubyte ptr , pNewBuffer) , ust1.m_BufferLen + l2 , - 1, ust1.m_SurPair )
'		END IF
'		RETURN ""
'	END OPERATOR

'	PRIVATE OPERATOR &(BYREF st1 AS STRING , BYREF ust2 AS DWSTR) AS DWSTR
'		DWSTR_DP( "OPERATOR & DWSTR :  DWSTR & WSTRING")
'		DIM AS LONG l1 = len(st1)
'		DIM pNewBuffer AS WSTRING PTR =  allocate((ust2.m_BufferLen + l1 + 2) *_MY_SIZE_WSTRING_)
'		IF pNewBuffer THEN
'			memcpy(pNewBuffer , -Dw_Wstr(st1), l1 * _MY_SIZE_WSTRING_)
'			memcpy(pNewBuffer + l1, ust2.m_pBuffer , ust2.m_BufferLen * _MY_SIZE_WSTRING_)
'			pNewBuffer[ust2.m_BufferLen + l1] = 0
'			pNewBuffer[ust2.m_BufferLen + l1 + 1] = _MY_TRAP_NUMBER_ '****extra value to check validity
'			RETURN DWSTR(cptr(ubyte ptr , pNewBuffer) , ust2.m_BufferLen + l1 , - 1, ust2.m_SurPair )
'		END IF
'		RETURN ""
'	END OPERATOR

	' ========================================================================================

#ENDIF   'IFNDEF __FB_DOS__


