del /Q .\decrypted\*.*
copy .\encrypted\*.* .\decrypted\

for %%A in (".\decrypted\*") do gunzip.exe -f %%A
for %%A in (".\decrypted\*") do encrypt.exe decrypt %%A %%A
