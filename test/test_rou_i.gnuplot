# Network file: ../runs/six_node_net.dot
# Traffic matrix file: ../runs/six_node_net.dot
# TLL = 5
# MLL = 1000

# Preferences from node b to nodes:
# a: cls = 2: 0 4 2 | dest = 0
# c: cls = 2: 2 4 0 | dest = 2
# d: cls = 3: 2 4 0 | dest = 3
# e: cls = 1: 4 0 2 | dest = 4
# f: cls = 3: 2 4 0 | dest = 5

set terminal postscript eps
set boxwidth 0.8
set size 0.65, 0.5
set border 15 lw 0.5
set xtics scale 0
set style histogram clustered gap 1
set style data histograms

set output "test_rou_i.eps"
set xlabel "number of packets"
set ylabel "probability"
set key right

plot [-0.5:9.5]'-' using 2:xtic(1) title "analysis" fs solid 1.00 lt -1 lw 0.5, '-' using 2 title "simulation" fs solid 0.25 lt -1 lw 0.5
0 0.0936295
1 0.221753
2 0.262601
3 0.207316
4 0.122752
5 0.0581455
6 0.022952
7 0.00776569
8 0.00229904
9 0.000605009
10 0.000143291
11 3.0852e-05
e
0 0.102543
1 0.223929
2 0.25429
3 0.199296
4 0.120708
5 0.0600248
6 0.0254372
7 0.0094193
8 0.00310247
9 0.000921014
10 0.000248927
11 6.17384e-05
12 1.41399e-05
13 3.00575e-06
14 5.95497e-07
15 1.10333e-07
16 1.91718e-08
17 3.13155e-09
18 4.81766e-10
19 6.99174e-11
20 9.58453e-12
21 1.24236e-12
22 1.52396e-13
23 1.77021e-14
24 1.94804e-15
25 2.0315e-16
26 2.00793e-17
27 1.88104e-18
28 1.67e-19
29 1.40479e-20
30 1.11933e-21
31 8.44536e-23
32 6.03191e-24
33 4.07727e-25
34 2.60812e-26
35 1.5789e-27
36 9.04686e-29
37 4.90565e-30
38 2.51517e-31
39 1.21664e-32
40 5.53176e-34
41 2.35281e-35
42 9.32001e-37
43 3.43033e-38
44 1.17336e-39
45 3.7334e-41
46 1.10462e-42
47 3.03828e-44
48 7.78634e-46
49 1.86192e-47
50 4.13622e-49
51 8.52515e-51
52 1.63759e-52
53 2.90846e-54
54 4.72363e-56
55 7.06458e-58
56 9.66416e-60
57 1.17456e-61
58 1.26923e-63
59 1.2298e-65
60 1.04658e-67
61 8.00486e-70
62 5.50995e-72
63 3.2939e-74
64 1.88976e-76
65 8.72048e-79
66 4.27791e-81
67 1.49963e-83
68 6.40612e-86
69 1.66567e-88
70 6.25879e-91
71 1.15845e-93
72 3.82906e-96
73 4.66282e-99
74 1.3307e-101
75 8.9032e-105
76 2.0082e-107
77 3.64067e-111
78 2.47596e-115
79 7.83529e-120
80 1.19914e-124
81 8.68033e-130
82 2.39233e-135
e

