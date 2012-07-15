/* See COPYRIGHT for copyright information. */

#include <inc/mmu.h>
#include <inc/memlayout.h>
#include <inc/trap.h>



###################################################################
# exceptions/interrupts
###################################################################

/* TRAPHANDLER defines a globally-visible function for handling a trap.
 * It pushes a trap number onto the stack, then jumps to _alltraps.
 * Use TRAPHANDLER for traps where the CPU automatically pushes an error code.
 *
 * You shouldn't call a TRAPHANDLER function from C, but you may
 * need to _declare_ one in C (for instance, to get a function pointer
 * during IDT setup).  You can declare the function with
 *   void NAME();
 * where NAME is the argument passed to TRAPHANDLER.
 */
/*
   num is the trap number.
 */
#define TRAPHANDLER(name, num)						\
	.globl name;		/* define global symbol for 'name' */	\
	.type name, @function;	/* symbol type is function */		\
	.align 2;		/* align function definition */		\
	name:			/* function starts here */		\
	pushl $(num);							\
	jmp _alltraps

/* Use TRAPHANDLER_NOEC for traps where the CPU doesn't push an error code.
 * It pushes a 0 in place of the error code, so the trap frame has the same
 * format in either case.
 */
#define TRAPHANDLER_NOEC(name, num)					\
	.globl name;							\
	.type name, @function;						\
	.align 2;							\
	name:								\
	pushl $0;							\
	pushl $(num);							\
	jmp _alltraps

/*
 * Lab 3: Your code here for generating entry points for the different traps.
 */
.text
TRAPHANDLER_NOEC(vect0 , 0)
TRAPHANDLER_NOEC(vect1 , 1)
TRAPHANDLER_NOEC(vect2 , 2)
TRAPHANDLER_NOEC(vect3 , 3)
TRAPHANDLER_NOEC(vect4 , 4)
TRAPHANDLER_NOEC(vect5 , 5)
TRAPHANDLER_NOEC(vect6 , 6)
TRAPHANDLER_NOEC(vect7 , 7)
TRAPHANDLER(vect8 , 8)
TRAPHANDLER_NOEC(vect9 , 9)
TRAPHANDLER(vect10 , 10)
TRAPHANDLER(vect11 , 11)
TRAPHANDLER(vect12 , 12)
TRAPHANDLER(vect13 , 13)
TRAPHANDLER(vect14 , 14)
TRAPHANDLER_NOEC(vect15 , 15)
TRAPHANDLER_NOEC(vect16 , 16)
TRAPHANDLER_NOEC(vect17 , 17)
TRAPHANDLER_NOEC(vect18 , 18)
TRAPHANDLER_NOEC(vect19 , 19)
TRAPHANDLER_NOEC(vect20 , 20)
TRAPHANDLER_NOEC(vect21 , 21)
TRAPHANDLER_NOEC(vect22 , 22)
TRAPHANDLER_NOEC(vect23 , 23)
TRAPHANDLER_NOEC(vect24 , 24)
TRAPHANDLER_NOEC(vect25 , 25)
TRAPHANDLER_NOEC(vect26 , 26)
TRAPHANDLER_NOEC(vect27 , 27)
TRAPHANDLER_NOEC(vect28 , 28)
TRAPHANDLER_NOEC(vect29 , 29)
TRAPHANDLER_NOEC(vect30 , 30)
TRAPHANDLER_NOEC(vect31 , 31)
TRAPHANDLER_NOEC(vect32 , 32)
TRAPHANDLER_NOEC(vect33 , 33)
TRAPHANDLER_NOEC(vect34 , 34)
TRAPHANDLER_NOEC(vect35 , 35)
TRAPHANDLER_NOEC(vect36 , 36)
TRAPHANDLER_NOEC(vect37 , 37)
TRAPHANDLER_NOEC(vect38 , 38)
TRAPHANDLER_NOEC(vect39 , 39)
TRAPHANDLER_NOEC(vect40 , 40)
TRAPHANDLER_NOEC(vect41 , 41)
TRAPHANDLER_NOEC(vect42 , 42)
TRAPHANDLER_NOEC(vect43 , 43)
TRAPHANDLER_NOEC(vect44 , 44)
TRAPHANDLER_NOEC(vect45 , 45)
TRAPHANDLER_NOEC(vect46 , 46)
TRAPHANDLER_NOEC(vect47 , 47)
TRAPHANDLER_NOEC(vect48 , 48)
TRAPHANDLER_NOEC(vect49 , 49)
TRAPHANDLER_NOEC(vect50 , 50)
TRAPHANDLER_NOEC(vect51 , 51)
TRAPHANDLER_NOEC(vect52 , 52)
TRAPHANDLER_NOEC(vect53 , 53)
TRAPHANDLER_NOEC(vect54 , 54)
TRAPHANDLER_NOEC(vect55 , 55)
TRAPHANDLER_NOEC(vect56 , 56)
TRAPHANDLER_NOEC(vect57 , 57)
TRAPHANDLER_NOEC(vect58 , 58)
TRAPHANDLER_NOEC(vect59 , 59)
TRAPHANDLER_NOEC(vect60 , 60)
TRAPHANDLER_NOEC(vect61 , 61)
TRAPHANDLER_NOEC(vect62 , 62)
TRAPHANDLER_NOEC(vect63 , 63)
TRAPHANDLER_NOEC(vect64 , 64)
TRAPHANDLER_NOEC(vect65 , 65)
TRAPHANDLER_NOEC(vect66 , 66)
TRAPHANDLER_NOEC(vect67 , 67)
TRAPHANDLER_NOEC(vect68 , 68)
TRAPHANDLER_NOEC(vect69 , 69)
TRAPHANDLER_NOEC(vect70 , 70)
TRAPHANDLER_NOEC(vect71 , 71)
TRAPHANDLER_NOEC(vect72 , 72)
TRAPHANDLER_NOEC(vect73 , 73)
TRAPHANDLER_NOEC(vect74 , 74)
TRAPHANDLER_NOEC(vect75 , 75)
TRAPHANDLER_NOEC(vect76 , 76)
TRAPHANDLER_NOEC(vect77 , 77)
TRAPHANDLER_NOEC(vect78 , 78)
TRAPHANDLER_NOEC(vect79 , 79)
TRAPHANDLER_NOEC(vect80 , 80)
TRAPHANDLER_NOEC(vect81 , 81)
TRAPHANDLER_NOEC(vect82 , 82)
TRAPHANDLER_NOEC(vect83 , 83)
TRAPHANDLER_NOEC(vect84 , 84)
TRAPHANDLER_NOEC(vect85 , 85)
TRAPHANDLER_NOEC(vect86 , 86)
TRAPHANDLER_NOEC(vect87 , 87)
TRAPHANDLER_NOEC(vect88 , 88)
TRAPHANDLER_NOEC(vect89 , 89)
TRAPHANDLER_NOEC(vect90 , 90)
TRAPHANDLER_NOEC(vect91 , 91)
TRAPHANDLER_NOEC(vect92 , 92)
TRAPHANDLER_NOEC(vect93 , 93)
TRAPHANDLER_NOEC(vect94 , 94)
TRAPHANDLER_NOEC(vect95 , 95)
TRAPHANDLER_NOEC(vect96 , 96)
TRAPHANDLER_NOEC(vect97 , 97)
TRAPHANDLER_NOEC(vect98 , 98)
TRAPHANDLER_NOEC(vect99 , 99)
TRAPHANDLER_NOEC(vect100 , 100)
TRAPHANDLER_NOEC(vect101 , 101)
TRAPHANDLER_NOEC(vect102 , 102)
TRAPHANDLER_NOEC(vect103 , 103)
TRAPHANDLER_NOEC(vect104 , 104)
TRAPHANDLER_NOEC(vect105 , 105)
TRAPHANDLER_NOEC(vect106 , 106)
TRAPHANDLER_NOEC(vect107 , 107)
TRAPHANDLER_NOEC(vect108 , 108)
TRAPHANDLER_NOEC(vect109 , 109)
TRAPHANDLER_NOEC(vect110 , 110)
TRAPHANDLER_NOEC(vect111 , 111)
TRAPHANDLER_NOEC(vect112 , 112)
TRAPHANDLER_NOEC(vect113 , 113)
TRAPHANDLER_NOEC(vect114 , 114)
TRAPHANDLER_NOEC(vect115 , 115)
TRAPHANDLER_NOEC(vect116 , 116)
TRAPHANDLER_NOEC(vect117 , 117)
TRAPHANDLER_NOEC(vect118 , 118)
TRAPHANDLER_NOEC(vect119 , 119)
TRAPHANDLER_NOEC(vect120 , 120)
TRAPHANDLER_NOEC(vect121 , 121)
TRAPHANDLER_NOEC(vect122 , 122)
TRAPHANDLER_NOEC(vect123 , 123)
TRAPHANDLER_NOEC(vect124 , 124)
TRAPHANDLER_NOEC(vect125 , 125)
TRAPHANDLER_NOEC(vect126 , 126)
TRAPHANDLER_NOEC(vect127 , 127)
TRAPHANDLER_NOEC(vect128 , 128)
TRAPHANDLER_NOEC(vect129 , 129)
TRAPHANDLER_NOEC(vect130 , 130)
TRAPHANDLER_NOEC(vect131 , 131)
TRAPHANDLER_NOEC(vect132 , 132)
TRAPHANDLER_NOEC(vect133 , 133)
TRAPHANDLER_NOEC(vect134 , 134)
TRAPHANDLER_NOEC(vect135 , 135)
TRAPHANDLER_NOEC(vect136 , 136)
TRAPHANDLER_NOEC(vect137 , 137)
TRAPHANDLER_NOEC(vect138 , 138)
TRAPHANDLER_NOEC(vect139 , 139)
TRAPHANDLER_NOEC(vect140 , 140)
TRAPHANDLER_NOEC(vect141 , 141)
TRAPHANDLER_NOEC(vect142 , 142)
TRAPHANDLER_NOEC(vect143 , 143)
TRAPHANDLER_NOEC(vect144 , 144)
TRAPHANDLER_NOEC(vect145 , 145)
TRAPHANDLER_NOEC(vect146 , 146)
TRAPHANDLER_NOEC(vect147 , 147)
TRAPHANDLER_NOEC(vect148 , 148)
TRAPHANDLER_NOEC(vect149 , 149)
TRAPHANDLER_NOEC(vect150 , 150)
TRAPHANDLER_NOEC(vect151 , 151)
TRAPHANDLER_NOEC(vect152 , 152)
TRAPHANDLER_NOEC(vect153 , 153)
TRAPHANDLER_NOEC(vect154 , 154)
TRAPHANDLER_NOEC(vect155 , 155)
TRAPHANDLER_NOEC(vect156 , 156)
TRAPHANDLER_NOEC(vect157 , 157)
TRAPHANDLER_NOEC(vect158 , 158)
TRAPHANDLER_NOEC(vect159 , 159)
TRAPHANDLER_NOEC(vect160 , 160)
TRAPHANDLER_NOEC(vect161 , 161)
TRAPHANDLER_NOEC(vect162 , 162)
TRAPHANDLER_NOEC(vect163 , 163)
TRAPHANDLER_NOEC(vect164 , 164)
TRAPHANDLER_NOEC(vect165 , 165)
TRAPHANDLER_NOEC(vect166 , 166)
TRAPHANDLER_NOEC(vect167 , 167)
TRAPHANDLER_NOEC(vect168 , 168)
TRAPHANDLER_NOEC(vect169 , 169)
TRAPHANDLER_NOEC(vect170 , 170)
TRAPHANDLER_NOEC(vect171 , 171)
TRAPHANDLER_NOEC(vect172 , 172)
TRAPHANDLER_NOEC(vect173 , 173)
TRAPHANDLER_NOEC(vect174 , 174)
TRAPHANDLER_NOEC(vect175 , 175)
TRAPHANDLER_NOEC(vect176 , 176)
TRAPHANDLER_NOEC(vect177 , 177)
TRAPHANDLER_NOEC(vect178 , 178)
TRAPHANDLER_NOEC(vect179 , 179)
TRAPHANDLER_NOEC(vect180 , 180)
TRAPHANDLER_NOEC(vect181 , 181)
TRAPHANDLER_NOEC(vect182 , 182)
TRAPHANDLER_NOEC(vect183 , 183)
TRAPHANDLER_NOEC(vect184 , 184)
TRAPHANDLER_NOEC(vect185 , 185)
TRAPHANDLER_NOEC(vect186 , 186)
TRAPHANDLER_NOEC(vect187 , 187)
TRAPHANDLER_NOEC(vect188 , 188)
TRAPHANDLER_NOEC(vect189 , 189)
TRAPHANDLER_NOEC(vect190 , 190)
TRAPHANDLER_NOEC(vect191 , 191)
TRAPHANDLER_NOEC(vect192 , 192)
TRAPHANDLER_NOEC(vect193 , 193)
TRAPHANDLER_NOEC(vect194 , 194)
TRAPHANDLER_NOEC(vect195 , 195)
TRAPHANDLER_NOEC(vect196 , 196)
TRAPHANDLER_NOEC(vect197 , 197)
TRAPHANDLER_NOEC(vect198 , 198)
TRAPHANDLER_NOEC(vect199 , 199)
TRAPHANDLER_NOEC(vect200 , 200)
TRAPHANDLER_NOEC(vect201 , 201)
TRAPHANDLER_NOEC(vect202 , 202)
TRAPHANDLER_NOEC(vect203 , 203)
TRAPHANDLER_NOEC(vect204 , 204)
TRAPHANDLER_NOEC(vect205 , 205)
TRAPHANDLER_NOEC(vect206 , 206)
TRAPHANDLER_NOEC(vect207 , 207)
TRAPHANDLER_NOEC(vect208 , 208)
TRAPHANDLER_NOEC(vect209 , 209)
TRAPHANDLER_NOEC(vect210 , 210)
TRAPHANDLER_NOEC(vect211 , 211)
TRAPHANDLER_NOEC(vect212 , 212)
TRAPHANDLER_NOEC(vect213 , 213)
TRAPHANDLER_NOEC(vect214 , 214)
TRAPHANDLER_NOEC(vect215 , 215)
TRAPHANDLER_NOEC(vect216 , 216)
TRAPHANDLER_NOEC(vect217 , 217)
TRAPHANDLER_NOEC(vect218 , 218)
TRAPHANDLER_NOEC(vect219 , 219)
TRAPHANDLER_NOEC(vect220 , 220)
TRAPHANDLER_NOEC(vect221 , 221)
TRAPHANDLER_NOEC(vect222 , 222)
TRAPHANDLER_NOEC(vect223 , 223)
TRAPHANDLER_NOEC(vect224 , 224)
TRAPHANDLER_NOEC(vect225 , 225)
TRAPHANDLER_NOEC(vect226 , 226)
TRAPHANDLER_NOEC(vect227 , 227)
TRAPHANDLER_NOEC(vect228 , 228)
TRAPHANDLER_NOEC(vect229 , 229)
TRAPHANDLER_NOEC(vect230 , 230)
TRAPHANDLER_NOEC(vect231 , 231)
TRAPHANDLER_NOEC(vect232 , 232)
TRAPHANDLER_NOEC(vect233 , 233)
TRAPHANDLER_NOEC(vect234 , 234)
TRAPHANDLER_NOEC(vect235 , 235)
TRAPHANDLER_NOEC(vect236 , 236)
TRAPHANDLER_NOEC(vect237 , 237)
TRAPHANDLER_NOEC(vect238 , 238)
TRAPHANDLER_NOEC(vect239 , 239)
TRAPHANDLER_NOEC(vect240 , 240)
TRAPHANDLER_NOEC(vect241 , 241)
TRAPHANDLER_NOEC(vect242 , 242)
TRAPHANDLER_NOEC(vect243 , 243)
TRAPHANDLER_NOEC(vect244 , 244)
TRAPHANDLER_NOEC(vect245 , 245)
TRAPHANDLER_NOEC(vect246 , 246)
TRAPHANDLER_NOEC(vect247 , 247)
TRAPHANDLER_NOEC(vect248 , 248)
TRAPHANDLER_NOEC(vect249 , 249)
TRAPHANDLER_NOEC(vect250 , 250)
TRAPHANDLER_NOEC(vect251 , 251)
TRAPHANDLER_NOEC(vect252 , 252)
TRAPHANDLER_NOEC(vect253 , 253)
TRAPHANDLER_NOEC(vect254 , 254)
TRAPHANDLER_NOEC(vect255 , 255)

.data
.globl vects
vects:
  .long vect0
  .long vect1
  .long vect2
  .long vect3
  .long vect4
  .long vect5
  .long vect6
  .long vect7
  .long vect8
  .long vect9
  .long vect10
  .long vect11
  .long vect12
  .long vect13
  .long vect14
  .long vect15
  .long vect16
  .long vect17
  .long vect18
  .long vect19
  .long vect20
  .long vect21
  .long vect22
  .long vect23
  .long vect24
  .long vect25
  .long vect26
  .long vect27
  .long vect28
  .long vect29
  .long vect30
  .long vect31
  .long vect32
  .long vect33
  .long vect34
  .long vect35
  .long vect36
  .long vect37
  .long vect38
  .long vect39
  .long vect40
  .long vect41
  .long vect42
  .long vect43
  .long vect44
  .long vect45
  .long vect46
  .long vect47
  .long vect48
  .long vect49
  .long vect50
  .long vect51
  .long vect52
  .long vect53
  .long vect54
  .long vect55
  .long vect56
  .long vect57
  .long vect58
  .long vect59
  .long vect60
  .long vect61
  .long vect62
  .long vect63
  .long vect64
  .long vect65
  .long vect66
  .long vect67
  .long vect68
  .long vect69
  .long vect70
  .long vect71
  .long vect72
  .long vect73
  .long vect74
  .long vect75
  .long vect76
  .long vect77
  .long vect78
  .long vect79
  .long vect80
  .long vect81
  .long vect82
  .long vect83
  .long vect84
  .long vect85
  .long vect86
  .long vect87
  .long vect88
  .long vect89
  .long vect90
  .long vect91
  .long vect92
  .long vect93
  .long vect94
  .long vect95
  .long vect96
  .long vect97
  .long vect98
  .long vect99
  .long vect100
  .long vect101
  .long vect102
  .long vect103
  .long vect104
  .long vect105
  .long vect106
  .long vect107
  .long vect108
  .long vect109
  .long vect110
  .long vect111
  .long vect112
  .long vect113
  .long vect114
  .long vect115
  .long vect116
  .long vect117
  .long vect118
  .long vect119
  .long vect120
  .long vect121
  .long vect122
  .long vect123
  .long vect124
  .long vect125
  .long vect126
  .long vect127
  .long vect128
  .long vect129
  .long vect130
  .long vect131
  .long vect132
  .long vect133
  .long vect134
  .long vect135
  .long vect136
  .long vect137
  .long vect138
  .long vect139
  .long vect140
  .long vect141
  .long vect142
  .long vect143
  .long vect144
  .long vect145
  .long vect146
  .long vect147
  .long vect148
  .long vect149
  .long vect150
  .long vect151
  .long vect152
  .long vect153
  .long vect154
  .long vect155
  .long vect156
  .long vect157
  .long vect158
  .long vect159
  .long vect160
  .long vect161
  .long vect162
  .long vect163
  .long vect164
  .long vect165
  .long vect166
  .long vect167
  .long vect168
  .long vect169
  .long vect170
  .long vect171
  .long vect172
  .long vect173
  .long vect174
  .long vect175
  .long vect176
  .long vect177
  .long vect178
  .long vect179
  .long vect180
  .long vect181
  .long vect182
  .long vect183
  .long vect184
  .long vect185
  .long vect186
  .long vect187
  .long vect188
  .long vect189
  .long vect190
  .long vect191
  .long vect192
  .long vect193
  .long vect194
  .long vect195
  .long vect196
  .long vect197
  .long vect198
  .long vect199
  .long vect200
  .long vect201
  .long vect202
  .long vect203
  .long vect204
  .long vect205
  .long vect206
  .long vect207
  .long vect208
  .long vect209
  .long vect210
  .long vect211
  .long vect212
  .long vect213
  .long vect214
  .long vect215
  .long vect216
  .long vect217
  .long vect218
  .long vect219
  .long vect220
  .long vect221
  .long vect222
  .long vect223
  .long vect224
  .long vect225
  .long vect226
  .long vect227
  .long vect228
  .long vect229
  .long vect230
  .long vect231
  .long vect232
  .long vect233
  .long vect234
  .long vect235
  .long vect236
  .long vect237
  .long vect238
  .long vect239
  .long vect240
  .long vect241
  .long vect242
  .long vect243
  .long vect244
  .long vect245
  .long vect246
  .long vect247
  .long vect248
  .long vect249
  .long vect250
  .long vect251
  .long vect252
  .long vect253
  .long vect254
  .long vect255

/*
 * Lab 3: Your code here for _alltraps
 */
.text
.globl _alltraps

#Build Trapframe
_alltraps:
    pushl %ds;
    pushl %es;
    pushal;

#Setup kernel stack to use
movw $(GD_KD), %ax
movw %ax, %ds
movw %ax, %es

#If it enables multiprocessor/mulitcore functionalities.
#movw $(GD_PERCPU),%ax
#movw %ax, %fs
#movw %ax, %gs
    
#Call trap(tf), tf is the current esp, trapframe address pointer,
pushl %esp
call trap
add $4, %esp #Why
