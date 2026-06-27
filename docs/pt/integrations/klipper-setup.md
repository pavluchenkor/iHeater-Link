# Configuração do Klipper para iHeater Link

## Para que serve

Este recurso foi desenvolvido para impressoras Klipper com sistemas fechados: Creality, Qidi, Flashforge e outros modelos modernos, onde o usuário não consegue compilar e instalar o firmware iHeater para trabalhar diretamente com o Klipper.

Arquivos de configuração normais do Klipper e macros G-code personalizadas geralmente estão disponíveis. Portanto, iHeater Link utiliza um caminho mais simples: ele se conecta à mesma rede Wi-Fi que a impressora, lê as variáveis da macro Klipper personalizada e transmite a temperatura alvo para o controlador iHeater.

No lado da impressora, você precisa apenas adicionar algumas macros G-code. Elas aceitam comandos de temperatura de câmara padrão `M141` e `M191` e armazenam a temperatura alvo em `VIRTUAL_CHAMBER`.

O esquema resultante:

```text
Fatiador / G-code -> M141 S50 -> Macro Klipper VIRTUAL_CHAMBER.target=50
                                      |
                                      v
Impressora na rede local <- Wi-Fi <- iHeater Link -> linha de sinal -> iHeater
```

O usuário não precisa obter acesso root ou intervir no firmware interno da impressora. É suficiente ter acesso às macros personalizadas do Klipper.

## O que você obterá

```text
M141 S50 -> target = 50 -> iHeater Link ativa o aquecimento
M141 S0  -> target = 0  -> iHeater Link desativa o aquecimento
```

Muitas impressoras modernas já possuem um sensor de temperatura dentro da câmara. Se esse sensor é fornecido pelo fabricante e visível na configuração do Klipper, ele pode ser usado para transmitir a temperatura real para o portal e iHeater Link. Se não houver sensor, iHeater Link ainda poderá controlar o aquecimento de acordo com a temperatura alvo.

## 1. Adicione o arquivo de macros

Crie o arquivo `virtual_chamber.cfg` na configuração do Klipper e inclua-o em `printer.cfg`:

```ini
[include virtual_chamber.cfg]
```

Conteúdo do `virtual_chamber.cfg`:

```ini
[gcode_macro VIRTUAL_CHAMBER]
variable_target: 0
variable_temperature: -1
variable_has_sensor: 0
gcode:

[gcode_macro M141]
gcode:
  {{ "{%" }} set t = params.S|default(0)|float {{ "%}" }}
  SET_GCODE_VARIABLE MACRO=VIRTUAL_CHAMBER VARIABLE=target VALUE={t}

[gcode_macro M191]
gcode:
  {{ "{%" }} set t = params.S|default(0)|float {{ "%}" }}
  SET_GCODE_VARIABLE MACRO=VIRTUAL_CHAMBER VARIABLE=target VALUE={t}

[gcode_macro CLEAR_VIRTUAL_CHAMBER]
gcode:
  SET_GCODE_VARIABLE MACRO=VIRTUAL_CHAMBER VARIABLE=target VALUE=0
  SET_GCODE_VARIABLE MACRO=VIRTUAL_CHAMBER VARIABLE=temperature VALUE=-1
  SET_GCODE_VARIABLE MACRO=VIRTUAL_CHAMBER VARIABLE=has_sensor VALUE=0
```

Após salvar, reinicie o Klipper ou execute `RESTART`.

## 2. Opcionalmente conecte o sensor da câmara

Abra a configuração da impressora e verifique se há um objeto semelhante a um sensor de temperatura de câmara. Diferentes fabricantes e configurações podem nomeá-lo de forma diferente, por exemplo:

```ini
[temperature_sensor chamber]
```

```ini
[temperature_sensor enclosure]
```

```ini
[temperature_sensor chamber_temp]
```

```ini
[heater_generic chamber]
```

Se esse objeto existir, você pode transmitir a temperatura real para iHeater Link. Adicione o bloco abaixo em `virtual_chamber.cfg` e substitua `heater_generic chamber` pelo nome do objeto da sua configuração:

```ini
[delayed_gcode UPDATE_VIRTUAL_CHAMBER_TEMP]
initial_duration: 1.0
gcode:
  {{ "{%" }} set t = printer["heater_generic chamber"].temperature|float {{ "%}" }}
  SET_GCODE_VARIABLE MACRO=VIRTUAL_CHAMBER VARIABLE=temperature VALUE={t}
  SET_GCODE_VARIABLE MACRO=VIRTUAL_CHAMBER VARIABLE=has_sensor VALUE=1
  UPDATE_DELAYED_GCODE ID=UPDATE_VIRTUAL_CHAMBER_TEMP DURATION=2.0
```

Por exemplo, se o seu sensor for descrito como `[temperature_sensor enclosure]`, a linha de leitura de temperatura deve fazer referência a `printer["temperature_sensor enclosure"].temperature`.

Se não houver sensor ou se não tiver certeza, ignore esta etapa. Para controlar o aquecimento, `target` fornecido pelas macros `M141` e `M191` é suficiente.

## 3. Ative a integração Klipper em iHeater Link

No portal, abra o dispositivo iHeater Link e clique em **MOONRAKER** no bloco **Device Info**. Na interface, este nome é usado para impressoras Klipper.

![Seleção de Moonraker em Device Info](../../img/iheater-link-settings-01.png)

Em seguida, clique no ícone de engrenagem no cartão do dispositivo, abra as configurações de conexão e ative **MOONRAKER**.

![Configurações do dispositivo](../../img/iheater-link-settings-03.png)

![Ativação de Moonraker nas configurações do dispositivo](../../img/iheater-link-settings-04.png)

Retorne às configurações de **MOONRAKER**, especifique o endereço IP da impressora e salve as configurações.

![Retorno às configurações de Moonraker](../../img/iheater-link-settings-05.png)

![Configurações de Moonraker](../../img/iheater-link-settings-02.png)

Geralmente, esses parâmetros são suficientes:

- Host: endereço IP da impressora na rede local;
- Port: `7125`;
- API key: deixe em branco se a impressora não o exigir;
- Use SSL (wss): desativado para conexão local comum;
- Poll interval: `1000`.

Após salvar, iHeater Link começará a ler `VIRTUAL_CHAMBER.target` do Klipper e transmiti-lo para iHeater.

## 4. Controle manual a partir do portal

O aquecimento pode ser ativado sem um fatiador: defina a temperatura da câmara no cartão do dispositivo e clique em **START**. No campo de tempo, você pode especificar a duração do aquecimento em minutos. Se deixar o tempo em `0`, iHeater funcionará sem limite de tempo até você clicar em **STOP** ou enviar um comando de desativação.

![Ativação manual de iHeater](../../img/iheater-link-settings-03.png)

## 5. Verifique o funcionamento das macros

No console do Klipper, execute:

```gcode
M141 S50
```

iHeater Link deve receber `target=50` e ativar o aquecimento de iHeater.

Em seguida, execute:

```gcode
M141 S0
```

Target se torna `0` e iHeater Link desativa o aquecimento.

## 6. Configure o fatiador

O fatiador não precisa saber sobre `VIRTUAL_CHAMBER`. Ele deve enviar comandos de temperatura de câmara padrão:

- `M141 S{T}` — definir temperatura da câmara sem espera;
- `M191 S{T}` — definir temperatura da câmara com espera.

As macros Klipper interceptarão esses comandos e registrarão o valor em `VIRTUAL_CHAMBER.target`.

### OrcaSlicer / Bambu Studio

A temperatura da câmara é definida no perfil de filamento:

```text
Filament Settings -> Temperatures -> Chamber temperature
```

Por exemplo:

- ABS / ASA: `40-50 °C`;
- PLA: `0 °C`.

Verifique o início do G-code. Deve aparecer uma linha semelhante a:

```gcode
M141 S45
```

### PrusaSlicer / SuperSlicer

Se há um campo de temperatura de câmara no perfil, use-o. Se o campo não existir, adicione o comando manualmente no Start G-code:

```gcode
M141 S45 ; chamber temperature for this filament
```

## 7. Certifique-se de desativar o aquecimento no final da impressão

Após o término da impressão, o target não é redefinido automaticamente. Adicione ao End G-code:

```gcode
CLEAR_VIRTUAL_CHAMBER
```

ou:

```gcode
M141 S0
```

Isso redefinirá `VIRTUAL_CHAMBER.target` para `0`, após o qual iHeater Link desativará iHeater.
