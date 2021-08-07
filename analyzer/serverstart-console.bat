@echo off 
title Analyzer Console 
:start 
echo Starting Packet Analyzer. 
echo. 
REM ------------------------------------- 
REM Default parameters for a basic server. 
java -cp analyzer.jar;lib\log4j-1.2.16.jar Analyzer.Analyzer
REM  2> .\log\error.log 
REM > .\log\stdout.log
REM ------------------------------------- 
pause 

