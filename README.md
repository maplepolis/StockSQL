# StockSQL

StockSQL is a stock trading simulator implemented in C with a PostgreSQL backend, designed to simulate buy and sell operations using historical market data. The system is intended for testing trading strategies and studying market behavior under realistic conditions.

---

## Tech Stack

* **Languages:** C, bash
* **Database:** PostgreSQL
* **Containerization:** Docker
* **Deployment:** AWS Lightsail

## Getting Started

### Prerequisites

Install the following before running the project:

* C/C++ compiler (e.g., gcc, clang)
* **PostgreSQL** server
* **Docker** & **Docker Compose**
* Bash shell utilities

### Installation

1. **Clone the repository**

   ```bash
   git clone https://github.com/maplepolis/StockSQL.git
   cd StockSQL
   ```

2. **Install dependencies**

   ```bash
   ./InstallDeps.sh
   ```

3. **Build the project**

   ```bash
   make
   ```

4. **Run database containers**

   ```bash
   docker compose up -d
   ```

---

If you want, I can help add **schema diagrams**, **example SQL queries**, or **API/CLI documentation** to this README too — just tell me what you need!

[1]: https://github.com/maplepolis/StockSQL "GitHub - maplepolis/StockSQL"
