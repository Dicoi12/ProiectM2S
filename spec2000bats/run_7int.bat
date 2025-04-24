sim-outorder.exe -redir:sim ./results/bzip2.res -fastfwd 300000000 -max:inst 1000000000 bzip2.arg

sim-outorder.exe -redir:sim ./results/gcc.res -fastfwd 300000000 -max:inst 1000000000 gcc.arg

sim-outorder.exe -redir:sim ./results/gzip.res -fastfwd 300000000 -max:inst 1000000000 gzip.arg

sim-outorder.exe -redir:sim ./results/mcf.res -fastfwd 300000000 -max:inst 1000000000 mcf.arg

sim-outorder.exe -redir:sim ./results/parser.res -fastfwd 300000000 -max:inst 1000000000 parser.arg

sim-outorder.exe -redir:sim ./results/twolf.res -fastfwd 300000000 -max:inst 1000000000 twolf.arg

sim-outorder.exe -redir:sim ./results/vpr.res -fastfwd 300000000 -max:inst 1000000000 vpr.arg

