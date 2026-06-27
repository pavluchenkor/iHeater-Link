# Conectando iHeater Link ao Home Assistant

iHeater Link publica o dispositivo no Home Assistant via MQTT Discovery: HA cria automaticamente um cartão com sensores reais e elementos de controle (temperatura alvo, duração, modo IDLE/DRYING/STORAGE).

!!! note
    O dispositivo **não aparecerá** em `Settings → Devices & services → Discovered`. iHeater Link usa MQTT Discovery, não UPnP/zeroconf. No Home Assistant, deve haver **já adicionada** a integração **MQTT** apontando para seu broker.

## O que deve estar pronto

1. MQTT-broker (por exemplo, complemento Mosquitto) executado em HA ou acessível pela rede.
2. No HA, adicionada a integração **MQTT** com o broker configurado.
3. iHeater Link recebeu do portal o comando `link_integration {type:"ha"}` e estabeleceu conexão com o mesmo broker.

## Passo 1. Abrir configurações

No menu lateral do Home Assistant, no rodapé, clique em **Settings**.

![Settings no menu lateral](../../img/HA-integration-01.png)

## Passo 2. Ir para Devices & services

Na lista de seções de configurações, selecione **Devices & services**.

![Devices & services](../../img/HA-integration-02.png)

## Passo 3. Abrir integração MQTT

Na lista de integrações, encontre o cartão **MQTT**. Abaixo do nome — contador de dispositivos conectados.

![MQTT na lista de integrações](../../img/HA-integration-03.png)

## Passo 4. Encontrar dispositivo iDryer

Na página de integração, na seção **Services**, expanda o nó do broker (`127.0.0.1` ou endereço do seu broker). Sob ele estão listados os dispositivos iDryer com seus números de série no formato `DEVICE_*`.

![Dispositivos MQTT](../../img/HA-integration-04.png)

Clique no dispositivo desejado.

## Passo 5. Controle e estado

Na página do dispositivo, há dois blocos:

- **Controls** — elementos de controle:
  - `iDryer U1 duration` — duração em minutos
  - `iDryer U1 mode control` — modo (`IDLE` / `DRYING` / `STORAGE`)
  - `iDryer U1 target temp` — temperatura alvo (controle deslizante)
- **Sensors** — valores reais. A composição depende do tipo de dispositivo (`Config` define quais sensores são publicados):
  - iHeater Link: `heater_power`, `mode`, `alerts`
  - Storage Link: o mesmo mais `temperature`, `humidity`

![Página do dispositivo em HA](../../img/HA-integration-05.png)

Para iniciar o aquecimento:

1. Defina a temperatura alvo com o controle deslizante.
2. Defina a duração.
3. Selecione o modo `DRYING` ou `STORAGE` no seletor.

Para parar — mude o seletor para `IDLE`.

!!! note
    Os valores `target temp` e `duration` são primeiro salvos no dispositivo como "pendentes", o início real ocorre ao selecionar o modo. Isso permite definir parâmetros em qualquer ordem e iniciar com uma única ação.

## O que acontece nos bastidores

- **Discovery** (criação de entidade na HA UI com ícones corretos) — publicado automaticamente ao conectar ao broker HA. A composição é determinada pelos sinalizadores `Config.hasXxx` — sensores ausentes não aparecem como fantasmas.
- **State** (valores atuais) — publicado em tópicos HA a cada 5 segundos, paralelo à publicação no portal.
- **Commands** (`set_temp` / `set_duration` / `set_mode`) — vêm de HA → MQTT-broker → dispositivo → são coletados em `Request` e passam pelo mesmo caminho que os comandos do portal. Nenhuma ramificação específica de HA no código do produto.

## Diagnóstico

| Sintoma | O que verificar |
|---------|-----------------|
| Dispositivo não aparece em HA | No dispositivo no portal — `Home Assistant → Ativado: sim`. O campo `ha.state` em `integrations/status` deve ser `online`. |
| Discovery publicado, mas cartão vazio | Aguarde 5–10 segundos após a primeira conexão. Se os valores não aparecerem — verifique se o MQTT-broker não está perdendo mensagens retained. |
| Botões de controle não respondem | Verifique `command_topic` do Discovery — o tópico deve coincidir com `idryer/{serial}/U1/set_mode`, etc. |
| Sensores fantasma com valor Unknown | Discovery retained antigo de versão anterior do firmware. Após a atualização, aguarde o próximo ciclo de publicação do Discovery ou limpe os retained: `mosquitto_pub -t 'homeassistant/.../config' -n -r`. |
