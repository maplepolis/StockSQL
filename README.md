StockSQL is a stock trading simulator built with C and PostgreSQL that allows users to buy and sell stocks using historical market data to test trading strategies in a realistic environment. The system is designed with a microservice-style architecture, running multiple PostgreSQL databases in Docker containers to support scalable transactions and data queries.

The project focuses on database design and performance optimization. SQL queries and schemas were carefully redesigned to improve efficiency, reducing query latency by approximately 50%. StockSQL runs in a Linux environment and includes Bash scripts for automation and deployment, with cloud deployment supported via AWS Lightsail.

Tech stack: C, PostgreSQL, SQL, Docker, Bash, Linux, AWS Lightsail
