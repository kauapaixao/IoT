# Monitoramento da Qualidade do Ar em Ambientes Escolares

> Projeto IoT alinhado ao ODS 3 – Saúde e Bem-Estar (ONU Agenda 2030)
> Universidade Presbiteriana Mackenzie — Faculdade de Computação e Informática

**Autores:** Julia D'agrela Araújo · Kauã Paixão · Pedro Henrique Gonçalves · Rafael Carvalho Cordeiro · Wallace Santana

---

## Visão Geral

Sistema de monitoramento ambiental em tempo real para salas de aula, utilizando ESP32 simulados no Wokwi com sensores DHT22 (temperatura e umidade) e MQ135 (CO₂). Os dados são transmitidos via MQTT ao Node-RED, armazenados no InfluxDB Cloud e visualizados em dashboards Grafana.  

ESP32 Sala 101 (Wokwi)
ESP32 Sala 202 (Wokwi)  

-> HiveMQ (MQTT) -> Node-RED (Railway) -> InfluxDB Cloud -> Grafana  

-> Comandos de volta (NORMAL / ATENÇÃO / CRÍTICO)  

-> LED RGB + Buzzer em cada sala  

---

## Conexões entre os Serviços

### ESP32 → HiveMQ
O ESP32 conecta ao broker HiveMQ via Wi-Fi usando o protocolo MQTT com TLS na porta 8883. A cada 5 segundos publica um JSON com temperatura, umidade e CO₂ no tópico escola/salaXXX/dados. Também fica escutando o tópico `escola/salaXXX/atuador` para receber comandos de volta.

### HiveMQ → Node-RED
O Node-RED se conecta ao mesmo broker HiveMQ como cliente MQTT. Ele assina (subscribe) os tópicos de dados das salas e recebe as mensagens em tempo real assim que o ESP32 publica.

### Node-RED → ESP32
Após processar os dados, o Node-RED publica de volta no tópico escola/salaXXX/atuador com o status NORMAL, ATENÇAO ou CRITICO. O ESP32 recebe esse comando e aciona o LED RGB e o buzzer de acordo.

### Node-RED → InfluxDB
O Node-RED monta os dados no formato Line Protocol e envia via HTTP POST para a API do InfluxDB Cloud, gravando no bucket "escola_ar" com os campos temperatura, umidade e co2, e a tag sala.

### InfluxDB → Grafana
O Grafana conecta ao InfluxDB Cloud como data source usando SQL. Consulta o bucket `escola_ar` e exibe os dados em 3 dashboards com gauges, tabelas e gráficos de série temporal.

## Demonstração

[![Assistir demonstração](https://img.shields.io/badge/▶%20Ver%20vídeo-FF0000?style=for-the-badge&logo=youtube)](https://youtu.be/QU0PB8yhGLA)

## Arquitetura e Componentes

| Componente | Tipo | Função |
|---|---|---|
| ESP32 + DHT22 + MQ135 | Hardware simulado (Wokwi) | Coleta temperatura, umidade e CO₂ |
| Buzzer + LED RGB | Atuador simulado (Wokwi) | Alerta local sonoro e visual |
| HiveMQ Cloud | Broker MQTT (Cloud) | Roteamento de mensagens MQTT |
| Node-RED | Plataforma low-code (Railway) | Orquestração, regras de negócio, integração |
| InfluxDB Cloud | Banco de dados (Cloud) | Armazenamento de séries temporais |
| Grafana Cloud | Dashboard (Cloud) | Visualização e análise gerencial |

---

## Pré-requisitos

- Conta no [Wokwi](https://wokwi.com) (gratuita)
- Conta no [HiveMQ Cloud](https://www.hivemq.com/mqtt-cloud-broker/) (gratuita)
- Node-RED rodando no [Railway](https://railway.app)
- Conta no [InfluxDB Cloud](https://cloud2.influxdata.com) (gratuita)
- Conta no [Grafana Cloud](https://grafana.com) (gratuita)

---

## Passo a Passo de Inicialização

### 1. ESP32 — Wokwi

1. Acesse os projetos:
   - Sala 101: https://wokwi.com/projects/465295995304550401
   - Sala 202: https://wokwi.com/projects/465296228219524097
2. Substitua no código as credenciais do HiveMQ:
   - `mqtt_server` -> URL do seu broker
   - `mqtt_user` / `mqtt_pass` -> suas credenciais
3. Clique em **Play** em cada aba
4. Verifique no **Serial Monitor** os JSONs sendo publicados a cada 5 segundos

### 2. HiveMQ Cloud — Broker MQTT

1. Acesse [hivemq.com](https://www.hivemq.com/mqtt-cloud-broker/) e crie uma conta
2. Crie um cluster e um usuário em **Access Management**
3. Anote: URL do broker, porta `8883`, usuário e senha

### 3. Node-RED — Flow

1. Acesse sua instância do Node-RED
2. Instale o pacote: Menu ☰ -> **Manage Palette** -> pesquise `node-red-contrib-influxdb`
3. Importe o arquivo `nodered/flows.json` deste repositório via Menu ☰ -> **Import**
4. Configure o broker HiveMQ: duplo clique em qualquer nó `mqtt in` -> lápis -> preencha server, porta `8883`, TLS ativado, usuário e senha
5. Clique em **Deploy**

### 4. InfluxDB Cloud

1. Acesse [cloud2.influxdata.com](https://cloud2.influxdata.com) e crie uma conta
2. Crie um bucket chamado `escola_ar`
3. Gere um token em **Load Data -> API Tokens -> All Access**
4. Anote: URL, Org ID, bucket e token
5. Atualize o token no nó HTTP Request do Node-RED

### 5. Grafana Cloud — Dashboards

1. Acesse [grafana.com](https://grafana.com) e crie uma conta
2. Vá em **Connections -> Data sources -> Add data source -> InfluxDB**
3. Configure:
   - **Product:** `InfluxDB Cloud Serverless`
   - **Query language:** `SQL`
   - **URL:** `https://us-east-1-1.aws.cloud2.influxdata.com`
   - **Database:** `escola_ar`
   - **Token:** seu token do InfluxDB
4. Importe os dashboards da pasta `grafana/` deste repositório

---

## Fluxo Completo dos Dados
1. ESP32 (Wokwi)
Lê DHT22 (temp + umidade) e MQ135 (CO₂) a cada 5s
Publica JSON via MQTT em escola/salaXXX/dados
2. HiveMQ Cloud
Recebe e repassa a mensagem ao Node-RED
3. Node-RED (Railway)
Classifica: NORMAL / ATENÇÃO / CRÍTICO
Publica comando de volta -> ESP32 aciona LED RGB + Buzzer
Envia dados via HTTP POST para o InfluxDB
4. InfluxDB Cloud
Armazena no bucket escola_ar
Measurement: qualidade_ar | Tags: sala | Fields: temperatura, umidade, co2
5. Grafana Cloud
Consulta o InfluxDB via SQL
Exibe 3 dashboards com gauges, tabelas e gráficos temporais
---

## Tópicos MQTT

| Tópico | Direção | Conteúdo |
|---|---|---|
| `escola/sala101/dados` | ESP32 -> Node-RED | JSON com temp, umidade, CO₂ |
| `escola/sala202/dados` | ESP32 -> Node-RED | JSON com temp, umidade, CO₂ |
| `escola/sala101/atuador` | Node-RED -> ESP32 | `NORMAL`, `ATENCAO` ou `CRITICO` |
| `escola/sala202/atuador` | Node-RED -> ESP32 | `NORMAL`, `ATENCAO` ou `CRITICO` |

---

## Thresholds de Qualidade do Ar

| Métrica | Normal | Atenção | Crítico |
|---|---|---|---|
| CO₂ | < 1000 ppm | 1000 – 2000 ppm | > 2000 ppm |
| Temperatura | 18 – 24 °C | 24 – 28 °C | > 28 °C |
| Umidade | 40 – 70 % | < 40% ou > 70% | — |

---

## Dashboards
![Dashboard monitoramento](projeto%20iot/prints%20projeto%20rodando/dashboard%20monitoramento%20em%20tempo%20real.png)

![Dashboard](projeto%20iot/prints%20projeto%20rodando/dashboard2.png)

![Dashboard](projeto%20iot/prints%20projeto%20rodando/dashboard3.png)

## Estrutura do Repositório
```
/
README.md
projeto iot/
-wokwi/
-sala 101 Copy.zip
-sala 202 Copy.zip
-nodered/
-flows.json
-grafana/
-dashboard1.json
-dashboard2.json
-dashboard3.json
```
---
