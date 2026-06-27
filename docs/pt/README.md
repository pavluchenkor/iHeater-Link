# iHeater Link — Guia rápido

iHeater Link é um módulo de conectividade para o controlador **iHeater** com firmware [iheater_revХ_Х_pulse](https://github.com/pavluchenkor/iHeater-Standalone-Firmware/releases). É uma placa em ESP32-C3 / ESP32-S3 que:

- Se conecta ao Wi-Fi e vincula o iHeater ao [portal.idryer.org](https://portal.idryer.org/).
- Recebe a temperatura alvo da câmara da impressora por meio de integrações **Moonraker (Klipper)**, **Bambu Lab (LAN)** ou **Home Assistant**.
- Converte a temperatura alvo em um sinal de pulso e o transmite ao controlador iHeater em um GPIO.

O gerenciamento do iHeater é feito "por fio": um pino de sinal ESP → entrada de sinal iHeater. Wi-Fi e integrações são responsabilidade do Link, aquecimento e segurança são responsabilidade do iHeater.

A conexão de um único fio não impõe limitações na colocação do Link. A placa ESP pode ser removida da câmara térmica. Isso evita:

- superaquecimento do chip e periféricos quando a câmara funciona acima de 60 °C;
- congelamentos térmicos da seção de rádio e interrupção da sessão Wi-Fi durante o aquecimento prolongado;
- degradação acelerada.

Apenas o iHeater permanece dentro da câmara, projetado para operar em altas temperaturas. O comprimento do fio de sinal para o ESP é limitado apenas pela carga razoável na linha (dezenas de centímetros — sem restrições).

## Placas suportadas

| Placa                        |       |
| ---------------------------- | ----- |
| ESP32-C3 Super Mini          | `✅`  |
| ESP32-C3 DevKitM-1           | `✅`  |
| Seeed XIAO ESP32-S3          | `✅`  |
| Waveshare ESP32-S3-Zero      | `✅`  |

Qualquer outra placa em ESP32-C3 ou ESP32-S3 pode ser usada se
houver um GPIO livre para a saída de sinal. Consulte o pinout
do fabricante.

## Esquema de conexão

!!! warning "Nunca conecte ou desconecte fios enquanto a alimentação estiver ligada."

A alimentação é fornecida ao ESP através de USB-C. O ESP, por sua vez, alimenta o controlador iHeater pela linha 5 V. Esta é a opção mais simples. Se necessário, a alimentação do iHeater pode ser organizada de forma diferente — a conexão com o Link não depende do esquema de alimentação.

![Conexão do ESP32-C3 Super Mini ao iHeater](../img/iHeaterLink.png)

Conexões (para todas as placas suportadas):

| ESP        | iHeater          | Finalidade            |
| ---------- | ---------------- | --------------------- |
| `5V`       | `5V`             | alimentação do controlador   |
| `GND`      | `GND`            | terra comum           |
| `GPIO3`    | entrada de sinal  | setpoint de pulso   |

### Pinout das placas

ESP32-C3 Super Mini:

![Pinout do ESP32-C3 Super Mini](../img/ESP32-C3-Super-Mini-pinout-low.jpg)

Waveshare ESP32-S3-Zero:

![Pinout do Waveshare ESP32-S3-Zero](../img/ESP32-C3-ZERO-Waveshare-pinout-low.jpg)

## Gravação através do gravador web

O gravador web está localizado em [install.idryer.org](https://install.idryer.org/).

1. Conecte o Link à porta USB do computador.
2. Abra [install.idryer.org](https://install.idryer.org/) e selecione o dispositivo **iHeater Link**.
3. Selecione a variante da placa.
4. Clique em **Connect**, selecione a porta serial (geralmente `USB JTAG/serial` ou `CH340`). Se o dispositivo não for detectado, pressione o botão `BOOT` na placa e clique rapidamente em `RST`.
5. Clique em **Install**. O gravador registrará o firmware.
6. Após a conclusão da gravação, o assistente de configuração Wi-Fi abrirá.

## Configuração de Wi-Fi

Após a gravação, o assistente Improv abre automaticamente na porta serial.

1. Insira o SSID e a senha de sua rede 2,4 GHz.
2. Aguarde o status **Connected**. O indicador Link passará para o modo de "respiração" azul.

Se o assistente não abrir, desconecte o USB e conecte novamente através de **Connect** sem refazer a gravação.

!!! note "O ESP32 suporta apenas 2,4 GHz. Redes 5 GHz não funcionam."

## Vinculação ao portal

1. Na página do gravador, clique em **Conectar e executar Claim**.
2. O comando `START_CLAIM` será enviado para o dispositivo. Após alguns segundos, um PIN aparecerá na página. O PIN é válido por ~5 minutos.
3. Abra [portal.idryer.org](https://portal.idryer.org/) → **Adicionar dispositivo** → insira o PIN.
4. Após a vinculação bem-sucedida, o dispositivo aparecerá na lista online.

Se você receber `CLAIM_ALREADY:DEVICE_…` em resposta, o dispositivo já está vinculado a esta ou outra conta. Neste caso, remova o dispositivo no portal e repita a vinculação.

## Conexão com iHeater

1. Desconecte a alimentação do controlador.
2. Conecte o ESP ao iHeater de acordo com o esquema acima: `5V`, `GND`, `GPIO3` → entrada de sinal iHeater.
3. Forneça alimentação ao USB do ESP. O controlador será alimentado pela linha 5 V.

Após a inicialização, o Link estabelecerá uma conexão com o portal, ativará a integração selecionada e começará a transmitir a temperatura alvo da câmara ao iHeater.

## O que deve acontecer

- LED 1 acende continuamente, LED 3 pisca brevemente 1 vez por segundo, indicando conectividade entre iHeater Link e iHeater.
- Em caso de perda de conexão, todos os LEDs piscam com frequência de 1 Hz.
- Outros erros não relacionados ao iHeater Link repetem a indicação do firmware standalone.

## Diagnóstico

No menu do dispositivo há uma opção **DIAGNOSTICS → DIAG LOG**. Quando habilitada, um relatório detalhado é emitido na porta serial a cada segundo: status do Wi-Fi, MQTT, integração ativa, alvo atual, erros de conector.

Detalhes sobre diagnóstico estão na documentação da biblioteca idryer-core, disponível no repositório do projeto no GitHub.
