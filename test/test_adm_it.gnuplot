# Network file: ../runs/six_node_net.dot
# Traffic matrix file: ../runs/six_node_net.dot
# TLL = 5
# MLL = 1000

set terminal postscript eps
set boxwidth 0.8
set size 0.65, 0.5
set border 15 lw 0.5
set xtics ("0" 0, "10" 10, "20" 20, "30" 30)
set style histogram clustered gap 1
set style data histograms

set output "test_adm_it.eps"
set xlabel "number of packets"
set ylabel "probability"
set key right

plot [-0.5:30.5]'-' using 2 title "analysis" fs solid 1.00 lt -1 lw 0.5, '-' using 2 title "simulation" fs solid 0.25 lt -1 lw 0.5
0 0
1 0
2 4.51772e-05
3 0.00022115
4 0.000811924
5 0.0023847
6 0.00583675
7 0.0122451
8 0.0224781
9 0.036678
10 0.0538634
11 0.0719101
12 0.0880029
13 0.0994128
14 0.10428
15 0.102094
16 0.0937062
17 0.0809483
18 0.0660425
19 0.0510456
20 0.0374815
21 0.0262112
22 0.0174965
23 0.0111715
24 0.0068358
25 0.00401549
26 0.00226805
27 0.00123361
28 0.000647006
29 0.000327641
30 0.000160386
31 7.59788e-05
32 3.48683e-05
33 1.55169e-05
e
0 1.05117e-06
1 1.34319e-05
2 8.6922e-05
3 0.000379578
4 0.00125763
5 0.00337042
6 0.00760709
7 0.014866
8 0.0256672
9 0.0397596
10 0.0559262
11 0.072129
12 0.085978
13 0.0953542
14 0.0989508
15 0.0965453
16 0.088941
17 0.0776484
18 0.0644519
19 0.0510116
20 0.0385977
21 0.0279852
22 0.0194847
23 0.0130526
24 0.00842767
25 0.00525333
26 0.00316616
27 0.0018476
28 0.00104525
29 0.000573987
30 0.000306294
31 0.000158998
32 8.03697e-05
33 3.95952e-05
34 1.90293e-05
35 8.92874e-06
36 4.09332e-06
37 1.83482e-06
38 8.04698e-07
39 3.45517e-07
40 1.4533e-07
41 5.99137e-08
42 2.42214e-08
43 9.6067e-09
44 3.7397e-09
45 1.4294e-09
46 5.36636e-10
47 1.97949e-10
48 7.17633e-11
49 2.55764e-11
50 8.96331e-12
51 3.08946e-12
52 1.04753e-12
53 3.49458e-13
54 1.1472e-13
55 3.70646e-14
56 1.17873e-14
57 3.69029e-15
58 1.13748e-15
59 3.45233e-16
60 1.03183e-16
61 3.03725e-17
62 8.80573e-18
63 2.5148e-18
64 7.07514e-19
65 1.96109e-19
66 5.35589e-20
67 1.44137e-20
68 3.82265e-21
69 9.99169e-22
70 2.57417e-22
71 6.5373e-23
72 1.63667e-23
73 4.03981e-24
74 9.83195e-25
75 2.35958e-25
76 5.58453e-26
77 1.30356e-26
78 3.0013e-27
79 6.81648e-28
80 1.52729e-28
81 3.37621e-29
82 7.3642e-30
83 1.58506e-30
84 3.3669e-31
85 7.05854e-32
86 1.46062e-32
87 2.98355e-33
88 6.01645e-34
89 1.19783e-34
90 2.3547e-35
91 4.57081e-36
92 8.76205e-37
93 1.65885e-37
94 3.10195e-38
95 5.72953e-39
96 1.04543e-39
97 1.88451e-40
98 3.35628e-41
99 5.90623e-42
100 1.02704e-42
101 1.76489e-43
102 2.99734e-44
103 5.03122e-45
104 8.34757e-46
105 1.36907e-46
106 2.21975e-47
107 3.55813e-48
108 5.6391e-49
109 8.83684e-50
110 1.36934e-50
111 2.09838e-51
112 3.18009e-52
113 4.76659e-53
114 7.06669e-54
115 1.03632e-54
116 1.50335e-55
117 2.1575e-56
118 3.06328e-57
119 4.30322e-58
120 5.98133e-59
121 8.22664e-60
122 1.11968e-60
123 1.50811e-61
124 2.01032e-62
125 2.65224e-63
126 3.46337e-64
127 4.47655e-65
128 5.72754e-66
129 7.25428e-67
130 9.0958e-68
131 1.12909e-68
132 1.38763e-69
133 1.68848e-70
134 2.0343e-71
135 2.42685e-72
136 2.86681e-73
137 3.35349e-74
138 3.88465e-75
139 4.45634e-76
140 5.06278e-77
141 5.69633e-78
142 6.34759e-79
143 7.00553e-80
144 7.65775e-81
145 8.29085e-82
146 8.89082e-83
147 9.4436e-84
148 9.93557e-85
149 1.03541e-85
150 1.06881e-86
151 1.09286e-87
152 1.10689e-88
153 1.11052e-89
154 1.10363e-90
155 1.08644e-91
156 1.05942e-92
157 1.02331e-93
158 9.79097e-95
159 9.27936e-96
160 8.71127e-97
161 8.10047e-98
162 7.46106e-99
163 6.80682e-100
164 6.15086e-101
165 5.50512e-102
166 4.8801e-103
167 4.28461e-104
168 3.72567e-105
169 3.20847e-106
170 2.7364e-107
171 2.31119e-108
172 1.9331e-109
173 1.6011e-110
174 1.31316e-111
175 1.06643e-112
176 8.57522e-114
177 6.8272e-115
178 5.38151e-116
179 4.19963e-117
180 3.24447e-118
181 2.48132e-119
182 1.87847e-120
183 1.40763e-121
184 1.04403e-122
185 7.66394e-124
186 5.56776e-125
187 4.0029e-126
188 2.84779e-127
189 2.00471e-128
190 1.39629e-129
191 9.62181e-131
192 6.55933e-132
193 4.42338e-133
194 2.95059e-134
195 1.94665e-135
196 1.27016e-136
197 8.19571e-138
198 5.22916e-139
199 3.29881e-140
200 2.05742e-141
201 1.26849e-142
202 7.73055e-144
203 4.65636e-145
204 2.77173e-146
205 1.63033e-147
206 9.47488e-149
207 5.43995e-150
208 3.08522e-151
209 1.72821e-152
210 9.56019e-154
211 5.22206e-155
212 2.8162e-156
213 1.49923e-157
214 7.8776e-159
215 4.08485e-160
216 2.09e-161
217 1.05496e-162
218 5.25256e-164
219 2.57917e-165
220 1.24878e-166
221 5.96088e-168
222 2.80461e-169
223 1.30044e-170
224 5.94127e-172
225 2.67394e-173
226 1.18527e-174
227 5.17352e-176
228 2.22313e-177
229 9.40288e-179
230 3.91359e-180
231 1.60255e-181
232 6.45463e-183
233 2.55654e-184
234 9.95534e-186
235 3.81047e-187
236 1.43323e-188
237 5.29627e-190
238 1.92236e-191
239 6.85182e-193
240 2.39763e-194
241 8.23495e-196
242 2.77548e-197
243 9.17727e-199
244 2.97635e-200
245 9.46558e-202
246 2.95123e-203
247 9.01882e-205
248 2.70076e-206
249 7.92334e-208
250 2.27675e-209
251 6.40621e-211
252 1.76466e-212
253 4.75758e-214
254 1.25507e-215
255 3.23882e-217
256 8.17392e-219
257 2.01684e-220
258 4.86391e-222
259 1.14613e-223
260 2.63802e-225
261 5.92878e-227
262 1.30059e-228
263 2.78375e-230
264 5.8111e-232
265 1.18259e-233
266 2.34504e-235
267 4.52887e-237
268 8.51372e-239
269 1.557e-240
270 2.7684e-242
271 4.78251e-244
272 8.02161e-246
273 1.30533e-247
274 2.05915e-249
275 3.14626e-251
276 4.65212e-253
277 6.65046e-255
278 9.18269e-257
279 1.22339e-258
280 1.57104e-260
281 1.94257e-262
282 2.31038e-264
283 2.64033e-266
284 2.89654e-268
285 3.04752e-270
286 3.07254e-272
287 2.96628e-274
288 2.74038e-276
289 2.42138e-278
290 2.04541e-280
291 1.65126e-282
292 1.27366e-284
293 9.38463e-287
294 6.60452e-289
295 4.439e-291
296 2.84914e-293
297 1.74622e-295
298 1.0219e-297
299 5.70957e-300
300 3.0453e-302
301 1.55032e-304
302 7.53124e-307
303 3.49055e-309
304 1.54253e-311
305 6.49986e-314
306 2.60792e-316
307 9.97054e-319
308 3.6215e-321
309 9.88131e-324
310 0
311 0
312 0
313 0
314 0
315 0
316 0
317 0
318 0
319 0
320 0
321 0
322 0
323 0
324 0
325 0
326 0
327 0
328 0
329 0
330 0
331 0
332 0
333 0
334 0
335 0
336 0
337 0
338 0
339 0
340 0
341 0
342 0
343 0
344 0
345 0
346 0
347 0
348 0
349 0
350 0
351 0
352 0
353 0
354 0
355 0
356 0
357 0
e