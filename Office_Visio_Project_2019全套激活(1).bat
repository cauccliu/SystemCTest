@ECHO OFF&PUSHD %~DP0

setlocal EnableDelayedExpansion&color 3e & cd /d "%~dp0"
title office2019 ���۰�ת��vol��

%1 %2
mshta vbscript:createobject("shell.application").shellexecute("%~s0","goto :runas","","runas",1)(window.close)&goto :eof
:runas

if exist "%ProgramFiles%\Microsoft Office\Office16\ospp.vbs" cd /d "%ProgramFiles%\Microsoft Office\Office16"
if exist "%ProgramFiles(x86)%\Microsoft Office\Office16\ospp.vbs" cd /d "%ProgramFiles(x86)%\Microsoft Office\Office16"

:WH
cls
echo.
echo                    ��ѡ��Ҫת���İ汾��Ų�����
echo.
echo --------------------------------------------------------------------------------                                              
echo              1. Office2019 ת��ΪVOL�沢����
echo.
echo              2. Visio2019 ת��ΪVOL�沢����
echo.
echo              3. Project2019 ת��ΪVOL�沢����
echo.
echo. --------------------------------------------------------------------------------
                                              
set /p tsk="��������Ҫת����office�汾��š��س���ȷ�ϣ�1-3��: "
if not defined tsk goto:err
if %tsk%==1 goto:1
if %tsk%==2 goto:2
if %tsk%==3 goto:3

:err
goto:WH

:1
cls

echo ��������Office2019���ۼ���...
cscript ospp.vbs /rearm

echo ���ڰ�װ KMS ���֤...
for /f %%x in ('dir /b ..\root\Licenses16\proplus2019vl_kms*.xrm-ms') do cscript ospp.vbs /inslic:"..\root\Licenses16\%%x" >nul

echo ���ڰ�װ MAK ���֤...
for /f %%x in ('dir /b ..\root\Licenses16\proplus2019vl_mak*.xrm-ms') do cscript ospp.vbs /inslic:"..\root\Licenses16\%%x" >nul

echo ���ڰ�װ KMS ��Կ...
cscript ospp.vbs /inpkey:NMMKJ-6RK4F-KMJVX-8D9MJ-6MWKP

goto :e

:2
cls

echo ��������Visio2019���ۼ���...
cscript ospp.vbs /rearm

echo ���ڰ�װ KMS ���֤...
for /f %%x in ('dir /b ..\root\Licenses16\visiopro2019vl_kms*.xrm-ms') do cscript ospp.vbs /inslic:"..\root\Licenses16\%%x" >nul

echo ���ڰ�װ MAK ���֤...
for /f %%x in ('dir /b ..\root\Licenses16\visiopro2019vl_mak*.xrm-ms') do cscript ospp.vbs /inslic:"..\root\Licenses16\%%x" >nul

echo ���ڰ�װ KMS ��Կ...
cscript ospp.vbs /inpkey:9BGNQ-K37YR-RQHF2-38RQ3-7VCBB

goto :e

:3
cls

echo ��������Project2019���ۼ���...
cscript ospp.vbs /rearm

echo ���ڰ�װ KMS ���֤...
for /f %%x in ('dir /b ..\root\Licenses16\projectpro2019vl_kms*.xrm-ms') do cscript ospp.vbs /inslic:"..\root\Licenses16\%%x" >nul

echo ���ڰ�װ MAK ���֤...
for /f %%x in ('dir /b ..\root\Licenses16\projectpro2019vl_mak*.xrm-ms') do cscript ospp.vbs /inslic:"..\root\Licenses16\%%x" >nul

echo ���ڰ�װ KMS ��Կ...
cscript ospp.vbs /inpkey:B4NPR-3FKK7-T2MBV-FRQ4W-PKD2B

goto :e

:e
cscript //nologo ospp.vbs /sethst:192.168.1.109
cscript //nologo ospp.vbs /act
echo.
echo ת����ɣ���������˳���