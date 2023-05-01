iverilog -y../../rtl/common -y../../rtl/ -y. -o fx2.vvp .\fx2_tb.v
vvp fx2.vvp
gtkwave wave.gtkw