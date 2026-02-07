@echo off
echo 🚀 Starting Game Server...

:: Compile project
echo 🔨 Compiling project...
call mvn clean compile -Dfile.encoding=UTF-8

:: Run server
echo 🖥️ Starting server on port 3009...
call mvn exec:java -Dexec.mainClass="server.Main"

pause
