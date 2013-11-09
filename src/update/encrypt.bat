del /Q .\encrypted\*
copy /B .\decrypted\*.* .\encrypted\

for %%A in (".\encrypted\*") do encrypt.exe encrypt %%A %%A
for %%A in (".\encrypted\*") do gzip.exe -f %%A