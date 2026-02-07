#!/bin/bash

echo "🚀 Starting Game Server..."

# Compile project
echo "🔨 Compiling project..."
mvn clean compile

# Chạy server với đầy đủ dependencies
echo "🖥️ Starting server on port 3009..."
mvn exec:java -Dexec.mainClass="server.ServerMain"
