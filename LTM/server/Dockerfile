# Stage 1: Build with Maven
FROM maven:3.9.9-eclipse-temurin-21 AS build
WORKDIR /app
COPY pom.xml .
COPY src ./src
RUN mvn clean package -DskipTests

# Stage 2: Run app
FROM eclipse-temurin:21-jdk
WORKDIR /app
COPY --from=build /app/target/game-1.0-SNAPSHOT.jar game.jar
CMD ["java", "-jar", "game.jar"]
