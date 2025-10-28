# an esoteric table based synthesizer

# (nonsensical) features

    4 voices (F[1-4])
    26 table lookup oscillators (a-z)
    26 tables (A-Z)
    10+26+26=62 table types (0-9a-zA-Z)

# approximate grammar is listed below

    I  := [1-9][0-9]*
    FN := [0-9]*.[0-9][0-9]*
    D  := [0-9]
    SA := [a-z]
    BA := [A-Z]
    AN := D | SA | BA
    K  := "F1"|"F2"|"F3"|"F4"

    OF := SA | FN

# vox_ctl: vox oscil ratemod ampmod

    V  := K SA OF OF

# tlo_ctl: tlo table rate(mod) amp(mod)

    O  := SA BA OF OF

# tab_ctl: tab type size

    T  := BA AN I
