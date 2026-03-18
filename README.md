# S_C_Irrigacao_por_umidade
## Sistema de Irrigação por sensor de umidade 🌱
ESP32 + Interface Web + Sensor de Umidade Solo
Painel de controle para uma zona de irrigação, apresentando modo automático baseado em sensor de umidade (HW-080), operação manual via interface web e configuração de parâmetros em tempo real sem necessidade de novo upload de código.

## 📌 Visão Geral
Projeto de controlador de irrigação embarcado focado em praticidade e autonomia:

- Modo Automático: Acionamento inteligente baseado na leitura analógica do solo.

- Modo Manual: Controle direto do relé via interface web.

- Interface Web Responsiva: Hospedada no próprio ESP32, permitindo configurar tempos e limites pelo celular ou PC.

- Operação Local: Cria seu próprio ponto de acesso (Access Point), funcionando mesmo sem roteador externo.

- Monitoramento em Tempo Real: Leitura de umidade e status do relé atualizados via AJAX (sem recarregar a página).

## 🎯 Objetivos do Projeto
- Facilidade de Ajuste: Alterar o limite de umidade e tempos de rega via navegador.

- Confiabilidade: Lógica de pausa entre irrigações para evitar encharcamento e picos de consumo.

- Acessibilidade: Interface amigável para usuários sem conhecimento técnico em programação.

- Dualidade de Operação: Flexibilidade para alternar entre automação total e controle manual.

## 🧠 Arquitetura do Sistema
Fluxo de Dados:

- Entrada: Sensor de Umidade Solo (Analógico) → ESP32.

- Processamento: Lógica de comparação (Umidade Atual vs. Limite Configurado).

- Saída: ESP32 → Módulo Relé → Válvula Solenóide / Bomba d'água.

- Interface: WebServer (Porta 80) executando rotas HTTP para controle e configuração.

## 🔌 Hardware Utilizado
- ESP32 ou ESP8266

- Módulo Relé 1 Canal (Pino Digital 16)

- Sensor de Umidade de Solo HW-080 (Pino Analógico 34)

- Fonte de Alimentação (5V para o ESP32 / Tensão da bomba para o relé)

- Válvula Solenóide ou Mini Bomba submersa

## ⚙️ Principais Funcionalidades
- Controle de Irrigação Inteligente

- Leitura Analógica: Monitoramento constante do solo.

- Limite Configurável: Ajuste do "setpoint" de umidade via interface (0 a 4095).

- Ciclos Protegidos: Tempo de rega definido pelo usuário com trava de segurança (Pausa) para estabilização da água no solo.

- Interface Web Dinâmica

- AJAX/Fetch API: Atualização do valor de umidade a cada 1 segundo sem "refresh" de página.

- Configuração On-the-fly: Formulário para salvar Tempo Ligado, Tempo de Pausa e Umidade Limite.

- Alternância de Modos: Botões dedicados para trocar entre Automático e Manual.

- Rede e Conectividade

- SoftAP: O ESP32 cria a rede IRRIGACAO_AP (Senha: 12345678).

- IP Padrão: Acesso via 192.168.4.1 no navegador.

## 🌐 Interface Web
- Design Clean: Focado em usabilidade com CSS moderno incorporado.

- Componentes: Mostrador de umidade em tempo real, formulário de parâmetros, seletores de modo e botões de acionamento manual (ON/OFF).

- Lógica de Visibilidade: A seção de controle manual aparece apenas quando o modo automático está desativado, evitando conflitos de lógica.

## 🛡️ Mecanismos de Segurança e Estabilidade
- Prevenção de Loop: O sistema aguarda o tempoPausa após cada irrigação, evitando ciclos infinitos caso o sensor demore a detectar a água.

- Estado Inicial Seguro: O relé inicia sempre em estado HIGH (desligado para módulos de nível baixo) para evitar regas acidentais em caso de reboot.

- Controle de Fluxo: Uso de millis() em vez de delay() na lógica principal para manter o servidor web sempre responsivo.

## 📈 Escalabilidade e Futuro
- Implementação de persistência de dados (EEPROM/SPIFFS) para salvar configurações após queda de energia.

- Adição de múltiplos sensores para média aritmética de umidade.

- Integração com sensores de nível de reservatório.

- Conexão com API de previsão do tempo para suspender irrigação em dias de chuva.

## 📊 Diferenciais Técnicos
- Interatividade Total: Comunicação assíncrona entre o frontend e o hardware.

- Portabilidade: Não depende de internet ou infraestrutura de rede existente.

- Código Modular: Funções separadas para controle de hardware, configuração de rede e rotas do servidor.

## 👨‍💻 Autor
Marcos Gabriel Ferreira Miranda

IoT Developer | Automação Residencial e Agrícola

Belo Horizonte - MG
