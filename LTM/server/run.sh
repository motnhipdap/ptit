# Tạo database MySQL trong Docker
docker-compose -f docker-compose.mysql.yml up -d

mvn clean compile exec:java -Dexec.mainClass="com.oop.game.HelloApplication" -Dexec.args="--add-exports javafx.base/com.sun.javafx.runtime=ALL-UNNAMED"
