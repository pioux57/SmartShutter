@echo off
cls
pushd %~dp0
echo Lancement de l'UI SmartShutter...

set __M2_REPO=%USERPROFILE%\.m2\repository

set MVN_HOME=F:\Softwares\apache-maven-3.6.3

rem Check if dependencies are present (uncomment if you want to use maven dependencies)
@REM set __DL_DEP=
@REM if not exist %__M2_REPO%\org\apache\groovy\groovy\4.0.10\groovy-4.0.10.jar ( set __DL_DEP=1)
@REM if not exist %__M2_REPO%\org\codehaus\groovy\groovy-json\2.5.6\groovy-json-2.5.6.jar ( set __DL_DEP=1)
@REM if not exist %__M2_REPO%\org\apache\groovy\groovy-swing\4.0.10\groovy-swing-4.0.10.jar ( set __DL_DEP=1)
@REM if defined __DL_DEP (
@REM   echo Missing Maven dependency, trying to download them
@REM   if not defined MVN_HOME (
@REM     echo *** MVN_HOME environment variable is not set ***
@REM     exit /b 1
@REM   )
@REM   call "%MVN_HOME%\bin\mvn" -B -ntp dependency:get -Dartifact=org.apache.groovy:groovy:4.0.10
@REM   call "%MVN_HOME%\bin\mvn" -B -ntp dependency:get -Dartifact=org.codehaus.groovy:groovy-json:2.5.6
@REM   call "%MVN_HOME%\bin\mvn" -B -ntp dependency:get -Dtransitive=false -Dartifact=org.apache.groovy:groovy-swing:4.0.10
@REM )
@REM set __DL_DEP=
@REM set __SCRIPT_CP=%__SCRIPT_CP%;%__M2_REPO%\org\apache\groovy\groovy\4.0.10\groovy-4.0.10.jar
@REM set __SCRIPT_CP=%__SCRIPT_CP%;%__M2_REPO%\org\codehaus\groovy\groovy-json\2.5.6\groovy-json-2.5.6.jar
@REM set __SCRIPT_CP=%__SCRIPT_CP%;%__M2_REPO%\org\apache\groovy\groovy-swing\4.0.10\groovy-swing-4.0.10.jar
@REM set __SCRIPT_CP=%__SCRIPT_CP%;D:\Projets\Arduino\SmartShutter\UI\jSerialComm-2.9.3.jar;.

rem Comment if you use the maven dependancies
set __SCRIPT_CP=UI/lib/groovy-4.0.10.jar;UI/lib/groovy-json-2.5.6.jar;UI/lib/groovy-swing-4.0.10.jar;UI/lib/jSerialComm-2.9.3.jar;.

cls
call java -cp %__SCRIPT_CP% groovy.ui.GroovyMain UI/SmartShutter.groovy %1 %2

set __SCRIPT_CP=
set __M2_REPO=

popd
