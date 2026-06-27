# iHeater Link — Guia rápido

iHeater Link é um módulo de comunicação para o controlador **iHeater** com o firmware [iheater_revХ_Х_pulse](https://github.com/pavluchenkor/iHeater-Standalone-Firmware/releases). É uma placa baseada em ESP32-C3 / ESP32-S3 que:

- Conecta-se ao Wi-Fi e vincula o iHeater ao [portal.idryer.org](https://portal.idryer.org/).
- Recebe a temperatura alvo da câmara da impressora através de
  integrações com **Moonraker (Klipper)**, **Bambu Lab (LAN)** ou
  **Home Assistant**.
- Converte a temperatura alvo em um sinal de pulso e o transmite ao controlador iHeater em um GPIO.

O controle do iHeater é feito "por fio": um pino de sinal ESP → entrada de sinal iHeater. Wi-Fi e integrações são responsabilidade do Link, aquecimento e segurança são responsabilidade do iHeater.

A conexão de um único fio não impõe restrições quanto ao posicionamento do Link. A placa ESP pode ser colocada fora da câmara térmica. Isto evita:

- superaquecimento do chip e periféricos durante o funcionamento da câmara a 60+ °C;
- travamentos térmicos da seção de rádio e interrupção de sessão Wi-Fi durante aquecimento prolongado;
- degradação acelerada.

Dentro da câmara permanece apenas o iHeater, projetado para funcionar em altas temperaturas. O comprimento do fio de sinal até o ESP é limitado apenas pela carga razoável na linha (dezenas de centímetros — sem ressalvas).

## Placas suportadas

| Placa                        |       |
| ---------------------------- | ----- |
| ESP32-C3 Super Mini          | `✅`  |
| ESP32-C3 DevKitM-1           | `✅`  |
| Seeed XIAO ESP32-S3          | `✅`  |
| Waveshare ESP32-S3-Zero      | `✅`  |

Qualquer outra placa baseada em ESP32-C3 ou ESP32-S3 pode ser usada se
houver um GPIO livre para a saída de sinal. Verifique o pinout
do fabricante.

## Esquema de conexão

!!! warning "Nunca conecte ou desconecte cabos com energia fornecida."

A energia é fornecida ao ESP através de USB-C. O ESP, por sua vez, alimenta o controlador iHeater através da linha 5 V. Esta é a opção mais simples. Se necessário, a alimentação do iHeater pode ser organizada de outra forma — a comunicação com o Link não depende do esquema de alimentação.

![Conexão ESP32-C3 Super Mini ao iHeater](../img/iHeaterLink.png)

Conexões (para todas as placas suportadas):

| ESP        | iHeater          | Propósito             |
| ---------- | ---------------- | --------------------- |
| `5V`       | `5V`             | alimentação do controlador   |
| `GND`      | `GND`            | terra comum           |
| `GPIO3`    | entrada de sinal  | setpoint de pulso   |

### Pinout das placas

ESP32-C3 Super Mini:

![Pinout ESP32-C3 Super Mini](../img/ESP32-C3-Super-Mini-pinout-low.jpg)

Waveshare ESP32-S3-Zero:

![Pinout Waveshare ESP32-S3-Zero](../img/ESP32-C3-ZERO-Waveshare-pinout-low.jpg)

## Flash do firmware via flasher web

O flasher web está localizado em [install.idryer.org](https://install.idryer.org/).

1. Conecte o Link a uma porta USB do seu computador.
2. Abra [install.idryer.org](https://install.idryer.org/) e selecione o dispositivo **iHeater Link**.
3. Selecione a variante da placa.
4. Clique em **Connect**, selecione a porta serial (geralmente `USB JTAG/serial`
   ou `CH340`). Se o dispositivo não for detectado, mantenha o botão `BOOT`
   pressionado e aperte rapidamente `RST`.
5. Clique em **Install**. O flasher gravará o firmware.
6. Após o flash ser concluído, o assistente de configuração de Wi-Fi será aberto.

## Configuração de Wi-Fi

Após o flash, o assistente Improv é aberto automaticamente na porta serial.

1. Digite o SSID e a senha de sua rede 2,4 GHz.
2. Aguarde o status **Connected**. O indicador de Link passará para
   o modo de "respiração" em azul.

Se o assistente não abrir, desconecte o USB e conecte novamente através de
**Connect** sem fazer flash novamente.

!!! note "O ESP32 suporta apenas 2,4 GHz. Redes 5 GHz não funcionam."

## Vinculação ao portal

1. Na página do flasher, clique em **Conectar e executar Claim**.
2. Um comando `START_CLAIM` será enviado ao dispositivo. Após alguns segundos,
   um PIN aparecerá na página. O PIN é válido por aproximadamente 5 minutos.
3. Abra [portal.idryer.org](https://portal.idryer.org/) → **Adicionar
   dispositivo** → digite o PIN.
4. Após a vinculação bem-sucedida, o dispositivo aparecerá na lista online.

Se você receber `CLAIM_ALREADY:DEVICE_…` — o dispositivo já está vinculado a esta ou outra conta. Neste caso, remova o dispositivo no portal e repita a vinculação.

## Conexão ao iHeater

1. Desative a alimentação do controlador.
2. Conecte o ESP ao iHeater de acordo com o esquema acima: `5V`, `GND`, `GPIO3` →
   entrada de sinal iHeater.
3. Forneça energia à USB do ESP. O controlador será alimentado através da linha 5 V.

Após o carregamento, o Link estabelecerá conexão com o portal, ativará
a integração selecionada e começará a transmitir a temperatura alvo da
câmara ao iHeater.

## O que você deve ver

- LED 1 acende continuamente, LED 3 pisca brevemente 1 vez por segundo, indicando que há comunicação entre iHeater Link - iHeater.
- quando a comunicação é perdida, todos os LEDs piscam a 1 Hz.
- os demais erros relacionados a iHeater Link repetem a indicação do firmware standalone.

## Diagnóstico

No menu do dispositivo há um item **DIAGNOSTICS → DIAG LOG**. Quando ativado, um relatório detalhado é enviado para a porta serial uma vez por segundo: status de Wi-Fi, MQTT, integração ativa, setpoint atual, erros de conectores.

Mais detalhes sobre diagnóstico estão disponíveis na documentação da biblioteca idryer-core, encontrada no repositório do projeto no GitHub.
