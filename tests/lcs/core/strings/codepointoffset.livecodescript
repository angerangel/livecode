script "CoreStringCodepointOffset"
/*
Copyright (C) 2016 LiveCode Ltd.

This file is part of LiveCode.

LiveCode is free software; you can redistribute it and/or modify it under
the terms of the GNU General Public License v3 as published by the Free
Software Foundation.

LiveCode is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or
FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
for more details.

You should have received a copy of the GNU General Public License
along with LiveCode.  If not see <http://www.gnu.org/licenses/>.  */

on TestCodepointOffsetSingleUnicodeChar
   local tNativeNeedle, tUnicodeNeedle, tNativeHaystack, tUnicodeHaystack
   
   put "B" into tNativeNeedle
   put numToCodepoint(0x391) into tUnicodeNeedle
   
   put "aAbBcC" into tNativeHaystack
   put "aAbBcC" & numToCodepoint(0x3B1) & numToCodepoint(0x391) into tUnicodeHaystack

   set the caseSensitive to true
   TestAssert "codepoint offset of native needle in unicode string - case-sensitive", codepointOffset(tNativeNeedle, tUnicodeHaystack) is 4
   TestAssert "codepoint offset of unicode needle in unicode string - case-sensitive", codepointOffset(tUnicodeNeedle, tUnicodeHaystack) is 8
   TestAssert "codepoint offset of native needle in native string - case-sensitive", codepointOffset(tNativeNeedle, tNativeHaystack) is 4

   set the caseSensitive to false
   TestAssert "codepoint offset of native needle in unicode string - caseless", codepointOffset(tNativeNeedle, tUnicodeHaystack) is 3
   TestAssert "codepoint offset of unicode needle in unicode string - caseless", codepointOffset(tUnicodeNeedle, tUnicodeHaystack) is 7
   TestAssert "codepoint offset of native needle in native string - caseless", codepointOffset(tNativeNeedle, tNativeHaystack) is 3
end TestCodepointOffsetSingleUnicodeChar
