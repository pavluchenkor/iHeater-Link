# Home Assistant

O iHeater Link publica-se no Home Assistant através de **MQTT Discovery**: o HA cria as entidades por si — temperatura, potência de aquecimento, campos de arranque e botões. O portal não é necessário para isto, tudo passa pelo seu broker MQTT.

A seguir: como ativar a integração, como verificar e uma disposição de cartão pronta a usar, para que o dispositivo fique com bom aspeto e não como uma lista de entidades.

![Cartão do iHeater Link no Home Assistant](../../img/iheater-ha-card.png)
*Temperatura da câmara, potência de aquecimento e arranque do aquecimento num único bloco.*

!!! note
    O dispositivo **não aparece** em `Settings → Devices & services → Discovered`: isto é MQTT Discovery, não UPnP/zeroconf. A integração **MQTT** no Home Assistant tem de estar adicionada previamente.

## O que é necessário

1. Um broker MQTT: o add-on **Mosquitto broker** no Home Assistant ou qualquer broker na sua rede.
2. No Home Assistant, a integração **MQTT** adicionada e a apontar para esse broker.
3. O iHeater Link na rede e `Online` no portal.

!!! info "iHeater Link — módulo de comunicação do controlador iHeater; grave no controlador o firmware [iheater_revX_X_pulse](https://github.com/pavluchenkor/iHeater-Standalone-Firmware/releases)."

## Passo 1. Ativar a integração no dispositivo

Abra o dispositivo em [portal.idryer.org](https://portal.idryer.org/) e encontre o bloco **Integrações** → **Home Assistant**.

| Campo | O que introduzir |
|---|---|
| Host | o endereço do broker na sua rede, por exemplo `192.168.1.27` |
| Port | a porta do broker, normalmente `1883` |
| Username / Password | as credenciais do broker, se este as exigir |
| Discovery prefix | `homeassistant`, se não o tiver alterado nas definições do HA |
| Ativado | a caixa de verificação — caso contrário o dispositivo não se liga ao broker |

As definições vão diretamente para o dispositivo através da rede local — o portal não as guarda. O Home Assistant é ativado pelo seu próprio interruptor e não interfere com as integrações de impressora: Bambu Lab e Moonraker são selecionadas à parte, e de cada vez funciona apenas uma delas.

![Janela do Home Assistant no bloco «Integrações» do portal](../../img/iheater-ha-portal-integration.png)
*O endereço do broker, a porta e a marca «Ativado» — tudo o que o dispositivo precisa.*

## Passo 2. Encontrar o dispositivo no Home Assistant

No menu lateral, em baixo, clique em **Settings**.

![Settings no menu lateral](../../img/HA-integration-01.png)

Selecione **Devices & services**.

![Devices & services](../../img/HA-integration-02.png)

Encontre o cartão **MQTT**. Por baixo do nome está o contador de dispositivos ligados.

![MQTT na lista de integrações](../../img/HA-integration-03.png)

Na secção **Services**, expanda o nó do broker. Os dispositivos iDryer aparecem com números de série no formato `DEVICE_*`.

![Dispositivos MQTT](../../img/HA-integration-04.png)

Abra o dispositivo: o HA já mostra as leituras e os elementos de controlo.

![Página do dispositivo no HA](../../img/HA-integration-05.png)

## Passo 3. Montar o cartão

O HA dispõe as entidades por si, e o resultado é uma lista longa. A disposição pronta coloca as leituras em cima e o arranque do aquecimento num bloco separado.

1. `Settings` → `Dashboards` → **Add dashboard** → um painel vazio, abra-o.
2. Canto superior direito → o lápis (**Edit**) → menu «⋮» → **Raw configuration editor**.
3. Cole o conteúdo abaixo e guarde.

A disposição destina-se a um painel do tipo `sections`.

```yaml
title: iDryer
views:
- title: Devices
  path: devices
  type: sections
  max_columns: 4
  sections:
  - type: grid
    background: true
    cards:
    - type: heading
      heading: iHeater Link
      heading_style: title
      icon: mdi:radiator
      badges:
      - type: entity
        entity: sensor.iheater_link_mode
        show_icon: false
        show_state: true
        color: primary
    - type: tile
      entity: sensor.iheater_link_temperature
      name: Temperature
      visibility:
      - condition: state
        entity: sensor.iheater_link_temperature
        state_not:
        - unknown
        - unavailable
    - type: tile
      entity: sensor.iheater_link_heater_power
      name: Heater power
    - type: heading
      heading: Heat
      heading_style: subtitle
    - type: tile
      entity: number.iheater_link_heat_temperature
      name: Temperature
      features:
      - type: numeric-input
        style: buttons
      features_position: bottom
    - type: tile
      entity: number.iheater_link_heat_duration
      name: Duration
      features:
      - type: numeric-input
        style: buttons
      features_position: bottom
    - type: tile
      entity: button.iheater_link_heat
      name: Start heating
      icon: mdi:play
      hide_state: true
      tap_action: &id001
        action: perform-action
        perform_action: button.press
        target:
          entity_id: button.iheater_link_heat
      icon_tap_action: *id001
    - type: tile
      entity: button.iheater_link_stop
      name: Stop
      icon: mdi:stop
      hide_state: true
      tap_action: &id002
        action: perform-action
        perform_action: button.press
        target:
          entity_id: button.iheater_link_stop
      icon_tap_action: *id002
```

![Raw configuration editor com a disposição colada](../../img/iheater-ha-raw-editor.png)
*A mesma disposição no editor de configuração do painel.*

A ordem de arranque é a mesma que na aplicação: primeiro definem-se a temperatura e a duração, depois carrega-se em **Iniciar aquecimento**. O botão **Parar** desliga o aquecimento.

## Se os nomes das entidades não coincidirem

A disposição destina-se aos identificadores padrão do tipo `sensor.iheater_link_temperature`. Se o cartão mostrar «Entity not found», consulte os seus: `Settings` → `Devices & services` → **MQTT** → o seu dispositivo → lista de entidades — e substitua o prefixo na disposição pelo seu.

## O que acontece nos bastidores

- O conjunto de entidades é declarado pelo próprio dispositivo — a partir da descrição do seu cartão: leituras, campos de parâmetros e botões de ações. O que o dispositivo não tem, não aparece no Home Assistant.
- Os valores são publicados em MQTT juntamente com a telemetria normal; o HA recebe-os em tempo real.
- Carregar num botão no HA chega ao dispositivo como a mesma ação que vem do portal ou da aplicação — no firmware não há lógica separada «para o Home Assistant».

## Diagnóstico

| Sintoma | O que verificar |
|---|---|
| O dispositivo não apareceu no HA | No portal, a integração Home Assistant tem a caixa «Ativado» marcada e o endereço e a porta do broker estão corretos. O dispositivo tem de estar `Online`. |
| Apareceu, mas os valores são `Unknown` | Aguarde um ciclo de telemetria. Se continuar vazio — o broker não guarda mensagens retained ou o dispositivo não se ligou a ele. |
| Não há temperatura da câmara | O sensor não está ligado ao controlador iHeater: sem ele o dispositivo não publica este valor. |
| Os botões não funcionam | Verifique que o broker permite a publicação nos tópicos `idryer/#` e que o registo do dispositivo não tem erros de autorização. |
| Entidades fantasma com o valor `Unknown` | Ficaram mensagens retained de um firmware anterior. Limpe-as: `mosquitto_pub -h <broker> -t 'homeassistant/<...>/config' -n -r`. |
| Juntamente com o Home Assistant desapareceu o Bambu ou o Moonraker | O Home Assistant não tem nada a ver com isso — é ativado à parte. Verifique a seleção da integração de impressora: funciona apenas uma delas. |
