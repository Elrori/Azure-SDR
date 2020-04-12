iverilog -y./common -y. -o fir_dec.vvp .\fir_dec_tb.v
vvp fir_dec.vvp
gtkwave wave2.gtkw