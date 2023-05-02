gcc -o .\waveform_maker .\waveform_maker.c
.\waveform_maker.exe
iverilog.exe -o cic_dec_tb.vvp -y../../rtl/common -y../../rtl/ -y. cic_dec_tb.v
vvp cic_dec_tb.vvp
gtkwave.exe wave.vcd